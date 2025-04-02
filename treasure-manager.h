#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

struct Treasure{
    uint8_t tid;
    char uname[50];
    struct {
        float lat;
        float lon;
    } GPS;
    char clue[255];
    uint8_t value;
};

void add(char* huntId);
