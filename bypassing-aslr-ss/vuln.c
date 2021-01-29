#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char **argv) {
    int sock, addrlen, nsock;
    struct sockaddr_in sin;
    char buf[256];

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    if (sock < 0) {
        perror("socket");
        return -1;
    }

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(31338);
    addrlen = sizeof(sin);

    if (bind(sock, (struct sockaddr *) &sin, addrlen) < 0) {
        perror("bind");
        return -1;
    }

    if (listen(sock, 5) < 0) {
        perror("listen");
        return -1;
    }

    nsock = accept(sock, (struct sockaddr *) &sin, &addrlen);
    if (nsock < 0) {
        perror("accept");
        close(sock);
        return -1;
    }

    read(nsock, buf, 1024);
    
    printf("%s",buf);

    close(nsock);
    close(sock);

    return 1;
}