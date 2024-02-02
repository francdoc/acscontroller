#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#include <stdio.h>
#include <string>
#include <time.h>
#include <cstdio>
#include <queue>
#include <windows.h>

#include "stage.h"

#define SIMULATED_HARDWARE 1
#define REAL_HARDWARE 0
#define GETPOS 0

#define NETWORK_SERVER 1

#define CRITICAL_PAUSE 5000 // millis (do not edit; key to correct hardware configuration)
#define PAUSE_EXIT 50000    // millis

Stage::Stage_system_t S_system;

int Stage::stage_connect(Stage_system_t *stage)
{
    printf("Connecting to stage.\n");
    S_system.ACSCptr = ACS_Controller::ACS_getInstance();
    stage->handle = S_system.ACSCptr->ConnectACS();
    return 0;
}

int Stage::stage_simulator_connect(Stage_system_t *stage)
{
    printf("Connecting to stage simulation.\n");
    S_system.ACSCptr = ACS_Controller::ACS_getInstance();
    stage->handle = S_system.ACSCptr->ConnectSimulatorACS();
    return 0;
}

int Stage::clear_fault_axes_xya(Stage_system_t *stage)
{
    printf("Clearing axes faults.\n");
    stage->FAULT_X = S_system.ACSCptr->ClearFault(stage->handle, S_system.ACSCptr->X);
    if ((stage->FAULT_X) != 0)
    {
        printf("Error clearing X axis fault.\n");
        return -1;
    }
    stage->FAULT_Y = S_system.ACSCptr->ClearFault(stage->handle, S_system.ACSCptr->Y);
    if ((stage->FAULT_Y) != 0)
    {
        printf("Error clearing Y axis fault.\n");
        return -1;
    }
    stage->FAULT_A = S_system.ACSCptr->ClearFault(stage->handle, S_system.ACSCptr->A);
    if ((stage->FAULT_A) != 0)
    {
        printf("Error clearing A axis fault.\n");
        return -1;
    }
    return 0;
}

int Stage::enable_axes_xya(Stage_system_t *stage)
{
    printf("Enabling axes.\n");

    if (S_system.ACSCptr->Enable(stage->handle, S_system.ACSCptr->X) != 0)
    {
        printf("Error enabling X axis.\n");
        return -1;
    }
    Sleep(CRITICAL_PAUSE);

    if (S_system.ACSCptr->Enable(stage->handle, S_system.ACSCptr->Y) != 0)
    {
        printf("Error enabling Y axis.\n");
        return -1;
    }
    Sleep(CRITICAL_PAUSE);

    if (S_system.ACSCptr->Enable(stage->handle, S_system.ACSCptr->A) != 0)
    {
        printf("Error enabling A axis.\n");
        return -1;
    }
    Sleep(CRITICAL_PAUSE);

    return 0;
}

int Stage::commute_axes_xya(Stage_system_t *stage, int pause)
{
    printf("Commuting axes.\n");

    if (S_system.ACSCptr->CommuteExt(stage->handle, S_system.ACSCptr->X) != 0)
    {
        printf("Error commuting X axis.\n");
        return -1;
    }
    printf("X (0) axis commuted.\n");
    Sleep(pause); // millis

    if (S_system.ACSCptr->CommuteExt(stage->handle, S_system.ACSCptr->Y) != 0)
    {
        printf("Error commuting Y axis.\n");
        return -1;
    }
    printf("Y (1) axis commuted.\n");
    Sleep(pause); // millis

    if (S_system.ACSCptr->CommuteExt(stage->handle, S_system.ACSCptr->A) != 0)
    {
        printf("Error commuting A axis.\n");
        return -1;
    }
    printf("A (4) axis commuted.\n");
    Sleep(pause); // millis

    return 0;
}

int Stage::get_fault_axes_xya(Stage_system_t *stage)
{
    printf("Getting axes faults.\n");

    int faultx = S_system.ACSCptr->GetFault(stage->handle, S_system.ACSCptr->X);
    int faulty = S_system.ACSCptr->GetFault(stage->handle, S_system.ACSCptr->Y);
    int faulta = S_system.ACSCptr->GetFault(stage->handle, S_system.ACSCptr->A);

    if (faultx != 0)
    {
        printf("Error getting X axis fault.\n");
        return -1;
    }
    stage->FAULT_X = faultx;

    if (faulty != 0)
    {
        printf("Error getting Y axis fault.\n");
        return -1;
    }
    stage->FAULT_Y = faulty;

    if (faulta != 0)
    {
        printf("Error getting A axis fault.\n");
        return -1;
    }
    stage->FAULT_A = faulta;

    return 0;
}

int Stage::get_pos_axes_xya(Stage_system_t *stage)
{
    printf("Getting axes positions.\n");

    int posx = S_system.ACSCptr->GetPosition(stage->handle, S_system.ACSCptr->X);
    int posy = S_system.ACSCptr->GetPosition(stage->handle, S_system.ACSCptr->Y);
    int posa = S_system.ACSCptr->GetPosition(stage->handle, S_system.ACSCptr->A);

    stage->FPOS_X = posx;
    stage->FPOS_Y = posy;
    stage->FPOS_A = posa;

    return 0;
}

int Stage::set_accel_axes_xya(Stage_system_t *stage, double acceleration)
{
    int accx = S_system.ACSCptr->SetAcceleration(stage->handle, S_system.ACSCptr->X, acceleration);
    int accy = S_system.ACSCptr->SetAcceleration(stage->handle, S_system.ACSCptr->Y, acceleration);
    int acca = S_system.ACSCptr->SetAcceleration(stage->handle, S_system.ACSCptr->A, acceleration);

    if (accx != 0)
    {
        printf("Error setting X axis acceleration.\n");
        return -1;
    }

    if (accy != 0)
    {
        printf("Error setting Y axis acceleration.\n");
        return -1;
    }

    if (acca != 0)
    {
        printf("Error setting A axis acceleration.\n");
        return -1;
    }

    return 0;
}

int Stage::move_stage_mm(Stage_system_t *stage, double abs_point_mm, double vel, double endvel)
{
    // Absolute position movement.
    if (S_system.ACSCptr->ExtToPointM_mm(stage->handle, abs_point_mm, vel, endvel) != 0)
    {
        printf("Error executing command to shift stage.\n");
        return -1;
    }
    printf("Command to shift stage successful.\n");
    return 0;
}

int Stage::move_stage_smooth_mm(Stage_system_t *stage, double abs_point_mm, double vel)
{
    // Absolute position movement.
    if (S_system.ACSCptr->SmoothPointToPointMotion_mm(stage->handle, abs_point_mm, vel) != 0)
    {
        printf("Error executing command to shift stage.\n");
        return -1;
    }

    printf("Command to shift stage successful.\n");

    while (true)
    {
        get_pos_axes_xya(stage); // update stage position variables
        if ((stage->FPOS_X == abs_point_mm) && (stage->FPOS_Y == abs_point_mm) && (stage->FPOS_A == abs_point_mm))
        {
            printf("Target stage position reached.\n");
            break;
        }
        Sleep(500); // ms
    }
    return 0;
}

// Prototype function, not used
int Stage::track_x_axis_motion(Stage_system_t *stage)
{
    if (S_system.ACSCptr->TrackAxisMotion(stage->handle, S_system.ACSCptr->X) != 0)
    {
        printf("Error tracking motion.\n");
        return -1;
    }
    printf("Tracked motion.\n");
    return 0;
}

int Stage::halt_stage(Stage_system_t *stage)
{
    if (S_system.ACSCptr->HaltAxes(stage->handle) != 0)
    {
        printf("Error halting stage.\n");
        return -1;
    }
    printf("Halted stage.\n");
    return 0;
}

int main()
{ // quick hw test
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

    if (stage.commute_axes_xya(&stage_sys, CRITICAL_PAUSE) == -1)
    {
        printf("Failed to commute axes. Exiting.\n");
        Sleep(PAUSE_EXIT);
        return -1;
    }

    if (stage.set_accel_axes_xya(&stage_sys, 25) == -1)
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
    if (stage.move_stage_smooth_mm(&stage_sys, 24, 25) == -1)
    {
        printf("Failed to shift stage. Exiting.\n");
        Sleep(PAUSE_EXIT);
        return -1;
    }

    if (stage.move_stage_smooth_mm(&stage_sys, 5, 25) == -1)
    {
        printf("Failed to shift stage. Exiting.\n");
        Sleep(PAUSE_EXIT);
        return -1;
    }

    if (stage.move_stage_smooth_mm(&stage_sys, 10, 25) == -1)
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