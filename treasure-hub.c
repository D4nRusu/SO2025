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

int th() // handles ./th
{
    printf("--Treasure hub--\n");

    char command[20];
    int okMoni = 0; // 0 = monitor inactive
    pid_t pid = -2;
    int skip = 0; // this determintes whether the console is cleared or not to allow the display of data
    int haveRead = 0;

    int pipefd[2]; // 0 - read |  1 - write
    int isPipe = 0;

    if (pipe(pipefd) == -1) {
        printf("pipe error\n");
        perror("pipe");
        exit(-1);
    }

    char message[50] = "";
    char buffer[1024]; // into which I will be reading from the pipe
    ssize_t count;

    int com;
    char cwd[CWD_SIZE];
    getcwd(cwd, CWD_SIZE);
    strcat(cwd, "/temp_com.txt");
    com = open(cwd, O_CREAT | O_WRONLY, S_IRWXU | S_IRWXG | S_IRWXG); // this is the temp file used to communicate the command to the child

    if(com == -1){
        printf("Error creating command file\n");
        return 0;
    }

    while(1){
        if(pid == 0){ // setting up pipe then leaving the parent loop, child has a separate one
            isPipe = 1;
            close(pipefd[0]); // closing read end for child
            if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
                perror("dup2 failed");
                printf("dup2 fatal error\n");
                exit(-1);
            }
            close(pipefd[1]); // closing the original write end
            break;
        } else {
            if(isPipe == 1){ // only happens once - after the monitor has been started
                isPipe = 0;
                close(pipefd[1]); // closing write end for parent
            }
        }
        if(skip == 0){ // since I clear the console after almost every command, the printing is happening in the next cycle of the while loop
                       // not when the command is given
            printf("\e[1;1H\e[2J"); // interesting solution I found for "clearing" the console
            printf("Available Commands:\n");
            printf("\tstart_monitor\n\tlist_hunts\n\tlist_treasures\n\tview_treasure\n\tcalculate_score\n\tstop_monitor\n\texit\n\n");
            printf("%s\n", message);
        } else {
            skip = 0;
            if(haveRead == 1){ // only certain commands require reading from the pipe
                while((count = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0){
                    buffer[count] = '\0';
                    printf("%s", buffer);
                    if(strstr(buffer, "------")) break;
                }
                haveRead = 0;
            }
        }
        
        scanf("%s", command);

        strcpy(message, "");

        /*
        In the temporary file, each command is coded with a decimal for easier communication:
        1 - list hunts
        2 - list treasures in a hunt
        3 - view treasure
        */
        if(strcmp(command, "start_monitor") == 0){
            if(okMoni == 1){
                strcpy(message, "Can't start monitor - already active\n");
                continue;
            } else {
                if((pid = startMonitor()) == -1){
                    printf("Fatal error: unable to create the process\n");
                    return -1;
                }
                strcpy(message, "Monitor active, PID: ");
                char pid_s[7];
                sprintf(pid_s, "%d", pid);
                strcat(message, pid_s);

                okMoni = 1;
                continue;
            }
        }
        
        if(strcmp(command, "stop_monitor") == 0){
            if(okMoni == 0){
                strcpy(message, "Error: no monitor active\n");
                continue;
            }
            kill(pid, SIGTERM);
            printf("Stopping monitor...\n");
            wait(NULL);
            strcpy(message, "Monitor stopped\n");
            okMoni = 0;
            continue;
        }

        if(strcmp(command, "calculate_score") == 0){
            calc_score();
            skip = 1;
            continue;
        }

        if(strcmp(command, "list_hunts") == 0){
            if(okMoni == 0){
                strcpy(message, "Error: no monitor active\n");
                continue;
            }
            skip = 1;
            write(com, "1", 1);
            lseek(com, 0, SEEK_SET);
            kill(pid, SIGUSR1);
            sleep(1);

            haveRead = 1;
            continue;
        }

        if(strcmp(command, "list_treasures") == 0){
            if(okMoni == 0){
                strcpy(message, "Error: no monitor active\n");
                continue;
            }
            skip = 1;

            char huntId[50];
            write(com, "2", 1);
            printf("\n\nHuntID = "); scanf("%s", huntId);
            write(com, huntId, sizeof(huntId));

            lseek(com, 0, SEEK_SET);
            kill(pid, SIGUSR1);
            sleep(1);

            haveRead = 1;
            continue;
        }

        if(strcmp(command, "view_treasure") == 0){
            if(okMoni == 0){
                strcpy(message, "Error: no monitor active\n");
                continue;
            }
            skip = 1;

            write(com, "3", 1);
            char huntId[50];
            uint8_t tid = 0;
            printf("\n\nHuntID = "); scanf("%s", huntId);
            printf("TreasureID = "); scanf("%hhd", &tid);
            write(com, huntId, sizeof(huntId));
            write(com, &tid, sizeof(tid));

            lseek(com, 0, SEEK_SET);
            kill(pid, SIGUSR1);
            sleep(1);

            haveRead = 1;
            continue;
        }

        if(strcmp(command, "exit") == 0){
            if(okMoni == 1){
                strcpy(message, "Cannot exit - monitor still active\n");
                continue;
            } else {
                printf("Exited successfully\n");
                break;
            }
        }

        strcpy(message, "Bad command\n");
    }

    if(pid == 0){
        childHandler(); // this is where the child enters it's loop and waits for signals from the parent
    } else {
        close(com);
        close(pipefd[0]); // closing read end for the parent since the program is supposed to exit
        remove(cwd);      // removing the temporary file from where the child reads the command it has to execute
    }

    return 0;
}