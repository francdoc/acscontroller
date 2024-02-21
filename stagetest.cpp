#include <stdio.h>
#include <string>
#include <time.h>
#include <cstdio>
#include <queue>
#include <iostream>
#include <cstring>

#include "stage.h"

#define SIMULATED_HARDWARE 1
#define REAL_HARDWARE 0
#define PAUSE_EXIT 1000000000 // millis
#define NETWORK_SERVER 1

int main(int argc, char *argv[])
{
    Stage::Stage_system_t stage_sys;
    Stage stage;

// API test
#if SIMULATED_HARDWARE
    stage.stage_simulator_connect(&stage_sys);
#endif

#if REAL_HARDWARE
    stage.stage_connect(&stage_sys);
#endif

    if (stage.clear_fault_axes_xya(&stage_sys) == -1)
    {
        printf("Failed to clear axes faults. Exiting.\n");
        Sleep(PAUSE_EXIT);
        return -1;
    };

    if (stage.enable_axes_xya(&stage_sys) == -1)
    {
        printf("Failed to enable axes. Exiting.\n");
        Sleep(PAUSE_EXIT);
        return -1;
    }

    if (stage.commute_axes_xya(&stage_sys, 4000) == -1)
    {
        printf("Failed to commute axes. Exiting.\n");
        Sleep(PAUSE_EXIT);
        return -1;
    }

    if (stage.set_accel_axes_xya(&stage_sys, 2) == -1)
    {
        printf("Failed to set axes acceleration. Exiting.\n");
        Sleep(PAUSE_EXIT);
        return -1;
    }

#if REAL_HARDWARE
    if (stage.get_fault_axes_xya(&stage_sys) == -1)
    {
        printf("Failed to get axes faults. Exiting.\n");
        Sleep(PAUSE_EXIT);
        return -1;
    }
#endif

    // movement test
    if (stage.move_stage_smooth_mm(&stage_sys, 1, 3) == -1)
    {
        printf("Failed to shift stage. Exiting.\n");
        Sleep(PAUSE_EXIT);
        return -1;
    }

    if (stage.move_stage_smooth_mm(&stage_sys, 0, 3) == -1)
    {
        printf("Failed to shift stage. Exiting.\n");
        Sleep(PAUSE_EXIT);
        return -1;
    }

    if (stage.move_stage_smooth_mm(&stage_sys, 3, 3) == -1)
    {
        printf("Failed to shift stage. Exiting.\n");
        Sleep(PAUSE_EXIT);
        return -1;
    }

#if GETPOS
    if (stage.get_pos_axes_xya(&stage_sys) == -1)
    {
        printf("Failed to get axes position. Exiting.\n");
        Sleep(PAUSE_EXIT);
        return -1;
    }
#endif

    printf("-------------------------\n");
    printf("System Status Report:\n");
    printf("-------------------------\n");

    printf("FAULT_X: %d\n", stage_sys.FAULT_X);
    printf("FAULT_Y: %d\n", stage_sys.FAULT_Y);
    printf("FAULT_A: %d\n", stage_sys.FAULT_A);
    printf("FPOS_X: %f\n", stage_sys.FPOS_X);
    printf("FPOS_Y: %f\n", stage_sys.FPOS_Y);
    printf("FPOS_A: %f\n", stage_sys.FPOS_A);

    printf("-------------------------\n");

    while (true)
    {
    };

    return 0;
}