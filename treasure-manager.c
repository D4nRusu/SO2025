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
    char format[CWD_SIZE];
    strcpy(format, "Hunt_");
    strcat(format, huntId);

    getPath(cwd, format);
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
    char format[20];
    strcpy(format, "Hunt_");
    strcat(format, huntId);

    getPath(cwd, format);
    int firstTime = 0;

    if(mkdir(cwd, 0755) < 0 || errno == EEXIST){
        printf("Hunt exists, moving on to adding treasure\n");
    } else{
        printf("Hunt created successfully at: %s/\nMoving on to adding treasure:\n", cwd);
        firstTime = 1;
    }
    
    struct Treasure t;
    memset(&t, 0, sizeof(t));
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
        snprintf(target, sizeof(target), "%s%s%s%s", source, PATH_SEP, format, "/logged_hunt");

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
    char format[CWD_SIZE];
    strcpy(format, "Hunt_");
    strcat(format, huntId);

    getPath(cwd, format);
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
    char format[CWD_SIZE];
    strcpy(format, "Hunt_");
    strcat(format, huntId);

    getPath(cwd, format);
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
    char format[CWD_SIZE];
    strcpy(format, "Hunt_");
    strcat(format, huntId);

    getPath(cwd, format);
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

void listHunts()
{
    DIR *dir;
    struct dirent *dent;

    dir = opendir(".");
    if(dir == NULL){
        printf("err opening directory");
        exit(-1);
    }

    printf("Hunts: ");
    int count = 0;
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
            printf("%s ", dent->d_name);
            count++;
        }
    }
    if(count == 0){
        printf("no hunts were found\n");
        return;
    }
    printf("\n");
}

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
