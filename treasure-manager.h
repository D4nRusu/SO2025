#pragma once

#include <stdio.h>
#include <stdint.h>

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
