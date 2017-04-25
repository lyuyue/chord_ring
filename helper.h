#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#include "const.h"
#include "structures.h"

#ifndef HELPER_H
#define HELPER_H

int socket_init(int port_n);

uint32_t power(uint32_t n, uint32_t k);

void get_local_ip(struct sockaddr_in *result);

void print_ctx(struct CTX *ctx);

#endif