#include <netinet/in.h>

struct Node {
    uint32_t id;
    struct sockaddr_in addr;
};

struct Finger_Entry {
    uint32_t start;
    uint32_t interval;
    struct Node node;
};

struct CTX {
    uint32_t local_id;
    struct Node *local_node;
    struct Node *local_pred;
    struct Node *local_succ;

    int sockfd;
    int port;

    struct sockaddr_in local_addr;·
    struct Finger_Entry finger[MAXM];
};

// Key_Query and Query_Answer
struct Key_Query {
    uint32_t type;
    uint32_t id;
    struct sockaddr_in src;
};

struct Query_Ans {
    uint32_t flag;
    uint32_t id;
    uint32_t value;
};

// Succ_Query and answer
struct Find_Succ {
    uint32_t type;
    uint32_t id;
};

struct Find_Succ_Ans {
    uint32_t type;
    struct Node succ;
};

struct Get_Succ {
    uint32_t type;
};

struct Get_Succ_Ans {
    uint32_t type;
    struct Node succ;
};

struct Get_Pred {
    uint32_t type;
};

struct Get_Pred_Ans {
    uint32_t type;
    struct Node pred;
};

struct Set_Pred {
    uint32_t type;
    struct Node pred;
};

struct Update_Finger { 
    uint32_t type;
    struct Node node;
    uint32_t idx;
};