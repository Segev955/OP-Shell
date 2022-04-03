#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <time.h>
#include "shell.h"

#define SERVER_PORT 5060


static double total = 0;

int checkSUB(char e[], char s[]) {
    if (strlen(s) < strlen(e))
        return 0;
    for (int i = 0; i < strlen(e); ++i) {
        if (s[i] != e[i])
            return 0;
    }
    return 1;
}


int main(int argc, char const *argv[]) {

    int listeningSocket = -1;
    struct sockaddr_in serverAddress, clientAddress;
    int reuse = 1;
    char buffer[1024];

    //open socket
    if ((listeningSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Could not create listening socket");
    }
    if (setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
        printf("setsockopt failed");
    }
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(SERVER_PORT);

    //bind
    printf("Binding\n");
    if (bind(listeningSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1) {
        perror("Bind failed");
        close(listeningSocket);
        return -1;
    }
    printf("Binded\n");
    //listen
    if (listen(listeningSocket, 5) < 0) {
        printf("Listen failed");
        close(listeningSocket);
        return -1;
    }
    printf("Listening\n");
    int clientSocket = -1;
    socklen_t clientAddressLen = sizeof(clientAddress);
    int c = 0;
    //accept
    memset(&clientAddress, 0, sizeof(clientAddress));
    clientAddressLen = sizeof(clientAddress);
    clientSocket = accept(listeningSocket, (struct sockaddr *) &clientAddress, &clientAddressLen);
    if (clientSocket == -1) {
        printf("Accept failed");
        close(listeningSocket);
        return -1;
    }
    printf("A new client connection accepted\n");
    char go[1024];
    bzero(go,1024);
    strcpy(go,"Good Bye");
    //receive message
    while (1) {
        bzero(buffer,1024);
        int re = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (re == 0) {
            printf("\nSocket close\n");
            close(clientSocket);
            break;
        }
        else if (re < 0) {
            printf("\nFailed receive message");
            close(clientSocket);
            break;
        } else {
//            printf("Message from client: %s", buffer);
            printf("%s", buffer);
//            if (checkSUB(go, buffer)) {
//                printf("\nSocket close");
//                close(clientSocket);
//                break;
//            }
        }
    }
    return 0;
}

