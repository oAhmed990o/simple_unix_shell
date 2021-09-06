#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

void handles() {
    int x, y;
    y = waitpid(-1, &x, WNOHANG);
    if (y > 0) {
        FILE* f = fopen("history.log", "a+");

        if(f == NULL)
            return;

        fprintf(f, "Process %d: terminated successfully in the background.\n", getpid());
    }
}

int main() {
    FILE* f = fopen("history.log", "w");
    fclose(f);

    struct sigaction sig;
    sig.sa_handler = handles;
    sig.sa_flags = SA_RESTART;

    char** commandArray;

    char userInput[100];
    char* temp, * delimiter = " \n";

    char token[50];

    int commandSize;
    int i, flag, frk, proc;

    while(1) {

        flag = 1;

        printf("> $ ");
        fgets(userInput, 100, stdin);

        if(!strcmp(userInput, "\n"))
            flag = 0;
        else
            strcpy(userInput, strtok(userInput, "\n"));

        if(flag) {
            strcpy(token, userInput);
            temp = strtok(token, delimiter);

            i = 0;
            commandSize = 0;

            while (temp != NULL) {
                commandSize += 1;
                temp = strtok(NULL, delimiter);
            }

            commandArray = malloc(sizeof(char *) * commandSize);
            strcpy(token, userInput);
            temp = strtok(token, delimiter);

            while (temp != NULL) {
                commandArray[i] = (char *) malloc(sizeof(char) * strlen(temp));
                strcpy(commandArray[i], temp);
                temp = strtok(NULL, delimiter);
                i++;
            }
            
            i = 0;
            if (!strcmp(commandArray[i], "exit")) {
                printf("Proceeding EXIT ...\n");
                exit(0);
            } else if (commandArray[0] == NULL) {
                continue;
            }

            if(!strcmp(commandArray[commandSize - 1], "&")) {
                flag = 0;
                commandArray[commandSize - 1] = NULL;
            }

            frk = fork();
            if (frk >= 0) {
                if (!frk) {
                    proc = execvp(commandArray[0], commandArray);
                    if (proc == -1)
                        printf("Error: %s: command not found\n", userInput);
                    exit(0);
                } else if(flag) {
                    wait(NULL);
                } else
                    sigaction(SIGCHLD, &sig, NULL);
            } else
                printf("Forking Error");
            usleep(30000);
        }
    }
}

