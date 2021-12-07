#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int sockfd, result;
    struct sockaddr_in address;
    char buffer[1024];

    // socket related
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Error establishing local socket\n");
        exit(1);
    }
    printf("client socket is created\n");

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = 9734;
    
    result = connect(sockfd, (struct sockaddr *)&address, sizeof(address));
    if (result < 0) {
        perror("error connecting to server");
        exit(1);
    }
    printf("client connected\n");

    // game related
    while (1) {
        // print welcome message
        if(recv(sockfd, buffer, 1024, 0) < 0){
            printf("[-]Error in receiving data.\n");
        }else{
            printf("%s\n", buffer);

            if(strstr(buffer, "GAME OVER!")) {
                close(sockfd);
                break;
            }
        }
        // get client letter guess
		while (1) {
            memset(&buffer, 0, sizeof(buffer));
            scanf("%s", &buffer[0]);
            if(strcmp(buffer, ":exit") == 0){
                send(sockfd, buffer, strlen(buffer), 0);
                close(sockfd);
                printf("exited");
                return 0;
                break;
            }else if (strlen(buffer) == 1) {
                break;
            }
            printf("\ntoo long/short, please send another guess, size = %i\n", strlen(buffer));
        }
		send(sockfd, buffer, strlen(buffer), 0);
    }
    
    return 0;
}