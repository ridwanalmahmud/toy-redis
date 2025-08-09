#include "redis.h"
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(void) {
    struct addrinfo hints, *serv_info;
    struct sockaddr_storage client_info;
    char ip_str[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(NULL, PORT, &hints, &serv_info);
    if (status != 0) {
        fprintf(stderr, "gai_error: %s\n", gai_strerror(status));
        exit(ERR);
    }

    struct addrinfo *p; // iterator
    // loop through the result and bind to the first
    int fd; // socket descriptor
    for (p = serv_info; p != NULL; p = p->ai_next) {
        // get the file descriptor
        fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (fd == -1) {
            log_errno("client: socket");
        }

        // set socket options
        int yes = 1;
        int opt = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        if (opt == -1) {
            log_errno("setsockopt");
        }

        // associates the socket with a port in the local machine
        int rv = bind(fd, serv_info->ai_addr, serv_info->ai_addrlen);
        if (rv == -1) {
            log_errno("bind");
            continue;
        }

        break;
    }

    freeaddrinfo(serv_info);

    if (p == NULL) {
        log_exit("failed to bind");
    }

    // listen
    int rv = listen(fd, SOMAXCONN);
    if (rv != 0) {
        log_errno("listen");
    }

    while (1) {
        socklen_t sin_size = sizeof(client_info);
        struct sockaddr client_addr;
        int client_fd = accept(fd, &client_addr, &sin_size);
        if (client_fd == -1) {
            log_errno("accept");
        }

        // get address in string
        inet_ntop(client_info.ss_family,
                  get_in_addr(&client_addr),
                  ip_str,
                  sizeof(ip_str));

        fprintf(stdout, "Connected with %s\n", ip_str);

        if (!fork()) { // this is the child process
            close(fd); // close listening socket in child
            char rbuf[BUFFER] = {0};
            // receive a message
            int n = recv(client_fd, &rbuf, BUFFER, 0);
            if (n == -1) {
                log_errno("read");
            }
            rbuf[n] = '\0';
            printf("Client says: %s\n", rbuf);

            // close the listener
            char wbuf[BUFFER] = "Hello from server"; // write buffer
            // send a message
            n = send(client_fd, wbuf, BUFFER, 0);
            if (n == -1) {
                log_errno("send");
            }

            // close child connection
            close(client_fd);
            exit(0);
        }
        // close the connection
        close(client_fd);
    }

    return 0;
}
