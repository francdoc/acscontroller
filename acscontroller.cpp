#include "acscontroller.h"
#include "windows.h"
#include <conio.h>
#include <stdio.h>
#include <string>
#include <time.h>
#include <cstdio>
#include <iostream>
#include <ostream>

#define DEFAULT_IP "10.0.0.100"

ACS_Controller *ACS_Controller::ControllerPtr = nullptr;

ACS_Controller *ACS_Controller::ACS_getInstance()
{
  if (ControllerPtr == nullptr)
  {
    ControllerPtr = new ACS_Controller(); // singleton pattern
    return ControllerPtr;
  }
  else
  {
    return ControllerPtr;
  }
};

ACS_Controller::ACS_Controller()
{
  printf("ACS_Controller init.\n");
};

// methods
void ACS_Controller::ErrorsHandler(const char *ErrorMessage, BOOL fCloseComm)
{
  printf(ErrorMessage);
  printf("Press any key to exit.\n");
  if (fCloseComm)
    acsc_CloseComm(hComm);
  _getch();
}

HANDLE ACS_Controller::ConnectACS()
{
  char ipAddress[] = DEFAULT_IP;
  hComm = acsc_OpenCommEthernet(ipAddress, ACSC_SOCKET_STREAM_PORT);
  if (hComm == ACSC_INVALID)
  {
    ErrorsHandler("Error while opening communication.\n", TRUE);
    return hComm;
  }
  printf("Communication with ACS controller hardware was established successfully.\n");
  return hComm;
}

int ACS_Controller::StopProgram(HANDLE Handle, int programId)
{
  if (!acsc_StopBuffer(Handle, programId, ACSC_SYNCHRONOUS))
  {
    ErrorsHandler("Stop program error.\n", TRUE);
    acsc_CloseComm(hComm);
    printf("Communication is closed.\n");
    return 1;
  }
  return 0;
}

int ACS_Controller::RunBufferProgram(HANDLE Handle, int Buffer, char *Label)
{
  if (!acsc_RunBuffer(Handle, Buffer, Label, ACSC_SYNCHRONOUS))
  {
    ErrorsHandler("Run program error.\n", TRUE);
    return 1;
  }
  return 0;
}

int ACS_Controller::DisconnectACS(HANDLE Handle)
{
  int Error;
  if (!acsc_CloseComm(Handle))
  {
    Error = acsc_GetLastError();
    printf("Transaction error: %d\n", Error);
    return Error;
  }
  printf("Communication is closed.\n");
  return 0;
}

int ACS_Controller::GetFault(HANDLE Handle, int Axis)
{
  int Fault;
  int Error;
  if (!acsc_GetFault(Handle, Axis, &Fault, ACSC_SYNCHRONOUS))
  {
    Error = acsc_GetLastError();
    printf("Transaction error: %d\n", Error);
    acsc_CloseComm(Handle);
    printf("Communication is closed.\n");
    return Error;
  }
  else
  {
    printf("Fault: %d\n", Fault);
    if (Fault & ACSC_SAFETY_RL)
    {
      printf("Right Limit fault\n");
    }
    if (Fault & ACSC_SAFETY_LL)
    {
      printf("Left Limit fault\n");
    }
  }
  return Fault;
}

int ACS_Controller::ClearFault(HANDLE Handle, int Axis)
{
  int Error;
  if (!acsc_FaultClear(Handle, Axis, ACSC_SYNCHRONOUS))
  {
    Error = acsc_GetLastError();
    printf("Transaction error: %d\n", Error);
    acsc_CloseComm(Handle);
    printf("Communication is closed.\n");
    return Error;
  }
  else
  {
    printf("Faults cleared.\n");
  }
  return 0;
}

int ACS_Controller::Enable(HANDLE Handle, int Axis)
{
  int Error;
  if (!acsc_Enable(Handle, Axis, ACSC_SYNCHRONOUS))
  {
    Error = acsc_GetLastError();
    printf("Transaction error: %d\n", Error);
    ErrorsHandler("Stop program error.\n", TRUE);
    acsc_CloseComm(hComm);
    printf("Communication is closed.\n");
    return Error;
  }
  printf("Axis enabled.\n");
  return 0;
}

int ACS_Controller::DisableFault(HANDLE Handle, int Axis)
{
  int Error;
  if (!acsc_DisableFault(Handle, Axis, ACSC_SAFETY_LL, ACSC_SYNCHRONOUS))
  {
    Error = acsc_GetLastError();
    printf("Transaction error: %d\n", Error);
    acsc_CloseComm(Handle);
    printf("Communication is closed.\n");
    return Error;
  }
  if (!acsc_DisableFault(Handle, Axis, ACSC_SAFETY_RL, ACSC_SYNCHRONOUS))
  {
    Error = acsc_GetLastError();
    printf("Transaction error: %d\n", Error);
    acsc_CloseComm(Handle);
    printf("Communication is closed.\n");
    return Error;
  }
  return 0;
}

int ACS_Controller::CommuteExt(HANDLE Handle, int Axis)
{
  int Error;
  if (!acsc_CommutExt(Handle, Axis, ACSC_NONE, ACSC_NONE, ACSC_NONE, ACSC_SYNCHRONOUS))
  {
    Error = acsc_GetLastError();
    printf("Transaction error: %d\n", Error);
    acsc_CloseComm(Handle);
    printf("Communication is closed.\n");
    return Error;
  }
  else
  {
    printf("Commutation successful.\n");
  }
  return 0;
}

double ACS_Controller::GetPosition(HANDLE Handle, int Axis)
{
  double FPOS;
  if (!acsc_GetFPosition(hComm, Axis, &FPOS, ACSC_SYNCHRONOUS))
  {
    int Error;
    Error = acsc_GetLastError();
    printf("Transaction error: %d\n", Error);
    acsc_CloseComm(hComm);
    printf("Communication is closed.\n");
    return Error;
  }
  else
  {
    printf("Position of encoder readout: %f\n", FPOS);
  }
  return FPOS;
}