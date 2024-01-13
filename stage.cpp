#include "stage.h"
#include <stdio.h>
#include <string>
#include <time.h>
#include <cstdio>

#define PAUSE 5000 // millis

Stage::Stage_system_t S_system;

void Stage::stage_init(Stage_system_t *stage)
{
    S_system.ACSCptr = ACS_Controller::ACS_getInstance();

    stage->handle = S_system.ACSCptr->ConnectACS();

    stage->FAULT_X = S_system.ACSCptr->GetFault(stage->handle, S_system.ACSCptr->X);
    stage->FAULT_Y = S_system.ACSCptr->GetFault(stage->handle, S_system.ACSCptr->Y);
    stage->FAULT_A = S_system.ACSCptr->GetFault(stage->handle, S_system.ACSCptr->A);

    printf("FAULT_X INIT: %d\n", stage->FAULT_X);
    printf("FAULT_Y INIT: %d\n", stage->FAULT_Y);
    printf("FAULT_A INIT: %d\n", stage->FAULT_A);

    stage->FPOS_X = S_system.ACSCptr->GetPosition(stage->handle, S_system.ACSCptr->X);
    stage->FPOS_Y = S_system.ACSCptr->GetPosition(stage->handle, S_system.ACSCptr->Y);
    stage->FPOS_A = S_system.ACSCptr->GetPosition(stage->handle, S_system.ACSCptr->A);

    printf("FPOS_X: %lf\n", stage->FPOS_X);
    printf("FPOS_Y: %lf\n", stage->FPOS_Y);
    printf("FPOS_A: %lf\n", stage->FPOS_A);

    stage->FAULT_X = S_system.ACSCptr->ClearFault(stage->handle, S_system.ACSCptr->X);
    stage->FAULT_Y = S_system.ACSCptr->ClearFault(stage->handle, S_system.ACSCptr->Y);
    stage->FAULT_A = S_system.ACSCptr->ClearFault(stage->handle, S_system.ACSCptr->A);

    printf("FAULT_X AFTER CLEANUP: %d\n", stage->FAULT_X);
    printf("FAULT_Y AFTER CLEANUP: %d\n", stage->FAULT_Y);
    printf("FAULT_A AFTER CLEANUP: %d\n", stage->FAULT_A);

    int resenx;
    int reseny;
    int resena;

    resenx = S_system.ACSCptr->Enable(stage->handle, S_system.ACSCptr->X);
    Sleep(PAUSE);
    reseny = S_system.ACSCptr->Enable(stage->handle, S_system.ACSCptr->Y);
    Sleep(PAUSE);
    resena = S_system.ACSCptr->Enable(stage->handle, S_system.ACSCptr->A);
    Sleep(PAUSE);

    printf("Enable X axis response: %d\n", resenx);
    printf("Enable Y axis response: %d\n", reseny);
    printf("Enable A axis response: %d\n", resena);

    int commux;
    int commuy;
    int commua;

    commux = S_system.ACSCptr->CommuteExt(stage->handle, S_system.ACSCptr->X);
    Sleep(PAUSE);
    commuy = S_system.ACSCptr->CommuteExt(stage->handle, S_system.ACSCptr->Y);
    Sleep(PAUSE);
    commua = S_system.ACSCptr->CommuteExt(stage->handle, S_system.ACSCptr->A);
    Sleep(PAUSE);

    printf("Commute X axis response: %d\n", resenx);
    printf("Commute Y axis response: %d\n", reseny);
    printf("Commute A axis response: %d\n", resena);

    printf("ACS stage initialized.\n");
}

int Stage::commute_axis_x(Stage_system_t *stage)
{
    int res;
    res = S_system.ACSCptr->CommuteExt(stage->handle, S_system.ACSCptr->X);
    printf("Commute X axis response: %d\n", res);
    return res;
}

int Stage::commute_axis_y(Stage_system_t *stage)
{
    int res;
    res = S_system.ACSCptr->CommuteExt(stage->handle, S_system.ACSCptr->Y);
    printf("Commute Y axis response: %d\n", res);
    return res;
}

int Stage::commute_axis_a(Stage_system_t *stage)
{
    int res;
    res = S_system.ACSCptr->CommuteExt(stage->handle, S_system.ACSCptr->A);
    printf("Commute A axis response: %d\n", res);
    return res;
}

int main()
{ // quick hw test
    Stage::Stage_system_t stage_sys;
    Stage stage;

    // API test
    stage.stage_init(&stage_sys);

    Sleep(2500); // millis

    // method test
    stage.commute_axis_x(&stage_sys);
    stage.commute_axis_y(&stage_sys);
    stage.commute_axis_a(&stage_sys);

    Sleep(50000); // millis

    return 0;
}