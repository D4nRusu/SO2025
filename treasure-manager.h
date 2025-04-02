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

#define CWD_SIZE 1000

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

void add(char* huntId);

void list(char* huntId);