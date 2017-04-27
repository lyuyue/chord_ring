#include "stablize.h"

// stablization
void query_pred(struct CTX *ctx, struct Node *dst, struct Node *result) {
    if (dst->id == ctx->local_id) {
        memcpy(result, ctx->local_pred, NODE_SIZE);
        return;
    }
    
    uint32_t msg_len = sizeof(struct Get_Pred);
    struct Get_Pred *msg = (struct Get_Pred *) malloc(msg_len);
    msg->type = GET_PRED_TYPE;

    if (sendto(ctx->sockfd, (char *) msg, msg_len, 0,
            (struct sockaddr *) &dst->addr, SOCKADDR_SIZE) < 0) {
        perror("ERROR sendto() query_pred");
    }

    char recv_buf[BUF_SIZE];

    while (1) {
        bzero(recv_buf, BUF_SIZE);

        if (recvfrom(ctx->sockfd, recv_buf, BUF_SIZE, 0,
                (struct sockaddr *) NULL, NULL) < 0) {
            continue;
        }

        uint32_t *msg_type = (uint32_t *) recv_buf;
        if (*msg_type != GET_PRED_ANS_TYPE) continue;

        memcpy(result, recv_buf + 4, NODE_SIZE);
        break;
    }

    return;
}

void notify(struct CTX *ctx, struct Node *dst) {
    printf("notify %u to be pred of %u\n", ctx->local_id, dst->id);
    uint32_t msg_len = sizeof(struct Notify);
    struct Notify *msg = (struct Notify *) malloc(msg_len);
    msg->type = NOTIFY_TYPE;
    memcpy(&msg->node, ctx->local_node, NODE_SIZE);

    if (sendto(ctx->sockfd, (char *) msg, msg_len, 0,
            (struct sockaddr *) &dst->addr, SOCKADDR_SIZE) < 0) {
        perror("ERROR sendto() notify");
    }
}

void stablize(struct CTX *ctx) {
    printf("stablize\n");
    struct Node *tmp_node = (struct Node *) malloc(NODE_SIZE);
    query_pred(ctx, ctx->local_succ, tmp_node);

    if (inrange(ctx->local_node->id, ctx->local_succ->id, tmp_node->id)) {
        memcpy(ctx->local_succ, tmp_node, NODE_SIZE);
    }

    notify(ctx, ctx->local_succ);
}

void notify_handler(struct CTX *ctx, struct Node *node) {
    if (ctx->local_pred == NULL || inrange(ctx->local_pred->id, ctx->local_node->id, node->id)) {
        if (ctx->local_pred == NULL) {
            ctx->local_pred = (struct Node *) malloc(NODE_SIZE);
        } 
        memcpy(ctx->local_pred, node, NODE_SIZE);
    }

    print_ctx(ctx);

    return;
}

void fix_fingers(struct CTX *ctx) {
    printf("fix_fingers\n");
    for (int i = 1; i < MAXM; i++) {
        find_successor(ctx, &ctx->finger[i].node, ctx->finger[i].start);
    }
    return;
}