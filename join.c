#include "join.h"

void query_succ(struct CTX *ctx, struct sockaddr_in *entry_addr, struct Node *result, uint32_t id) {
    struct Find_Succ *find_succ = (struct Find_Succ *) malloc(sizeof(struct Find_Succ));
    find_succ->type = FIND_SUCC_TYPE;
    find_succ->id = id;

    if (sendto(ctx->sockfd, (char *) find_succ, sizeof(struct Find_Succ), 0,
        (struct sockaddr *) entry_addr, sizeof(struct sockaddr)) < 0) {
        perror("ERROR sendto(): Find_Succ");
        free(find_succ);
        return;
    }

    free(find_succ);

    char recv_buf[BUF_SIZE];
    while (1) {
        bzero(recv_buf, BUF_SIZE);
        if (recvfrom(ctx->sockfd, recv_buf, BUF_SIZE, 0,
                (struct sockaddr *) NULL, NULL) < 0) {
            perror("ERROR recvfrom() query_succ");
        }

        uint32_t *msg_type = (uint32_t *) recv_buf;
        if (*msg_type != FIND_SUCC_ANS_TYPE) continue;

        memcpy(result, recv_buf + 4, NODE_SIZE);
        break;
    }

    return;
}

void set_pred(struct CTX *ctx, struct Node *dst, struct Node *pred) {
    printf("set_pred dst ID: %u, pred ID: %u\n", dst->id, pred->id);
    struct Set_Pred *msg = (struct Set_Pred *) malloc(sizeof(struct Set_Pred));
    msg->type = SET_PRED_TYPE;
    memcpy(&msg->pred, pred, NODE_SIZE);

    if (sendto(ctx->sockfd, (char *) msg, sizeof(struct Set_Pred), 0,
            (struct sockaddr *) &dst->addr, SOCKADDR_SIZE) < 0) {
        perror("ERROR sendto() set_pred");
    }

    free(msg);
    return;
}

void init_finger_table(struct CTX *ctx, char *entry_point) {
    // init finger entries

    // convert entry_point to sockaddr
    struct sockaddr_in entry_node;
    entry_node.sin_family = AF_INET;
    entry_node.sin_port = htons(ctx->port);
    entry_node.sin_addr.s_addr = inet_addr(entry_point);

    // finger[0].node = n_.find_successor(finger[0].start)
    query_succ(ctx, &entry_node, &ctx->finger[0].node, ctx->finger[0].start);
    ctx->local_succ = &ctx->finger[0].node;

    print_ctx(ctx);

    struct Get_Pred *get_pred = (struct Get_Pred *) malloc(sizeof(struct Get_Pred));
    get_pred->type = GET_PRED_TYPE;

    if (sendto(ctx->sockfd, (char *) get_pred, sizeof(struct Get_Pred), 0,
        (struct sockaddr *) &ctx->local_succ->addr, SOCKADDR_SIZE) < 0) {
        perror("ERROR sendto(): Get_Pred");
        free(get_pred);
        return;  
    }

    free(get_pred);

    char recv_buf[BUF_SIZE];

    while (1) {
        bzero(recv_buf, BUF_SIZE);
        if (recvfrom(ctx->sockfd, recv_buf, BUF_SIZE, 0,
                (struct sockaddr *) NULL, NULL) < 0) {
            perror("ERROR recvfrom() get_pred");
        }

        uint32_t *msg_type = (uint32_t *) recv_buf;
        if (*msg_type != GET_PRED_ANS_TYPE) continue;

        memcpy(ctx->local_pred, recv_buf + 4, NODE_SIZE);
        break;
    }

    print_ctx(ctx);

    //  successor.predecessor = local_node
    set_pred(ctx, ctx->local_succ, ctx->local_node);


    for (int i = 0; i < MAXM - 1; i++) {
        uint32_t finger_id = ctx->finger[i].node.id;
        if (finger_id <= ctx->local_id) finger_id += power(2, MAXM);

        if (ctx->local_id <= ctx->finger[i + 1].start && ctx->finger[i + 1].start < finger_id) {
            memcpy(&ctx->finger[i + 1].node, &ctx->finger[i].node, sizeof(struct Node));
        } else {
            query_succ(ctx, &entry_node, &ctx->finger[i + 1].node, ctx->finger[i + 1].start);
        }
    }

    print_ctx(ctx);

    printf("init_finger_table() finished\n");

    return;
}


void update_others(struct CTX *ctx) {
    printf("update_others\n");
    for (int i = 0; i < MAXM; i++) {
        struct Node *tmp = (struct Node *) malloc(NODE_SIZE);
        find_predecessor(ctx, tmp, (ctx->local_id + power(2, MAXM) - power(2, i)) % power(2, MAXM));
        update_finger_table(ctx, tmp, ctx->local_node, i);
    }
}

void update_finger_table(struct CTX *ctx, struct Node *dst, struct Node *node, uint32_t idx) {
    printf("update_finger_table DST id: %u, Finger ID: %u, finger idx: %u\n", dst->id, node->id, idx);
    struct Update_Finger *msg = (struct Update_Finger *) malloc(sizeof(struct Update_Finger));
    msg->type = UPDATE_FINGER_TYPE;
    msg->idx = idx;
    memcpy(&msg->node, node, sizeof(struct Node));

    if (sendto(ctx->sockfd, (char *) msg, sizeof(struct Update_Finger), 0,
        (struct sockaddr *) &dst->addr, sizeof(struct sockaddr_in)) < 0) {
        perror("ERROR sendto() update_finger_table");
    }

    free(msg);
    return;
}

void update_finger_table_handler(struct CTX *ctx, struct Node *node, uint32_t idx) {
    uint32_t finger_id = ctx->finger[idx].node.id;
    if (finger_id <= ctx->local_id) finger_id += power(2, MAXM);

    uint32_t node_id = node->id;
    if (node_id < ctx->local_id) node_id += power(2, MAXM);

    if (ctx->local_id < node_id && node_id < finger_id) {
        memcpy(&ctx->finger[idx].node, node, NODE_SIZE);
        print_ctx(ctx);
        update_finger_table(ctx, ctx->local_pred, node, idx);
    }
}