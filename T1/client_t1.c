#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>

int main() {
    int sockfd;
    int len;
    struct sockaddr_in address;
    int result;
    int* out;
    char in[20];
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = 9734;
    len = sizeof(address);
    result = connect(sockfd, (struct sockaddr *)&address, len);
    if (result == -1) {
        perror("oops: client1");
        exit(1);
    }
    printf("press q to quit\n");
    printf("format: operation#operator1#operator2\n");
    while (std::strcmp(in, "q")) {
        std::cin >> in;
        printf(in);
        write(sockfd, in, strlen(in));
        read(sockfd, &out, sizeof(int));
        printf("\nresult from server = %i\n", out);
    }
    close(sockfd);
    exit(0);
}