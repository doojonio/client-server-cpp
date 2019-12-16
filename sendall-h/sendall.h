#pragma once

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

int sendall(int sockfd, void *message, int size, int flags) {
    int sended = 0;
    int n = -1;

    while (sended < size) {
        n = send(sockfd, message, size, flags);
        if (n < 0) {
            perror("send");
            break;
        }
        sended += n;
    }

    return n < 0 ? -1 : sended;
}
