#include <stdio.h>
#include <string>
#include <time.h>
#include <cstdio>
#include <queue>
#include <iostream>

#include "stage.h"

#define GETPOS 0
#define CRITICAL_PAUSE 5000                 // millis (do not edit; key to correct hardware configuration)
#define TARGET_POSITION_TOLERANCE 0.0000001 // mm

Stage *Stage::ControllerPtr = nullptr;

Stage *Stage::Stage_getInstance()
{
    if (ControllerPtr == nullptr)
    {
        ControllerPtr = new Stage(); // singleton pattern
        return ControllerPtr;
    }
    else
    {
        return ControllerPtr;
    }
};

Stage::Stage()
{
    printf("Stage_Controller init.\n");
};

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

    double posx = S_system.ACSCptr->GetPosition(stage->handle, S_system.ACSCptr->X);
    double posy = S_system.ACSCptr->GetPosition(stage->handle, S_system.ACSCptr->Y);
    double posa = S_system.ACSCptr->GetPosition(stage->handle, S_system.ACSCptr->A);

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

// Function to check if the stage has reached the desired position
bool Stage::is_target_position_reached(double abs_point_mm, double target_tolerance, double position)
{
    printf("---------------------------- target position check -----------------------------------\n");

    bool result = (fabs(abs_point_mm - position) <= target_tolerance);

    printf("abs_point_mm: %.6f\n", abs_point_mm);
    printf("target_tolerance: %.6f\n", target_tolerance);
    printf("position: %.6f\n", position);
    printf("Result: %s\n", result ? "True" : "False");

    return result;
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

    double last_fpos_x = 0; // to store last position and compare it with actual position to check in case of timeouts, mechanical obstructions, etc
    double last_fpos_y = 0;
    double last_fpos_a = 0;

    while (true)
    {
        printf("------------------------------- movement cycle  --------------------------------------\n");

        get_pos_axes_xya(stage); // update stage position variables

        last_fpos_x = stage->FPOS_X;
        last_fpos_y = stage->FPOS_Y;
        last_fpos_a = stage->FPOS_A;

        printf("last_fpos_x: %.6f\n", last_fpos_x);
        printf("last_fpos_y: %.6f\n", last_fpos_y);
        printf("last_fpos_a: %.6f\n", last_fpos_a);

        if (is_target_position_reached(abs_point_mm, TARGET_POSITION_TOLERANCE, last_fpos_x) &&
            is_target_position_reached(abs_point_mm, TARGET_POSITION_TOLERANCE, last_fpos_y) &&
            is_target_position_reached(abs_point_mm, TARGET_POSITION_TOLERANCE, last_fpos_a))
        {
            printf("Stage reached desired position.\n");
            return 0;
        }
        else
        {
            printf("Stage has not reached desired position yet.\n");
        }
        Sleep(1000); // ms
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