#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <ftw.h>
#include <linux/limits.h>
#include <time.h>
#include <fcntl.h>

#define CWD_SIZE PATH_MAX
#define PATH_SEP "/"

#define CR_AP O_CREAT | O_WRONLY | O_APPEND

struct Treasure{
    uint8_t tid;
    char uname[50];
    struct {
        float lat;
        float lon;
    } GPS;
    char clue[255];
    int value;
};

void getPath(char cwd[], char* huntId);

void getSize(const char* path, int* totalSize);

void logger(char* huntId, char* op, char* param);

uint8_t add(char* huntId);

int rm_h(char* huntId);

void list(char* huntId, uint8_t tid);

void rm_t(char* huntId, uint8_t tid);

void listHunts();