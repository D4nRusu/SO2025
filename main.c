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

int th()
{
    printf("--Treasure hub--\n");

    char command[15];
    int okMoni = 0;
    pid_t pid = -2;

    char message[50];

    while(1){
        if(pid == 0){
            break;
        }
        printf("\e[1;1H\e[2J"); // interesting solution I found for clearing the console
        printf("Available Commands:\n");
        printf("\tstart_monitor\n\tlist_hunts\n\tlist_treasures\n\tview_treasure\n\tstop_monitor\n\texit\n\n");

        printf("%s\n", message);
        scanf("%s", command);

        strcpy(message, "");

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
            kill(pid, SIGTERM);
            printf("Stopping monitor...\n");
            wait(NULL);
            strcpy(message, "Monitor stopped\n");
            okMoni = 0;
            continue;
        }

        if(strcmp(command, "list_hunts") == 0){
            kill(pid, SIGUSR1);
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
        int status = childHandler();
    }

    return 0;
}

int main(int argc, char** argv)
{
    return th();
}