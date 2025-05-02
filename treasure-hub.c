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
    }
}


int childHandler()
{
    struct sigaction term;
    term.sa_handler = signalHandler;
    term.sa_flags = 0;

    if (sigaction(SIGTERM, &term, NULL) == -1) {
        perror("sigaction SIGTERM");
        printf("Error SIGTERM\n");
        exit(1);
    }

    printf("Monitor awaiting command...\n");
    while(1){
        pause();
    }
    return 0;
}