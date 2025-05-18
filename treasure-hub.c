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
                read(com, &huntId, sizeof(huntId));
                list(huntId, 0);
            }
            else if(command == '3'){
                char huntId[50];
                uint8_t tid = 0;
                read(com, &huntId, sizeof(huntId));
                read(com, &tid, sizeof(tid));
                list(huntId, tid);
            }
            printf("------\n"); // end marker so the parent knows when to stop reading
            fflush(stdout);
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

    // printf("Monitor awaiting command...\n");
    while(1){
        pause();
    }
}

void calc_score()
{
    DIR *dir;
    struct dirent *dent;

    dir = opendir(".");
    if(dir == NULL){
        printf("err opening directory");
        exit(-1);
    }

    int count = 0;
    char huntIds[100][20];
    while((dent = readdir(dir)) != NULL){
        char comp[5] = "Hunt";
        uint8_t found = 1;
        
        // checking if directory name starts with "Hunt"
        for(int i = 0; i < 4; ++i){
            if(dent->d_name[i] != comp[i]){
                found = 0;
                break;
            }
        }

        if(found == 1){
            char* afterBar = strchr(dent->d_name, '_'); // only interested in the hunt IDs, which are found after _
            afterBar++;
            strcpy(huntIds[count], afterBar);
            count++;
        }
    }

    if(count == 0){
        printf("No hunts found\n");
        return;
    } 

    int pipes[20][2];  
    pid_t pids[20];     

    for(int i = 0; i < count; ++i){
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(1);
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(1);
        }

        if(pid == 0){
            close(pipes[i][0]); // close read end for child
        
            if (dup2(pipes[i][1], STDOUT_FILENO) == -1) { // redirecting child's output to pipe
                perror("dup2");
                exit(1);
            }
            close(pipes[i][1]); // close the old write end

            execlp("./tc", "./tc", huntIds[i], NULL);
            perror("exec");
            exit(1);
        } else {
            close(pipes[i][1]); // close parents write end
            pids[i] = pid;
        }
    }

    for (int i = 0; i < count; i++) {
        char buffer[1024];
        ssize_t bytes;

        while ((bytes = read(pipes[i][0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes] = '\0';
            printf("%s", buffer);
            printf("PID associated with this hunt: %d\n", pids[i]);
        }

        close(pipes[i][0]);
        waitpid(pids[i], NULL, 0);
        printf("\n");
    }
}