#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

void operation(char a[], int* b);
bool search(char x, char* word, char* mark);
bool found_all(char* mark);

int main() {

    char* word_vector[10] = {"vector", "palindrome", "foo", "bar", "dog", "boolean"};
    int word_vector_size = 6;

    char* body_part_vector[6] = {"head", "neck", "arms", "torso", "legs", "feet"};
    int body_part_vector_size = 6;
    
    int sockfd, ret; 
    struct sockaddr_in server_address;

    int new_socket;
    struct sockaddr_in new_client_address;

    socklen_t addr_size;

    char buffer[1024];
    pid_t child_pid;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Error establishing local(server) socket\n");
        exit(1);
    }
    printf("server socket is created\n");

    memset(&server_address, '\0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = 9734;

    ret = bind(sockfd, (struct sockaddr*)&server_address, sizeof(server_address));
    if (ret < 0) {
        printf("Error binding\n");
        exit(1);
    }
    printf("bind to port %d\n", 9734);

    if (listen(sockfd, 10) == 0) {
        printf("listening in port %d\n", 9734);
    } else {
        printf("error in listening\n");
    }

    while (1) {
        srand( time(NULL) );
        new_socket = accept(sockfd, (struct sockaddr*)&new_client_address, &addr_size);
        if (new_socket < 0) {
            exit(1);
        }
        printf("Connection accepted from %s:%d\n", inet_ntoa(new_client_address.sin_addr), ntohs(new_client_address.sin_port));

        if ((child_pid = fork()) == 0) {
            // setup for game
            close(sockfd);
            
            int fail_counter = 0;

			// choose a random word
            int w = rand() % word_vector_size;
            char* word = word_vector[w];
            printf("client got the random word: %s\n", word);

            // create mark verctor
            char* mark = malloc(strlen(word));
            for (int i = 0; i < strlen(word); i++) {
                mark[i] = '*';
            }

            // first message server -> client
            snprintf(buffer, sizeof(buffer), "----------\nHANGMAN GAME\n----------\nYour word is %s (it has %i length) please input your guesses\ntype :exit if you want to quit", mark, strlen(mark));
            send(new_socket, buffer, strlen(buffer), 0);

            // game loop
            while (1) {

                // client response
                memset(&buffer, 0, sizeof(buffer));
                recv(new_socket, buffer, 1024, 0);
                printf("client responded with: %s\n", buffer);
                if(strcmp(buffer, ":exit") == 0){
					printf("Disconnected from %s:%d\n", inet_ntoa(new_client_address.sin_addr), ntohs(new_client_address.sin_port));
                    scanf("%s", &buffer[0]);
					break;
                }
                
                // check for match in word
                bool found = search(buffer[0], word, mark);
                
                // create message
                memset(&buffer, 0, sizeof(buffer));
                if (found_all(mark)) {
                    snprintf(buffer, sizeof(buffer), "GAME OVER!\nyou found the word, you win!\nword: %s\n", mark);
                    send(new_socket, buffer, strlen(buffer), 0);
                    break;
                }
                if (found) {
                    snprintf(buffer, sizeof(buffer), "you found a letter! %s\n", mark);
                } else {
                    if (fail_counter == body_part_vector_size - 1) {
                        snprintf(buffer, sizeof(buffer), "GAME OVER!\nword: %s, your %s was hanged!\n", mark, body_part_vector[fail_counter]);
                        send(new_socket, buffer, strlen(buffer), 0);
                        break;
                    }
                    snprintf(buffer, sizeof(buffer), "word: %s, your %s was hanged!\n", mark, body_part_vector[fail_counter]);
                    fail_counter++;
                }

                // send response
                send(new_socket, buffer, strlen(buffer), 0);
            }
        }
    }
    close(new_socket);
    return 0;
}

// method to iterate in word and search for letter x
// it updates mark[i] = x where word[i] == x
// input : char x, word, and mark vector (if it was already found)
// output : message to client, found with indexes found or not found
bool search(char x, char* word, char* mark) {
    int word_len = strlen(word);
    bool found = false;
    for (int i = 0; i < word_len; i++) {
        if (word[i] == x && mark[i] == '*') {
            found = true;
            mark[i] = x;
        }
    }
    return found;
}

//char* create_message(bool found, char* mark) {

  //  return 'hahah';
//}

bool found_all(char* mark) {
    for (int i = 0; i < sizeof(mark); i++) {
        if (mark[i] == '*') {
            return false;
        }
    }
    return true;
}
