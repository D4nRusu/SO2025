#include "treasure-manager.h"

void getPath(char cwd[], char* huntId)
{
    getcwd(cwd, CWD_SIZE);
    strcat(cwd, "/");
    strcat(cwd, huntId);
}

void getSize(const char *path, int *totalSize) {
    struct dirent *in_file;
    DIR *pDir = opendir(path);
    while ((in_file = readdir(pDir)) != NULL) {
        // current, parent directories ignored
        if (!strcmp (in_file->d_name, "."))
            continue;
        if (!strcmp (in_file->d_name, ".."))    
            continue;

        char fPath[PATH_MAX];
        strcpy(fPath, path);
        strcat(fPath, PATH_SEP);
        strcat(fPath, in_file->d_name);
        struct stat info;
        if(stat(fPath, &info) == -1) {
            perror("stat");
            return;
        }
        if(S_ISDIR(info.st_mode))
            getSize(fPath, totalSize);
        else if(S_ISREG(info.st_mode))
            (*totalSize) += info.st_size;
    }
    closedir(pDir);
}

void add(char* huntId)
{
    char cwd[CWD_SIZE];
    getPath(cwd, huntId);

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

    strcat(cwd, PATH_SEP);
    strcat(cwd, t.uname);
    FILE* out = fopen(cwd, "a");
    if(out == NULL){
        printf("Error adding treasure to the hunt\n");
        return;
    }

    fwrite(&t, sizeof(struct Treasure), 1, out);
    fclose(out);
}

void list(char* huntId, uint8_t tid){
    char cwd[CWD_SIZE];
    struct dirent* in_file;
    getPath(cwd, huntId);
    DIR* hunt = opendir(cwd);

    if(tid == 0){
        printf("\nHunt name: %s\n", huntId);
        int size = 0;
        getSize(cwd, &size);
        printf("Total file size: %dB\n", size);
    }

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
        strcat(fPath, PATH_SEP);
        strcat(fPath, in_file->d_name);

        struct stat file_stat;
        stat(fPath, &file_stat);
        char timebuf[64];
        struct tm *tm_info = localtime(&file_stat.st_mtime);
        strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", tm_info);

        FILE* in = fopen(fPath, "rb");
        if(in == NULL){
            printf("Error opening a treasure\n");
            closedir(hunt);
            return;
        }
        while(fread(&t, sizeof(t), 1, in) == 1){
            if(tid == 0){
                printf("\n\tTreasure id: %hhd\n", t.tid);
                printf("\tUser name: %s\n", t.uname);
                printf("\tGPS coords (lat lon): %g %g\n", t.GPS.lat, t.GPS.lon);
                printf("\tClue: %s\n", t.clue);
                printf("\tValue: %d\n", t.value);
                printf("\tLast modified: %s\n\n", timebuf);
            } else {
                if(t.tid == tid){
                    printf("\n\tTreasure id: %hhd\n", t.tid);
                    printf("\tUser name: %s\n", t.uname);
                    printf("\tGPS coords (lat lon): %g %g\n", t.GPS.lat, t.GPS.lon);
                    printf("\tClue: %s\n", t.clue);
                    printf("\tValue: %d\n\n", t.value);
                    printf("\tLast modified: %s\n\n", timebuf);
                    break;
                }
            }
        }
        fclose(in);
    }

    closedir(hunt);
}