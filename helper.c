#include "helper.h"

int socket_init(int port_n) {
    struct sockaddr_in addr;
    bzero(&addr, SOCKADDR_SIZE);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_n);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("ERROR socket()");
        return -1;
    }

    if (bind(sockfd, (struct sockaddr *) &addr, SOCKADDR_SIZE) < 0) {
        perror("ERROR bind()");
        return -1;
    }

    return sockfd;
}

void get_local_ip(struct sockaddr_in *result) {
    const char* google_dns_server = "8.8.8.8";
    int dns_port = 53;
     
    struct sockaddr_in serv;
     
    int sock = socket ( AF_INET, SOCK_DGRAM, 0);
     
    //Socket could not be created
    if(sock < 0)
    {
        perror("Socket error");
    }
     
    memset( &serv, 0, sizeof(serv) );
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr( google_dns_server );
    serv.sin_port = htons( dns_port );
 
    int err = connect( sock , (const struct sockaddr*) &serv , sizeof(serv) );
     
    struct sockaddr_in name;
    socklen_t namelen = sizeof(name);
    err = getsockname(sock, (struct sockaddr*) &name, &namelen);

    memcpy(result, &name, SOCKADDR_SIZE);         

    char buffer[100];
    const char* p = inet_ntop(AF_INET, &name.sin_addr, buffer, 100);
         
    if(p != NULL)
    {
        printf("Local ip is : %s \n" , buffer);
    }

    close(sock);
}

uint32_t power(uint32_t n, uint32_t k) {
    uint32_t result = n;
    for (int i = 0; i < k; i ++) {
        result = result * n;
    }
    return result;
}

void print_ctx(struct CTX *ctx) {
    printf("Node ID: %u\n", ctx->local_id);
    printf("Predcessor ID: %u\n", ctx->pred->id);

    printf("Finger Table\n");
    for (int i = 0; i < MAXM; i++) {
        printf("start: %u, successor ID: %u\n", ctx->finger[i].start, ctx->finger[i].node.id);
    }

    return;
}