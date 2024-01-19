#include "stage.h"
#include <stdio.h>
#include <string>
#include <time.h>
#include <cstdio>
#include <windows.h>

#define CRITICAL_PAUSE 5000 // millis (DO NOT EDIT -> key to correct hardware configuration)
#define PAUSE_EXIT 50000    // millis

Stage::Stage_system_t S_system;

Stage::Stage()
{
    printf("Stage class instanced.\n")
}

int Stage::stage_connect(Stage_system_t *stage)
{
    printf("Connecting to stage.\n");
    S_system.ACSCptr = ACS_Controller::ACS_getInstance();
    stage->handle = S_system.ACSCptr->ConnectACS();
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

    if (posx != 0)
    {
        printf("Error getting X axis position.\n");
        return -1;
    }
    stage->FPOS_X = posx;

    if (posy != 0)
    {
        printf("Error getting Y axis position.\n");
        return -1;
    }
    stage->FPOS_Y = posy;

    if (posa != 0)
    {
        printf("Error getting A axis position.\n");
        return -1;
    }
    stage->FPOS_A = posa;

    return 0;
}

int Stage::shift_xya_mm(Stage_system_t *stage, double shift_mm, double vel, double endvel)
{
    int Axes_XYA[] = {S_system.ACSCptr->X, S_system.ACSCptr->X, S_system.ACSCptr->X, -1};
    if (S_system.ACSCptr->ShiftAxes(stage->handle, shift_mm, vel, endvel, Axes_XYA) != 0)
    {
        printf("Error shifting stage.\n");
        return -1;
    }
    printf("Shifted stage.\n");
    return 0;
}