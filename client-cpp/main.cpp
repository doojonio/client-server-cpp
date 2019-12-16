#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "../sendall-h/sendall.h"

int main() {

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(2000);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(sock, (sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        exit(2);
    }
    while(1) {
        int numbers[2] = {1, 2};
        int ans;

        printf("Enter to numbers separated by space: ");
        scanf("%d %d", &numbers[0], &numbers[1]);

        void * answer = (void *)&ans;
        
        sendall(sock, numbers, sizeof(numbers), 0);
        recv(sock, answer, sizeof(ans), 0);

        ans = *static_cast<int*>(answer);
        printf("sum: %d \n", ans);
    }
    close(sock);
    
    return 0;
}
