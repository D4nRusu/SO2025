#include <stdio.h>
#include <string.h>

#include "treasure-manager.h"

int main(int argc, char** argv)
{
    if(argc < 2){
        printf("./tm --<command> <options>\n");
        return 0;
    }
    if(argv[1][0] != '-' || argv[1][1] != '-'){
        printf("Wrong command format\n");
        return 0;
    }

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

    // --list
    } else if(strcmp(argv[1], "--list") == 0){
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