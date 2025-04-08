#include "treasure-manager.h"

int perms = S_IRWXU | S_IRWXG | S_IRWXG;

void getPath(char cwd[], char* huntId)
{
    getcwd(cwd, CWD_SIZE);
    strcat(cwd, PATH_SEP);
    strcat(cwd, huntId);
}

void getSize(const char* path, int* totalSize) {
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
        else if(S_ISREG(info.st_mode)){ // calculate size and display last modified time
            char timebuf[64];
            struct tm *tm_info = localtime(&info.st_mtime);
            strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", tm_info);
            printf("%s %s\n", in_file->d_name, timebuf);
            (*totalSize) += info.st_size;
        }
    }
    closedir(pDir);
}

void logger(char* huntId, char* op, char* param)
{
    char cwd[CWD_SIZE];
    getPath(cwd, huntId);
    strcat(cwd, PATH_SEP);
    strcat(cwd, "logged_hunt");

    int out = open(cwd, CR_AP, perms);
    if(out == -1){
        printf("Error logging operation\n");
        return;
    }

    char buf[50];
    snprintf(buf, sizeof(buf), "%s %s %s\n", op, huntId, param);

    if(write(out, &buf, strlen(buf)) == -1){
        printf("Error printing to log\n");
    }
    close(out);
}

uint8_t add(char* huntId)
{
    char cwd[CWD_SIZE];
    getPath(cwd, huntId);
    int firstTime = 0;

    if(mkdir(cwd, 0755) < 0 || errno == EEXIST){
        printf("Hunt exists, moving on to adding treasure\n");
    } else{
        printf("Hunt created successfully at: %s/\nMoving on to adding treasure:\n", cwd);
        firstTime = 1;
    }
    
    struct Treasure t;
    printf("\n\tTreasure id: "); scanf("%hhd", &t.tid);
    if(t.tid == 0){
        printf("Error! TreasureID can't be 0!!\n");
        return 0;
    }

    printf("\tUser name: "); scanf("%s", t.uname);
    printf("\tGPS coords (lat lon): "); scanf("%g %g", &t.GPS.lat, &t.GPS.lon);
    printf("\tClue: "); scanf("%s", t.clue);
    printf("\tValue: "); scanf("%d", &t.value);

    strcat(cwd, PATH_SEP);
    strcat(cwd, t.uname);

    if(firstTime == 1){
        char source[CWD_SIZE - 100], target[CWD_SIZE];
        getcwd(source, CWD_SIZE);

        /*strcpy(target, source);
        strcat(target, PATH_SEP);
        strcat(target, huntId);
        strcat(target, "/logged_hunt");*/
        if(strlen(source) == CWD_SIZE - 100){
            printf("File path too long\n");
            return 0;
        }
        snprintf(target, sizeof(target), "%s%s%s%s", source, PATH_SEP, huntId, "/logged_hunt");

        strcat(source, "/logged_hunt_");
        strcat(source, huntId);
        if (symlink(target, source) != 0) {
            perror("symlink");
        } 
    }

    int out = open(cwd, CR_AP, perms);
    if(out == -1){
        printf("Error adding treasure to the hunt\n");
        return 0;
    }

    write(out, &t, sizeof(struct Treasure));
    close(out);
    return t.tid;
}

void list(char* huntId, uint8_t tid){
    char cwd[CWD_SIZE];
    struct dirent* in_file;
    getPath(cwd, huntId);
    DIR* hunt = opendir(cwd);

    if(hunt == NULL){
        printf("Error opening hunt\n");
        return;
    }

    if(tid == 0){
        printf("\nHunt name: %s\n", huntId);
        int size = 0;
        getSize(cwd, &size);
        printf("Total file size: %dB\n", size);
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

        int in = open(fPath, O_RDONLY, perms);
        if(in == -1){
            printf("Error opening a treasure\n");
            closedir(hunt);
            return;
        }
        while(read(in, &t, sizeof(t)) == sizeof(t)){
            if(tid == 0){
                printf("\n\tTreasure id: %hhd\n", t.tid);
                printf("\tUser name: %s\n", t.uname);
                printf("\tGPS coords (lat lon): %g %g\n", t.GPS.lat, t.GPS.lon);
                printf("\tClue: %s\n", t.clue);
                printf("\tValue: %d\n\n", t.value);
            } else {
                if(t.tid == tid){
                    printf("\n\tTreasure id: %hhd\n", t.tid);
                    printf("\tUser name: %s\n", t.uname);
                    printf("\tGPS coords (lat lon): %g %g\n", t.GPS.lat, t.GPS.lon);
                    printf("\tClue: %s\n", t.clue);
                    printf("\tValue: %d\n\n", t.value);
                    break;
                }
            }
        }
        close(in);
    }

    closedir(hunt);
}

void rm_t(char* huntId, uint8_t tid)
{
    char cwd[CWD_SIZE];
    getPath(cwd, huntId);
    struct dirent* in_file;
    DIR* hunt = opendir(cwd);

    if(hunt == NULL){
        printf("Error opening hunt\n");
        return;
    }
    struct Treasure t;
    int found = 0;
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

        int in = open(fPath, O_RDWR, perms);
        if(in == -1){
            printf("Error opening a treasure\n");
            closedir(hunt);
            return;
        }
        int offset = 0;
        while(read(in, &t, sizeof(t)) == sizeof(t)){
            if(t.tid != tid){
                offset++;
                continue;
            }
            found = 1;
            break;
        }

        if(found == 1){
            long pos = offset * sizeof(struct Treasure);
            while(read(in, &t, sizeof(t)) == sizeof(t)){
                lseek(in, pos, SEEK_SET);
                write(in, &t, sizeof(t));
                pos += sizeof(struct Treasure);
            }
            
            ftruncate(in, lseek(in, 0, SEEK_END) - sizeof(struct Treasure)); //truncate file
            struct stat info;
            if(stat(fPath, &info)){
                perror("stat");
                closedir(hunt);
                return;
            }
            if(info.st_size == 0){
                close(in);
                remove(fPath);
                break;
            } 
            printf("Treasure successfully removed\n");
            close(in);
            break;
        } 
        close(in);
    }
    closedir(hunt);
}


int rm_h(char* huntId)
{
    char cwd[CWD_SIZE];
    char sym[CWD_SIZE];
    getcwd(sym, CWD_SIZE);

    getPath(cwd, huntId);
    struct dirent* in_file;
    DIR* hunt = opendir(cwd);

    if(hunt == NULL){
        printf("Error opening hunt\n");
        return -1;
    }

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
        remove(fPath);
    }
    closedir(hunt);

    strcat(sym, "/logged_hunt_");
    strcat(sym, huntId);
    if (unlink(sym) != 0) {
        perror("unlink");
    }

    return remove(cwd);
}