#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "structures.h"
#include "const.h"
#include "helper.h"
#include "join.h"
#include "query.h"

struct CTX ctx;

char *entry_point = NULL;

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
            continue;
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

    get_local_ip(&ctx.local_node->addr);
    ctx.local_node->addr.sin_family = AF_INET;
    ctx.local_node->addr.sin_port = htons(ctx.port);

    // Initialization

    if ((ctx.sockfd = socket_init(ctx.port)) < 0) {
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
        init_finger_table(&ctx, entry_point);
        update_others(&ctx);
    }


    fcntl(ctx.sockfd, F_SETFL, O_NONBLOCK);

    char recv_buf[BUF_SIZE];
    while (1) {
        // Initialization
        bzero(recv_buf, BUF_SIZE);

        struct sockaddr_in src_addr;
        uint32_t addrlen = SOCKADDR_SIZE;

        if (recvfrom(ctx.sockfd, recv_buf, BUF_SIZE, 0,
                (struct sockaddr *) &src_addr, &addrlen) < 0) {
            continue;
        }

        uint32_t *msg_type = (uint32_t *) recv_buf;

        if (*msg_type == FIND_SUCC_TYPE) {

        }

        if (*msg_type == GET_SUCC_TYPE) {

        }

        if (*msg_type == GET_PRED_TYPE) {

        }

        if (*msg_type == SET_PRED_TYPE) {

        }

        if (*msg_type == UPDATE_FINGER_TYPE) {

        }

        if (*msg_type == GET_CLOSEST_PRED_TYPE) {

        }
    }
    
    return 0;
}