#include "query.h"

// ask node for id's successor
void find_successor(struct CTX *ctx, struct Node *result, uint32_t id) {
    find_predecessor(ctx, result, id);
    get_node_successor(ctx, result, result);
    return;
}

// ask node for id's predecessor
void find_predecessor(struct CTX *ctx, struct Node *result, uint32_t id) {
    printf("find_predecessor ID：%u\n", id);
    struct Node *cur_node = (struct Node *) malloc(NODE_SIZE);
    struct Node *succ_node = (struct Node *) malloc(NODE_SIZE);

    memcpy(cur_node, ctx->local_node, NODE_SIZE);

    while (1) {
        bzero(succ_node, NODE_SIZE);
        get_node_successor(ctx, cur_node, succ_node);
        uint32_t upper_bound = succ_node->id;
        if (upper_bound <= cur_node->id) upper_bound += power(2, MAXM);

        if (cur_node->id < id && id <= upper_bound) {
            break;
        }

        get_closest_preceding_finger(ctx, cur_node, id);
        printf("closest_preceding_finger %u %s\n", cur_node->id, inet_ntoa(cur_node->addr.sin_addr));
    }

    memcpy(result, cur_node, NODE_SIZE);

    printf("find_predecessor result %u\n", result->id);

    free(cur_node);
    free(succ_node);
    return;
}

void get_node_successor(struct CTX *ctx, struct Node *cur_node, struct Node *result) {
    // struct Node *tmp_node = (struct Node *) malloc(sizeof(struct Node));
    // send get_successor and receive response

    if (cur_node->id == ctx->local_id) {
        memcpy(result, ctx->local_succ, NODE_SIZE);
        return;
    }

    socklen_t addrlen = SOCKADDR_SIZE;
    uint32_t msg_len = sizeof(struct Get_Succ);
    struct Get_Succ *msg = (struct Get_Succ *) malloc(msg_len);
    msg->type = GET_SUCC_TYPE;

    if (sendto(ctx->sockfd, (char *) msg, msg_len, 0,
        (struct sockaddr *) &cur_node->addr, SOCKADDR_SIZE) < 0) {
        perror("ERROR sendto() get_node_successor");
        free(msg);
        return;
    }

    free(msg);

    char recv_buf[BUF_SIZE];
    struct sockaddr_in src;

    while (1) {
        bzero(recv_buf, BUF_SIZE);

        if (recvfrom(ctx->sockfd, recv_buf, BUF_SIZE, 0,
            (struct sockaddr *) &src, &addrlen) < 0) {
            continue;
        }

        memcpy(result, recv_buf + 4, NODE_SIZE);
    }

    return;
}

void get_closest_preceding_finger(struct CTX *ctx, struct Node *node, uint32_t id) {
    printf("get_closest_preceding_finger Node ID: %u, ID: %u\n", node->id, id);
    // get local 
    if (node->id == ctx->local_id) {
        closest_preceding_finger_handler(ctx, node, id);
        return;
    }


    struct Get_Closest_Pred *msg = (struct Get_Closest_Pred *)
        malloc(sizeof(struct Get_Closest_Pred));
    msg->type = GET_CLOSEST_PRED_TYPE;
    memcpy(&msg->type, node, NODE_SIZE);

    if (sendto(ctx->sockfd, (char *) msg, sizeof(struct Get_Closest_Pred), 0,
        (struct sockaddr *) &node->addr, SOCKADDR_SIZE) < 0) {
        perror("ERROR sendto() get_closest_preceding_finger");
        free(msg);
        return;
    }

    free(msg);

    char recv_buf[BUF_SIZE];
    while (1) {
        bzero(recv_buf, BUF_SIZE);

        if (recvfrom(ctx->sockfd, recv_buf, BUF_SIZE, 0,
            (struct sockaddr *) NULL, NULL) < 0) {
            perror("ERROR recvfrom() get_closest_preceding_finger");
        }

        uint32_t *msg_type = (uint32_t *) recv_buf;

        if (*msg_type != CLOSEST_PRED_TYPE) continue;

        memcpy(node, recv_buf + 4, NODE_SIZE);
        break;
    }

    return;
}

// return closet finger preceding id
void closest_preceding_finger_handler(struct CTX *ctx, struct Node *result, uint32_t id) {
    printf("closest_preceding_finger_handler ID: %u\n", id);
    if (id < ctx->local_id) id += power(2, MAXM);
    for (int i = MAXM; i > 0; i--) {
        uint32_t finger_id = ctx->finger[i].node.id;
        if (finger_id < ctx->local_id) finger_id += power(2, MAXM);
        if (finger_id > ctx->local_id && ctx->finger[i].node.id < id) {
            memcpy(result, &ctx->finger[i].node, NODE_SIZE);
            return;
        }
    }

    memcpy(result, ctx->local_node, NODE_SIZE);
    return;
}