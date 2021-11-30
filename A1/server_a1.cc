#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <bits/stdc++.h>

void operation(char a[], int* b);

int main() {
    int server_sockfd, client_sockfd;
    socklen_t server_len, client_len;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    char in[20];
    int out = 0;

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = 9734;
    server_len = sizeof(server_address);
    bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
    while(1) {
        listen(server_sockfd, 5);
        printf("server waiting\n");
        client_len = sizeof(client_address);
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);
        while (out != -1) {
            int err = read(client_sockfd, in, sizeof(in));
            if (err == -1) {
                perror("Algo de errado na leitura");
                exit(EXIT_FAILURE);
            }
            operation(in, &out);
            memset(in, 0, sizeof(in));
            write(client_sockfd, &out, sizeof(int));
        }
        close(client_sockfd);
    }
}

void operation(char a[], int* out) {
    //printf("here");
    printf(a);
    char* token = strtok(a, "#");
    int op1;
    int op2;
    char* op;
    op = token;
    token = strtok(NULL, "#");
    op1 = atoi(token);
    token = strtok(NULL, "#");
    op2 = atoi(token);  

    *out = -1;
    if (strcmp(op, "+")==0) {
        *out = op1 + op2;
    } else if (strcmp(op, "-")==0) {
        *out = op1 - op2;
    } else if (strcmp(op, "/")==0) {
        *out = op1 / op2;
    } else if (strcmp(op, "*")==0) {
        *out = op1 * op2;
    }
}