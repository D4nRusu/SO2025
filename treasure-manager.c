#include "treasure-manager.h"

void addHunt(char* huntId)
{
    char cwd[1000];
    getcwd(cwd, sizeof(cwd));

    strcat(cwd, "/");
    strcat(cwd, huntId);
    if(mkdir(cwd, 0755) < 0 || errno == EEXIST){
        printf("Pathname already exists\n");
        return;
    } else{
        printf("Hunt created successfully at: %s/\n", cwd);
    }
}