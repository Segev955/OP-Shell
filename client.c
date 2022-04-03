#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include "shell.h"

#define SERVER_PORT 5060
#define SERVER_IP_ADDRESS "127.0.0.1"


int openSocket() {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Could not create socket : %d", errno);
    } else
        printf("Socket open\n");
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    int rval = inet_pton(AF_INET, (const char *) SERVER_IP_ADDRESS, &serverAddress.sin_addr);
    if (rval <= 0) {
        printf("inet pton failed");
        return -1;
    }
// connect
    printf("connecting...\n");
    int c = connect(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    if (c == -1) {
        perror("connect failed");
        close(sock);
        return -1;
    }
//    char msg[1024];
//    int finish = 0;
//    do {
//        bzero(msg, 1024);
//        for (int i = 0; i < 1024; i++) {
//            if (msg[i - 1] == '\n' || msg[i - 1] == '\t') {
//                msg[i] = '\0';
//                break;
//            }
//            scanf("%c", &msg[i]);
//        }
//        if (checkSUB("LOCAL ", msg) || checkSUB("LOCAL\n", msg)) {
//            finish = 1;
//            bzero(msg, 1024);
//            strcpy(msg, "Good Bye");
//            int se = send(sock, msg, strlen(msg), 0);
//        } else {
//            int se = send(sock, msg, strlen(msg), 0);
//            if (se <= 0) {
//                printf("failed with error code : %d", errno);
//            } else
//                printf("Message sent\nSend new message or type 'LOCAL' to disconnect\n");
//        }
//    } while (!finish);
//    close(sock);
//    printf("Socket closed");
//    return 1;
}
