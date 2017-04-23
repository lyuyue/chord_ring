int socket_init(int port_n) {
    struct sockaddr_in addr;
    bzero(&addr, SOCKADDR_SIZE;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_n);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("ERROR socket()");
        return -1;
    }

    // fcntl(sockfd, F_SETFL, O_NONBLOCKING);

    if (bind(sockfd, (struct sockaddr *) &addr, SOCKADDR_SIZE) < 0) {
        perror("ERROR bind()");
        return -1;
    }

    return sockfd;
}