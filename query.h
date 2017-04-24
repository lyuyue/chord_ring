#include <stdio.h>
#include <string.h>

#include "helper.h"
#include "const.h"
#include "structures.h"

// ask node for id's successor
void find_successor(struct CTX *ctx, struct sockaddr_in *entry_point, struct Node *result, uint32_t id);

// ask node for id's predecessor
void find_predecessor(struct CTX *ctx, struct Node *result, uint32_t id);

void get_closest_preceding_finger(struct CTX *ctx, struct Node *node, uint32_t id);
// return closet finger preceding id
void closest_preceding_finger(struct CTX *ctx, struct Node *result, uint32_t id);

void get_node_successor(struct CTX *ctx, struct Node *cur_node, struct Node *result);
