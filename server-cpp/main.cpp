#include <sys/types.h>
#include <sys/socket.h>

#include <stdio.h>

#include <sys/select.h>
#include <netinet/in.h>

#include <fcntl.h>
#include <unistd.h>

#include <algorithm>
#include <set>

using namespace std;

// echo server realization

int main() {
    int listener;
    struct sockaddr_in addr;

    // configure a non-blocking socket
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        perror("socket");
        exit(1);
    }

    fcntl(listener, F_SETFL, O_NONBLOCK);

    // socket naming (binding to an address)
    addr.sin_family = AF_INET;
    addr.sin_port = htons(2000);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listener, (sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(2);
    }

    // activate listening
    listen(listener, 5);

    printf("Socket listener descr: %d \n", listener);

    // current connetction-clients
    set<int> clients;
    clients.clear();

    // main life-loop for server
    while(1) {
        // set for readable sockets
        fd_set readset;
        FD_ZERO(&readset);
        FD_SET(listener, &readset);

        // updating readset for new clients
        for (set<int>::iterator it = clients.begin(); it != clients.end(); it++)
            FD_SET(*it, &readset);

        // configure timeout-param
        timeval timeout;
        timeout.tv_sec = 500;
        timeout.tv_usec = 0;

        // check and get sockets for reading from readset
        int mx = max(listener, *max_element(clients.begin(), clients.end()));
        if (select(mx + 1, &readset, NULL, NULL, &timeout) <= 0) {
            perror("select");
            exit(3);
        }

        // if we have a new connection query -- we accept it
        if (FD_ISSET(listener, &readset)) {
            int new_sock = accept(listener, NULL, NULL);
            if (new_sock < 0) {
                perror("accept");
                exit(3);
            }

            fcntl(new_sock, F_SETFL, O_NONBLOCK);
            printf("Get new connection, socket: %d \n", new_sock);

            clients.insert(new_sock);
        }

        //  getting messages from sockets
        for (set<int>::iterator it = clients.begin(); it != clients.end(); it++) {
            if (FD_ISSET(*it, &readset)) {
                int numbers[2] = {0, 0};
                int bytes_read = recv(*it, numbers, sizeof(numbers), 0);

                if (bytes_read <= 0) {
                    close(*it);
                    clients.erase(*it);
                    continue;
                }

                printf("Get numbers: %d and %d from client: %d \n", 
                    numbers[0], numbers[1], *it
                );
                int sum = numbers[0] + numbers[1];
                printf("Calculated sum: %d \nSending it to the client...\n", sum);
                send(*it, (void *)&sum, sizeof(sum), 0);
            }
        }
    }

    return 0;
}
