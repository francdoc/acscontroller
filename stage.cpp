#include "stage.h"
#include <stdio.h>
#include <string>
#include <time.h>
#include <cstdio>
#include <queue>
#include <WinSock2.h>
#include <windows.h>

#define PAUSE 5000       // millis
#define PAUSE_EXIT 50000 // millis

Stage::Stage_system_t S_system;

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

    if (S_system.ACSCptr->Enable(stage->handle, S_system.ACSCptr->Y) != 0)
    {
        printf("Error enabling Y axis.\n");
        return -1;
    }

    if (S_system.ACSCptr->Enable(stage->handle, S_system.ACSCptr->A) != 0)
    {
        printf("Error enabling A axis.\n");
        return -1;
    }

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
    Sleep(pause); // millis

    if (S_system.ACSCptr->CommuteExt(stage->handle, S_system.ACSCptr->Y) != 0)
    {
        printf("Error commuting Y axis.\n");
        return -1;
    }
    Sleep(pause); // millis

    if (S_system.ACSCptr->CommuteExt(stage->handle, S_system.ACSCptr->A) != 0)
    {
        printf("Error commuting A axis.\n");
        return -1;
    }
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

int main()
{ // quick hw test
    Stage::Stage_system_t stage_sys;
    Stage stage;

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

    if (stage.commute_axes_xya(&stage_sys, PAUSE) == -1)
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

    if (stage.get_pos_axes_xya(&stage_sys) == -1)
    {
        printf("Failed to get axes position.Exiting.\n");
        Sleep(PAUSE_EXIT);
        return -1;
    }

    printf("FAULT_X: %d\n", stage_sys.FAULT_X);
    printf("FAULT_Y: %d\n", stage_sys.FAULT_Y);
    printf("FAULT_A: %d\n", stage_sys.FAULT_A);
    printf("FPOS_X: %lf\n", stage_sys.FPOS_X);
    printf("FPOS_Y: %lf\n", stage_sys.FPOS_Y);
    printf("FPOS_A: %lf\n", stage_sys.FPOS_A);
    printf("ACSCptr: %p\n", stage_sys.ACSCptr);

    while (true)
    {
    };

    return 0;
}