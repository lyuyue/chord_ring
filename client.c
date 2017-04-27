#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "structures.h"
#include "const.h"

uint32_t key = -1;
struct sockaddr_in addr;
int sockfd = -1;

int main(int argc, char *argv[]) {
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8801);

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--port") == 0) {
            i++;
            addr.sin_port = htons(atoi(argv[i]));
            continue;
        }

        if (strcmp(argv[i], "--addr") == 0) {
            i++;
            uint32_t tmp = inet_addr(argv[i]);
            memcpy(&addr.sin_addr, &tmp, 4);
            continue;
        }

        if (strcmp(argv[i], "--key") == 0) {
            i++;
            key = atoi(argv[i]);
            continue;
        }
    }
    // init socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        perror("ERROR socket()");
        return -1;
    }

    // parse arguments
    if (strcmp(argv[1], "leave") == 0) {
        printf("LEAVE_TYPE\n");
        uint32_t msg_len = sizeof(struct Leave);
        struct Leave *msg = (struct Leave *) malloc(msg_len);
        msg->type = LEAVE_TYPE;

        if (sendto(sockfd, (char *) msg, msg_len, 0,
                (struct sockaddr *) &addr, SOCKADDR_SIZE) < 0) {
            perror("ERROR sendto() leave");
        }
    }

    // if (strcmp(argv[0], "query") == 0) {
    //     uint32_t msg_len = sizeof(struct Key_Query);
    //     struct Key_Query *msg = (struct Key_Query *) malloc(msg_len);
    //     msg->type = KEY_QUERY_TYPE;
    //     msg->key = key;

    //     if (sendto(sockfd, (char *) msg, msg_len, 0,
    //             (struct sockaddr *) &addr, SOCKADDR_SIZE) < 0) {
    //         perror("ERROR sendto() query");
    //         return -1;
    //     }

    //     char recv_buf[BUF_SIZE];

    //     while (1) {
    //         bzero(recv_buf, BUF_SIZE);
    //         if (recvfrom(sockfd, recv_buf, BUF_SIZE, 0,
    //                 (struct sockaddr *) NULL, NULL)  < 0) {
    //             perror("ERROR recvfrom() query");
    //         }

    //         uint32_t *msg_type = (uint32_t *) recv_buf;
    //         if (*msg_type != KEY_QUERY_ANS_TYPE) {
    //             continue;
    //         }

    //         struct Node result;
    //         memcpy(&result, recv_buf + 4, NODE_SIZE);

    //         printf("Node ID: %u, IP addr: %s\n", result.id, inet_ntoa(result.addr.sin_addr));
    //         break;
    //     } 
    // }

    return 0;
}