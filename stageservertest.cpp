#include <stdio.h>
#include <string>
#include <time.h>
#include <cstdio>
#include <queue>
#include <iostream>
#include <cstring>

#include "stageserver.h"

#define SIMULATED_HARDWARE
#define INIT_HARDWARE

int main()
{
    // Instantiate Stageserver
    Stageserver stageserver;

#ifdef SIMULATED_HARDWARE
    // Connect with stage simulator
    stageserver.stageConnectSimu();
#endif

#ifdef INIT_HARDWARE
    stageserver.stageInit();
#endif

    // Open connection on port 1000
    int clientSocket = stageserver.openConnection(1000);

    while (true)
    {
    }

    return 0;
}
