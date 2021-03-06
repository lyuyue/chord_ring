#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "structures.h"
#include "const.h"
#include "helper.h"
#include "join.h"
#include "query.h"
#include "stablize.h"

struct CTX ctx;

char *entry_point = NULL;

time_t thres = 10;
time_t prev_time = 0;
time_t cur_time = 0;

int terminate_flag = 0;

int main(int argc, char *argv[]) {
    // Initialization
    bzero(&ctx, sizeof(struct CTX));

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
    ctx.local_node = (struct Node *) malloc(NODE_SIZE);
    ctx.local_pred = NULL;

    for (int i = 0; i < MAXM; i ++) {
        ctx.finger[i].start = (ctx.local_id + power(2, i)) % power(2, MAXM);
    }

    ctx.local_succ = (struct Node *) &ctx.finger[0].node;

    get_local_ip(&ctx.local_addr);

    memcpy(&ctx.local_node->addr, &ctx.local_addr, SOCKADDR_SIZE);

    ctx.local_node->addr.sin_family = AF_INET;
    ctx.local_node->addr.sin_port = htons(ctx.port);

    ctx.local_node->id = ctx.local_id;
    // Initialization

    if ((ctx.sockfd = socket_init(ctx.port)) < 0) {
        perror("ERROR socket_init()");
        exit(-1);
    }

    print_ctx(&ctx);

    // Node Join
    if (entry_point == NULL) {
        // only node in the network
        for (int i = 0; i < MAXM; i++) {
            memcpy(&ctx.finger[i].node, ctx.local_node, NODE_SIZE);
        }
        ctx.local_pred = (struct Node *) malloc(NODE_SIZE);
        memcpy(ctx.local_pred, ctx.local_node, NODE_SIZE);
    } else {
        // an arbitrary entry point of network is given
        init_finger_table(&ctx, entry_point);
        // update_others(&ctx);
    }

    time(&prev_time);

    print_ctx(&ctx);

    fcntl(ctx.sockfd, F_SETFL, O_NONBLOCK);

    char recv_buf[BUF_SIZE];
    while (1) {
        // Initialization
        bzero(recv_buf, BUF_SIZE);

        struct sockaddr_in src_addr;
        uint32_t addrlen = SOCKADDR_SIZE;

        recvfrom(ctx.sockfd, recv_buf, BUF_SIZE, 0,
            (struct sockaddr *) &src_addr, &addrlen);

        uint32_t *msg_type = (uint32_t *) recv_buf;

        if (*msg_type == FIND_SUCC_TYPE) {
            uint32_t id = 0;
            memcpy(&id, recv_buf + 4, 4);

            printf("FIND_SUCC_TYPE from %s for ID %u\n", inet_ntoa(src_addr.sin_addr), id);

            struct Find_Succ_Ans *msg = (struct Find_Succ_Ans *)
                malloc(sizeof(struct Find_Succ_Ans));
            msg->type = FIND_SUCC_ANS_TYPE;

            find_successor(&ctx, &msg->succ, id);

            if (sendto(ctx.sockfd, (char *) msg, sizeof(struct Find_Succ_Ans), 0,
                    (struct sockaddr *) &src_addr, SOCKADDR_SIZE) < 0) {
                perror("ERROR sendto() find_successor");
            }

            free(msg);
        }

        if (*msg_type == GET_SUCC_TYPE) {
            printf("GET_SUCC_TYPE from %s\n", inet_ntoa(src_addr.sin_addr));
            struct Get_Succ_Ans *msg = (struct Get_Succ_Ans *) 
                malloc(sizeof(struct Get_Succ_Ans));
            msg->type = GET_SUCC_ANS_TYPE;
            memcpy(&msg->succ, ctx.local_succ, NODE_SIZE);

            if (sendto(ctx.sockfd, (char *) msg, sizeof(struct Get_Succ_Ans), 0,
                    (struct sockaddr *) &src_addr, SOCKADDR_SIZE) < 0) {
                perror("ERROR sendto() get_succ_ans");
                free(msg);
                continue;
            }

            free(msg);
        }

        if (*msg_type == GET_PRED_TYPE) {
            printf("GET_PRED_TYPE from %s\n", inet_ntoa(src_addr.sin_addr));
            struct Get_Pred_Ans *msg = (struct Get_Pred_Ans *) 
                malloc(sizeof(struct Get_Pred_Ans));
            msg->type = GET_PRED_ANS_TYPE;
            memcpy(&msg->pred, ctx.local_pred, NODE_SIZE);
            if (sendto(ctx.sockfd, (char *) msg, sizeof(struct Get_Pred_Ans), 0,
                    (struct sockaddr *) &src_addr, SOCKADDR_SIZE) < 0) {
                perror("ERROR sendto() get_succ_ans");
                free(msg);
                continue;
            }

            free(msg);
        }

        if (*msg_type == SET_PRED_TYPE) {
            printf("SET_PRED_TYPE from %s\n", inet_ntoa(src_addr.sin_addr));
            memcpy(ctx.local_pred, recv_buf + 4, NODE_SIZE);

            print_ctx(&ctx);
        }

        if (*msg_type == UPDATE_FINGER_TYPE) {
            struct Node *tmp_node = (struct Node *) malloc(NODE_SIZE);
            memcpy(tmp_node, recv_buf + 4, NODE_SIZE);
            uint32_t idx;
            memcpy(&idx, recv_buf + 4 + NODE_SIZE, 4);


            printf("UPDATE_FINGER_TYPE from %s idx: %u, finger_node: %u\n", 
                inet_ntoa(src_addr.sin_addr), idx, tmp_node->id);

            // TODO: multithread
            update_finger_table_handler(&ctx, tmp_node, idx);

            print_ctx(&ctx);
        }

        if (*msg_type == GET_CLOSEST_PRED_TYPE) {
            struct Get_Closest_Pred *tmp = (struct Get_Closest_Pred *) recv_buf;

            printf("GET_CLOSEST_PRED_TYPE from %s for %u\n", inet_ntoa(src_addr.sin_addr), tmp->id);
            
            uint32_t msg_len = sizeof(struct Closest_Pred);
            struct Closest_Pred *msg = (struct Closest_Pred *) 
                malloc(msg_len);
            msg->type = CLOSEST_PRED_TYPE;

            closest_preceding_finger_handler(&ctx, &msg->pred, tmp->id);

            if (sendto(ctx.sockfd, (char *) msg, msg_len, 0,
                    (struct sockaddr *) &src_addr, SOCKADDR_SIZE) < 0) {
                perror("ERROR sendto() closest_preceding_finger_handler");
                free(msg);
                continue;
            }

            free(msg);
        }

        if (*msg_type == NOTIFY_TYPE) {
            struct Notify *msg = (struct Notify *) recv_buf;
            printf("NOTIFY_TYPE from %u %s\n", msg->node.id, inet_ntoa(msg->node.addr.sin_addr));
            notify_handler(&ctx, &msg->node);
            continue;
        }

        if (*msg_type == LEAVE_TYPE) {
            printf("LEAVE_TYPE\n");
            uint32_t msg_len = sizeof(struct Set_Pred);
            struct Set_Pred *set_pred = (struct Set_Pred *) malloc(msg_len);
            set_pred->type = SET_PRED_TYPE;
            memcpy(&set_pred->pred, ctx.local_pred, NODE_SIZE);

            if (sendto(ctx.sockfd, (char *) set_pred, msg_len, 0,
                    (struct sockaddr *) &ctx.local_succ->addr, SOCKADDR_SIZE) < 0) {
                perror("ERROR sendto() closest_preceding_finger_handler");
                free(set_pred);
                continue;
            }

            free(set_pred);

            struct Set_Succ *set_succ = (struct Set_Succ *) malloc(msg_len);
            set_succ->type = SET_SUCC_TYPE;
            memcpy(&set_succ->succ, ctx.local_succ, NODE_SIZE);

            if (sendto(ctx.sockfd, (char *) set_succ, msg_len, 0,
                    (struct sockaddr *) &ctx.local_pred->addr, SOCKADDR_SIZE) < 0) {
                perror("ERROR sendto() closest_preceding_finger_handler");
                free(set_succ);
                continue;
            }

            free(set_succ);

            terminate_flag = 1;
        }

        if (*msg_type == SET_SUCC_TYPE) {
            printf("SET_SUCC_TYPE from %s\n", inet_ntoa(src_addr.sin_addr));
            memcpy(ctx.local_succ, recv_buf + 4, NODE_SIZE);

            print_ctx(&ctx);
        }

        time(&cur_time);

        if (cur_time - prev_time > thres && terminate_flag == 0) {
            stablize(&ctx);
            fix_fingers(&ctx);
            time(&prev_time);
        }
    }
    
    return 0;
}