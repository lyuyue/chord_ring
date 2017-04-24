#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#include "structures.h"
#include "const.h"
#include "helper.h"
#include "query.h"

void query_succ(struct CTX *ctx, struct sockaddr_in *entry_addr, struct Node *result, uint32_t id);

void set_pred(struct CTX *ctx, struct Node *dst, struct Node *pred);

void init_finger_table(struct CTX *ctx, char *entry_point);

void update_others(struct CTX *ctx);

void update_finger_table(struct CTX *ctx, struct Node *dst, struct Node *node, uint32_t idx);

void update_finger_table_handler(struct CTX *ctx, struct Node *node, uint32_t idx);

