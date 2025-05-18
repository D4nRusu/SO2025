#include <stdio.h>
#include "treasure-manager.h"

char users[100][50];
int score[100];

void calculate_score(char* huntId)
{
    char huntPath[PATH_MAX];
    getcwd(huntPath, CWD_SIZE);
    strcat(huntPath, PATH_SEP);
    strcat(huntPath, "Hunt_");
    strcat(huntPath, huntId);

    int currentUser = 1;
    struct Treasure t;
    int perms = S_IRWXU | S_IRWXG | S_IRWXG;

    struct dirent *in_file;
    DIR *hunt = opendir(huntPath);
    if(hunt == NULL){
        printf("Error opening hunt\n");
        exit(-1);
    }

    while ((in_file = readdir(hunt)) != NULL) {
        // current, parent directories ignored
        if (!strcmp (in_file->d_name, "."))
            continue;
        if (!strcmp (in_file->d_name, ".."))    
            continue;

        char fPath[CWD_SIZE];
        strcpy(fPath, huntPath);
        strcat(fPath, PATH_SEP);
        strcat(fPath, in_file->d_name);

        int in = open(fPath, O_RDONLY, perms);
        if(in == -1){
            printf("Error opening a treasure\n");
            closedir(hunt);
            return;
        }
        while(read(in, &t, sizeof(t)) == sizeof(t)){
            int i;
            int found = 0;
            for(i = 0; i < currentUser; ++i){
                if(strcmp(users[i], t.uname) == 0){
                    found = 1;
                    break;
                }
            }
            strcpy(users[i], t.uname);
            score[i] += t.value;
            if(found == 0){
                currentUser++;
            }
        }
        close(in);
    }
    printf("\n\t    Hunt_%s\n\tUser\tScore\n", huntId);
    for(int i = 1; i < currentUser; ++i){
        printf("\t%s\t%d\n", users[i], score[i]);
    }
    printf("\n");
}

int main(int argc, char** argv)
{
    if(argc != 2){
        printf("Wrong number of arguments given\n");
        return -1;
    }

    calculate_score(argv[1]);
    return 0;
}