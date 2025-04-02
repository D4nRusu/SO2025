#include "treasure-manager.h"

void add(char* huntId)
{
    char cwd[1000];
    getcwd(cwd, sizeof(cwd));

    strcat(cwd, "/");
    strcat(cwd, huntId);
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
    printf("\tValue: "); scanf("%hhd", &t.value);

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