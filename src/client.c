#include "redis.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    struct addrinfo hints, *serv_info;
    char ip_str[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(NULL, PORT, &hints, &serv_info);
    if (status != 0) {
        fprintf(stderr, "%s\n", gai_strerror(status));
        exit(ERR);
    }

    struct addrinfo *p;
    int fd;
    for (p = serv_info; p != NULL; p = p->ai_next) {
        fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (fd == -1) {
            log_errno("client: socket");
        }

        int rv = connect(fd, p->ai_addr, p->ai_addrlen);
        if (rv == -1) {
            log_errno("connect");
            continue;
        }

        break;
    }

    // get address in string
    inet_ntop(p->ai_family, get_in_addr(p->ai_addr), ip_str, sizeof(ip_str));

    fprintf(stdout, "Connected with %s\n", ip_str);

    char wbuf[BUFFER] = "Hello from client"; // write buffer
    // send a message
    int n = send(fd, wbuf, BUFFER, 0);
    if (n == -1) {
        log_errno("send");
    }
    char rbuf[BUFFER];
    // receive a message
    n = recv(fd, &rbuf, BUFFER, 0);
    if (n == -1) {
        log_errno("recv");
    }
    rbuf[n] = '\0';
    printf("Server says: %s\n", rbuf);

    // close the connection
    close(fd);
}
