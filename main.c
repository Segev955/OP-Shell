#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>

#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include <unistd.h>
#include "shell.h"

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
    memset(c, 0, sizeof(c));
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

int copy(char src[], char dest[]) {
    FILE *source, *target;
    char rtow;
    char c[256];
    getcwd(c, sizeof(c));
    strcat(c, "/");
    strcat(c, src);
    source = fopen(c, "r");
    if (source == NULL) {
        printf("open 'source' error");
        return 1;
    }
    getcwd(c, sizeof(c));
    strcat(c, "/");
    strcat(c, dest);
    strcat(c, "/");
    strcat(c, src);
    target = fopen(c, "w+"); // library function
    if (target == NULL) {
        printf("write to 'target' error");
        fclose(source);
        return 1;
    }
    while ((fscanf(source, "%c", &rtow)) != EOF) // library function
        fprintf(target, "%c", rtow); // library function
    printf("The file '%s' copied to folder '%s'", src, dest);
    fclose(source);
    fclose(target);
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
        memset(s, 0, sizeof(s));
        for (int i = 0; i < 1024; i++) {
            if (s[i - 1] == '\n' || s[i - 1] == '\t') {
                s[i] = '\0';
                break;
            }
            scanf("%c", &s[i]);
        }

//        scanf("%s", &s);

        //Exit
        if (checkSUB("EXIT ", s) || checkSUB("EXIT\n", s) || checkSUB("exit\n", s) || checkSUB("exit ", s) ||
            checkSUB("exit", s) || checkSUB("EXIT", s)) {
            printf("Exit from shell\n");
            exit(1);
        } //Write after 'ECHO'
        else if (checkSUB("ECHO ", s) || checkSUB("echo ", s)) {
            printECHO(s, 5);
        }//connect to server
        else if (checkSUB("TCP PORT", s) || checkSUB("TCP PORT ", s) || checkSUB("tcp port", s) ||
                 checkSUB("tcp port ", s)) {
            if (openSocket() != -1)
                printf("Move to server\n");
            dup2(1, 555);
            dup2(sock, 1);
        }//LOCAL
        else if (checkSUB("LOCAL ", s) || checkSUB("LOCAL\n", s) || checkSUB("LOCAL", s) || checkSUB("local", s)) {
            if (sock > 0) {
                close(sock);
                dup2(555, 1);
                printf("in shell\n");
            } else
                printf("not connected");
        }//file list
        else if (checkSUB("DIR", s) || checkSUB("DIR ", s) || checkSUB("dir ", s) || checkSUB("dir", s)) {
            struct dirent *de;
            DIR *dir = opendir(".");
            if (dir == NULL)  // opendir returns NULL if couldn't open directory
            {
                printf("Could not open directory");
                closedir(dir);
                continue;
            } else {
                while ((de = readdir(dir)) != NULL) {
                    if (de->d_name[0] != '.')
                        printf("%s\n", de->d_name);
                }
                closedir(dir);
            }
        }//change direction
        else if (checkSUB("cd ", s) || checkSUB("CD ", s)) {
            changeDIR(s, 3); // chdir - system call

        }//copy file - COPY {filename} {dest}
        else if (checkSUB("COPY ", s) || checkSUB("copy ", s)) {
            char src[20], dest[20], rtow;
            int x = 0;
            int fr;
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
            copy(src, dest);
        }//Delete file
        else if (checkSUB("DELETE ", s) || checkSUB("delete ", s)) {
            char f[20];
            for (int i = 7; i < strlen(s); ++i) {
                if (s[i] == '\n' || s[i] == '\t') {
                    f[i - 7] = '\0';
                    break;
                }
                f[i - 7] = s[i];
            }
            if (unlink(f) != 0) // system call
                perror("unlink error");
            else
                printf("File '%s' deleted\n", f);
        }// fork, execlp, wait
        else {
//            system(s);
            int fo = fork();
            if (fo < 0) //error
                return 1;
            else if (fo == 0) { //child
                execl("/bin/sh", "sh", "-c", s, (char *) NULL);
            } else {
                printf("New child\n");
                wait(NULL);
                printf("Child finish\n");
            }
        }
    }

    return 0;
}



