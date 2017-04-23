#include <stdio.h>
#include <stdlib.h>

#include "structures.h"
#include "const.h"
#include "udp.h"

struct CTX ctx;

char *entry_point = NULL;

uint32_t power(uint32_t n, uint32_t k) {
    uint32_t result = n;
    for (int i = 0; i < k; i ++) {
        result = result * n;
    }
    return result;
}

int main(int argc, char *argv[]) {
    // Parse Argument
    for (int arg_itr = 1; arg_itr < argc; arg_itr ++) {
        if (strcmp(argv[arg_itr], "--node-id") == 0) {
            arg_itr ++;
            ctx.local_id = atoi(argv[arg_itr]);
            continue;
        }

        if (strcmp(argv[arg_itr], "--port") == 0) {
            arg_itr ++;
            ctx.port = atoi(argv[arg_itr]);
            continue
        }

        if (strcmp(argv[arg_itr], "--entry-point") == 0) {
            arg_itr ++;
            entry_point = argv[arg_itr];
            continue;
        }
    }

    // Context setup
    bzero(&ctx, sizeof(struct CTX));
    ctx.local_node = (struct Node *) malloc(NODE_SIZE);
    ctx.local_pred = (struct Node *) malloc(NODE_SIZE);

    for (int i = 0; i < MAXM; i ++) {
        ctx.finger[i].start = ctx.local_id + power(2, i);
    }

    ctx.local_succ = (struct Node *) &ctx.finger[0].node;

    // Initialization
    bzero(&local_addr, SOCKADDR_SIZE);

    if ((sockfd = socket_init(port)) < 0) {
        perror("ERROR socket_init()");
        exit(-1);
    }

    // Node Join
    if (entry_point == NULL) {
        // only node in the network
        for (int i = 0; i < MAXM; i++) {
            memcpy(&ctx.finger[i].node, ctx.local_node, NODE_SIZE);
        }
        memcpy(ctx.local_pred, ctx.local_node, NODE_SIZE);
    } else {
        // an arbitrary entry point of network is given
        init_finger_table(ctx);
        update_others();


    }


    //
    char recv_buf[BUF_SIZE];
    while (1) {
        // Initialization
        bzero(recv_buf, BUF_SIZE);

        struct sockaddr_in src_addr;
        uint32_t addrlen = SOCKADDR_SIZE;

        if (recvfrom(sockfd, recv_buf, BUF_SIZE,
                (struct sockaddr *) &src_addr, &addrlen) < 0) {
            continue;
        }

        uint32_t *msg_type = (uint32_t *) recv_buf;

        if (*msg_type == KEY_QUERY_TYPE) {
            struct Query_Message *query = (struct Query_Message *) malloc(QUERY_SIZE);
            memcpy(query, recv_buf, QUERY_SIZE);

            if (query->src.sin_addr.s_addr == 0) {
                memcpy(query->src, &src_addr, SOCKADDR_SIZE);
            }


            continue;
        }

        if (*msg_type == FIND_SUCC_TYPE) {
            struct Node *tmp_node = (struct Node *) malloc(NODE_SIZE);
        }

        if (*msg_type == SUCC_ANS_TYPE) {

        }

        if (*msg_type == PRED_QUERY_TYPE) {

        }

        if (*msg_type == PRED_ANS_TYPE) {

        }
    }
    
    return 0;
}