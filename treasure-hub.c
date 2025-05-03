#include "treasure-hub.h"

pid_t startMonitor()
{
    pid_t pid = fork();
    return pid;
}

void signalHandler(int sig)
{
    if(sig == SIGTERM){
        sleep(1);
        exit(1);
    } else {
        if(sig == SIGUSR1){
            int com;
            char cwd[CWD_SIZE];
            getcwd(cwd, CWD_SIZE);
            strcat(cwd, "/temp_com.txt");
            com = open(cwd, O_CREAT | O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXG);

            char command;
            read(com, &command, 1);
            if(command == '1'){
                listHunts();
            } 
            else if(command == '2'){
                char huntId[50];
                printf("\n\nHuntID = "); scanf("%s", huntId);
                list(huntId, 0);
            }
            else if(command == '3'){
                char huntId[50];
                uint8_t tid = 0;
                printf("\n\nHuntID = "); scanf("%s", huntId);
                printf("TreasureID = "); scanf("%hhd", &tid);
                list(huntId, tid);
            }
        }
    }
}

void childHandler()
{
    struct sigaction usr1;
    usr1.sa_handler = signalHandler;
    usr1.sa_flags = 0;

    struct sigaction term;
    term.sa_handler = signalHandler;
    term.sa_flags = 0;

    if(sigaction(SIGUSR1, &usr1, NULL) == -1){
        perror("sigaction SIGUSR1");
        printf("Error SIGUSR1\n");
        exit(1);
    }

    if(sigaction(SIGTERM, &term, NULL) == -1){
        perror("sigaction SIGTERM");
        printf("Error SIGTERM\n");
        exit(1);
    }

    printf("Monitor awaiting command...\n");
    while(1){
        pause();
    }
}