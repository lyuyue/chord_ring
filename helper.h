#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>

#include "const.h"

#ifndef HELPER_H
#define HELPER_H

int socket_init(int port_n);

uint32_t power(uint32_t n, uint32_t k);

#endif