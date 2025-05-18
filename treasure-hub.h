#pragma once

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "treasure-manager.h"

pid_t startMonitor();

void childHandler();

void signalHandler(int sig);

void calc_score();