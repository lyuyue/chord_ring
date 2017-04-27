#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#include "structures.h"
#include "const.h"
#include "helper.h"
#include "query.h"
#include "join.h"

void notify(struct CTX *ctx, struct Node *dst);

void stablize(struct CTX *ctx);

void notify_handler(struct CTX *ctx, struct Node *node);

void query_pred(struct CTX *ctx, struct Node *dst, struct Node *result);

void fix_fingers(struct CTX *ctx);