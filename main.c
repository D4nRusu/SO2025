#include <stdio.h>
#include <string.h>

#include "treasure-manager.h"
#include "treasure-hub.h"

int tm(int argc, char** argv) // handles ./tm
{
    if(argc < 2){
        printf("./tm --<command> <options>\n");
        return 0;
    }
    if(argv[1][0] != '-' || argv[1][1] != '-'){
        printf("Wrong command format\n");
        return 0;
    }

    // --la
    if(strcmp(argv[1], "--la") == 0){
        listHunts();
        return 0;
    }

    // commands beyond here require at least a huntId
    char huntId[20];
    if(argc < 3){
        printf("No huntID specified\n");
        printf("Please enter a huntID: "); scanf("%s", huntId);
        if(strlen(huntId) < 1){
            printf("huntID must be at least 1 character\n");
            return 0;
        }
    } else {
        strcpy(huntId, argv[2]);
    }
    
    // --add
    if(strcmp(argv[1], "--add") == 0){
        uint8_t id = add(huntId);
        if(id != 0){
            char ids[256];
            sprintf(ids, "%hhd", id);
            logger(huntId, argv[1], ids);
        }

    // --l
    } else if(strcmp(argv[1], "--l") == 0){
        logger(huntId, argv[1], "");
        list(huntId, 0);

    // --rm_h (removes hunt)
    } else if(strcmp(argv[1], "--rm_h") == 0){
        int res = rm_h(huntId);
        if(res < 0){
            printf("Error removing hunt\n");
            return 0;
        } else{
            printf("Hunt removed successfully\n");
        }
    } else {
        uint8_t tid = 0;

        if(argc < 4){
            printf("No treasureID specified\n");
            printf("Please enter a treasureID:" ); scanf("%hhd", &tid);
        } else {
            tid = strtol(argv[3], NULL, 10);
        }
        if(tid == 0){
            printf("treasureID cannot be 0\n");
            return 0;
        }
        char tid_s[600];
        sprintf(tid_s, "%hhd", tid);
        logger(huntId, argv[1], tid_s);
        // --view
        if(strcmp(argv[1], "--view") == 0){
            list(huntId, tid);

        // --rm_t (removes treasure)
        } else if(strcmp(argv[1], "--rm_t") == 0){
            rm_t(huntId, tid);

            
        } else {
            printf("Bad command\n");
        }
    }
    return 0; 
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
    com = open(cwd, O_CREAT | O_WRONLY, S_IRWXU | S_IRWXG | S_IRWXG);

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

int main(int argc, char** argv)
{
    if(argv[0][3] == 'h')
        return th();
    return tm(argc, argv);
}