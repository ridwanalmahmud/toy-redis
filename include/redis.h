#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

#define PORT "3490"
#define ERR 1
#define BUFFER 1024

void log_errno(char *msg) {
    perror(msg);
    exit(ERR);
}

void log_exit(char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(ERR);
}

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &((struct sockaddr_in *)sa)->sin_addr;
    }
    return &((struct sockaddr_in6 *)sa)->sin6_addr;
}
