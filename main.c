#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
//#include <winsock2.h>
//#include <ws2tcpip.h>

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT 5060
#define SERVER_IP_ADDRESS "127.0.0.1"

int checkSUB(char e[], char s[]) {
    if (strlen(s) < strlen(e))
        return 0;
    for (int i = 0; i < strlen(e); ++i) {
        if (s[i] != e[i])
            return 0;
    }
    return 1;
}

int printECHO(char s[], int n) {
    char c[1024];
    if (n >= strlen(s))
        return 0;
    for (int i = n; i < strlen(s); ++i) {
        c[i - n] = s[i];
    }
    printf("%s", c);
    return 1;
}

int changeDIR(char s[], int n) {
    char c[1024];
    int x = 0;
    if (n >= strlen(s))
        return 0;
    for (int i = n; i < strlen(s); ++i) {
        c[i - n] = s[i];
        x = i;
    }
    c[x - n] = '\0';
    if (chdir(c) != 0) {
        printf("change direction to '%s' failed", c);
    }
    return 1;
}

int openSocket() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
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
    char msg[1024];
    int finish = 0;
    do {
        bzero(msg, 1024);
        for (int i = 0; i < 1024; i++) {
            if (msg[i - 1] == '\n' || msg[i - 1] == '\t') {
                msg[i] = '\0';
                break;
            }
            scanf("%c", &msg[i]);
        }
        if (checkSUB("LOCAL ", msg) || checkSUB("LOCAL\n", msg)) {
            finish = 1;
            bzero(msg, 1024);
            strcpy(msg, "Good Bye");
            int se = send(sock, msg, strlen(msg), 0);
        } else {
            int se = send(sock, msg, strlen(msg), 0);
            if (se <= 0) {
                printf("failed with error code : %d", errno);
            } else
                printf("Message sent\nSend new message or type 'LOCAL' to disconnect\n");
        }
    } while (!finish);
    close(sock);
    printf("Socket closed");
    return 1;
}

int main() {
    char c[256];
    char s[1024];
    while (1) {
//        printf("yes master?");
        if (getcwd(c, sizeof(c)) == NULL)
            perror("getcwd error");
        else
            printf("\n%s>", c);
        for (int i = 0; i < 1024; i++) {
            if (s[i - 1] == '\n' || s[i - 1] == '\t') {
                s[i] = '\0';
                break;
            }
            scanf("%c", &s[i]);
        }

//        scanf("%s", &s);

        //Exit
        if (checkSUB("EXIT ", s) || checkSUB("EXIT\n", s) || checkSUB("exit\n", s) || checkSUB("exit ", s)) {
            exit(1);
            //Write after 'ECHO'
        } else if (checkSUB("ECHO ", s) || checkSUB("echo ", s)) {
            printECHO(s, 5);
            //connect to server
        } else if (checkSUB("TCP PORT", s) || checkSUB("TCP PORT ", s) || checkSUB("tcp port", s) ||
                   checkSUB("tcp port ", s)) {
            openSocket();
            //file list
        } else if (checkSUB("DIR", s) || checkSUB("DIR ", s)) {
            struct dirent *de;
            DIR *dir = opendir(".");
            if (dir == NULL)  // opendir returns NULL if couldn't open directory
            {
                printf("Could not open directory");
                closedir(dir);
                continue;
            } else {
                while ((de = readdir(dir)) != NULL)
                    printf("%s\n", de->d_name);

                closedir(dir);
            }
            //change direction
        } else if (checkSUB("cd ", s) || checkSUB("CD ", s)) {
            changeDIR(s, 3); // chdir - system call
            //copy file
        } else if (checkSUB("COPY ", s) || checkSUB("copy ", s)) {
            char src[20], dest[20];
            int x = 0;
            for (int i = 5; i < strlen(s); ++i) {
                if (s[i] == ' ') {
                    src[i - 5] = '\0';
                    x = i + 1;
                    break;
                }
                src[i - 5] = s[i];
            }
            for (int i = x; i < strlen(s); ++i) {
                if (s[i] == ' ' || s[i] == '\n' || s[i] == '\t') {
                    dest[i - x] = '\0';
                    break;
                }
                dest[i - x] = s[i];
            }
            FILE *source, *target;
            int ch;
            strcat(c, "\\");
            strcat(c, src);
            int size = 0;
            source = fopen(c, "r");
            fread(&size, sizeof(size), 1, source);
            getcwd(c, sizeof(c));
            strcat(c, "\\");
            strcat(c, dest);
            strcat(c, "\\");
            strcat(c, src);
            target = fopen(c, "w"); // library function
            while ((ch = fgetc(source)) != EOF) // library function
                fputc(ch, target); // library function
            fclose(source);
            fclose(target);
        }
            //Delete file
        else if (checkSUB("DELETE ", s) || checkSUB("delete ", s)) {
            char f[20];
            for (int i = 7; i < strlen(s); ++i) {
                if (s[i] == ' ' || s[i] == '\n' || s[i] == '\t') {
                    f[i - 7] = '\0';
                    break;
                }
                f[i - 7] = s[i];
            }
            printf("filename: '%s'\n", f);
            if (unlink(f) != 0) // system call
                perror("unlink error");
        } else {
//            system(s);
            int fo = fork();
            if (fo < 0) //error
                return 1;
            else if (fo == 0) { //child
                char bin[256] = "/bin/";
                strcat(bin, c);
                execlp(bin, c, NULL);
            } else {
                printf("Waiting for child\n");
                wait(NULL);
                printf("Child finish\n");
            }
        }
    }

    return 0;
}



