#include "stage.h"
#include <stdio.h>
#include <string>
#include <time.h>
#include <cstdio>
#include <windows.h>

Stage stage;

int main()
{ // quick hw test
    Stage::Stage_system_t stage_sys;

    // API test
    stage.stage_connect(&stage_sys);

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

    if (stage.commute_axes_xya(&stage_sys, CRITICAL_PAUSE) == -1)
    {
        printf("Failed to commute axes. Exiting.\n");
        Sleep(PAUSE_EXIT);
        return -1;
    }

    if (stage.get_fault_axes_xya(&stage_sys) == -1)
    {
        printf("Failed to get axes faults. Exiting.\n");
        Sleep(PAUSE_EXIT);
        return -1;
    }

    if (stage.shift_stage_mm(&stage_sys, 2.0, 1.5, 0.5))
    {
        printf("Failed to shift stage. Exiting.\n");
        Sleep(PAUSE_EXIT);
        return -1;
    }

    if (stage.get_pos_axes_xya(&stage_sys) == -1)
    {
        printf("Failed to get axes position.Exiting.\n");
        Sleep(PAUSE_EXIT);
        return -1;
    }

    printf("-------------------------\n");
    printf("System Status Report:\n");
    printf("-------------------------\n");

    printf("FAULT_X: %d\n", stage_sys.FAULT_X);
    printf("FAULT_Y: %d\n", stage_sys.FAULT_Y);
    printf("FAULT_A: %d\n", stage_sys.FAULT_A);
    printf("FPOS_X: %f\n", stage_sys.FPOS_X);
    printf("FPOS_Y: %f\n", stage_sys.FPOS_Y);
    printf("FPOS_A: %f\n", stage_sys.FPOS_A);
    printf("ACSCptr: %p\n", stage_sys.ACSCptr);

    printf("-------------------------\n");

    while (true)
    {
    };

    return 0;
}