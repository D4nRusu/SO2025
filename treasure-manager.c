#include "treasure-manager.h"

void getPath(char cwd[], char* huntId)
{
    getcwd(cwd, CWD_SIZE);
    strcat(cwd, "/");
    strcat(cwd, huntId);
}

void add(char* huntId)
{
    char cwd[CWD_SIZE];
    getPath(cwd, huntId);
    printf("\n\n%s\n", cwd);
    if(mkdir(cwd, 0755) < 0 || errno == EEXIST){
        printf("Hunt exists, moving on to adding treasure\n");
    } else{
        printf("Hunt created successfully at: %s/\nMoving on to adding treasure:\n", cwd);
    }
    
    struct Treasure t;
    printf("\n\tTreasure id: "); scanf("%hhd", &t.tid);
    printf("\tUser name: "); scanf("%s", t.uname);
    printf("\tGPS coords (lat lon): "); scanf("%g %g", &t.GPS.lat, &t.GPS.lon);
    printf("\tClue: "); scanf("%s", t.clue);
    printf("\tValue: "); scanf("%d", &t.value);

    strcat(cwd, "/");
    strcat(cwd, t.uname);
    FILE* out = fopen(cwd, "a");
    if(out == NULL){
        printf("Error adding treasure to the hunt\n");
        return;
    }

    fwrite(&t, sizeof(struct Treasure), 1, out);
    fclose(out);
}

void list(char* huntId){
    char cwd[CWD_SIZE];
    struct dirent* in_file;
    getPath(cwd, huntId);
    DIR* hunt = opendir(cwd);
    
    if(hunt == NULL){
        printf("Error opening hunt\n");
        return;
    }

    struct Treasure t;
    while((in_file = readdir(hunt))){
        // current, parent directories ignored
        if (!strcmp (in_file->d_name, "."))
            continue;
        if (!strcmp (in_file->d_name, ".."))    
            continue;

        char fPath[CWD_SIZE];
        strcpy(fPath, cwd);
        strcat(fPath, "/");
        strcat(fPath, in_file->d_name);

        FILE* in = fopen(fPath, "rb");
        if(in == NULL){
            printf("Error opening a treasure\n");
            closedir(hunt);
            return;
        }
        while(fread(&t, sizeof(t), 1, in) == 1){
            printf("\n\tTreasure id: %hhd\n", t.tid);
            printf("\tUser name: %s\n", t.uname);
            printf("\tGPS coords (lat lon): %g %g\n", t.GPS.lat, t.GPS.lon);
            printf("\tClue: %s\n", t.clue);
            printf("\tValue: %d\n\n", t.value);
        }
        fclose(in);
    }

    closedir(hunt);
}