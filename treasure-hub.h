#pragma once

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "treasure-manager.h"

pid_t startMonitor();

int childHandler();

void signalHandler(int sig);