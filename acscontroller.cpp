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

int ACS_Controller::GetErrorDisconnect()
{
  int Error;
  Error = acsc_GetLastError();
  printf("Transaction error: %d\n", Error);
  acsc_CloseComm(Handle);
  printf("Communication is closed.\n");
  return Error;
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

ACSC_CONNECTION_INFO ACS_Controller::GetConnInfo(HANDLE Handle)
{
  ACSC_CONNECTION_INFO ConnectionInfo;
  if (!acsc_GetConnectionInfo(Handle, &ConnectionInfo))
  {
    int Error;

    Error = GetErrorDisconnect();
    return Error;
  }
  return ConnectionInfo;
}

int ACS_Controller::StopProgram(HANDLE Handle, int programId)
{
  if (!acsc_StopBuffer(Handle, programId, ACSC_SYNCHRONOUS))
  {
    ErrorsHandler("Stop program error.\n", TRUE);
    int Error;

    Error = GetErrorDisconnect();
    return Error;
  }
  return 0;
}

int ACS_Controller::RunBufferProgram(HANDLE Handle, int Buffer, char *Label)
{
  if (!acsc_RunBuffer(Handle, Buffer, Label, ACSC_SYNCHRONOUS))
  {
    ErrorsHandler("Run program error.\n", TRUE);
    int Error;

    Error = GetErrorDisconnect();
    return Error;

    return 1;
  }
  return 0;
}

int ACS_Controller::DisconnectACS(HANDLE Handle)
{
  if (!acsc_CloseComm(Handle))
  {
    int Error;

    Error = GetErrorDisconnect();
    return Error;
  }
  printf("Communication is closed.\n");
  return 0;
}

int ACS_Controller::GetFault(HANDLE Handle, int Axis)
{
  int Fault;
  if (!acsc_GetFault(Handle, Axis, &Fault, ACSC_SYNCHRONOUS))
  {
    int Error;

    Error = GetErrorDisconnect();
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
  if (!acsc_FaultClear(Handle, Axis, ACSC_SYNCHRONOUS))
  {
    int Error;

    Error = GetErrorDisconnect();
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
  if (!acsc_Enable(Handle, Axis, ACSC_SYNCHRONOUS))
  {
    ErrorsHandler("Stop program error.\n", TRUE);
    int Error;

    Error = GetErrorDisconnect();
    return Error;
  }
  printf("Axis enabled.\n");
  return 0;
}

int ACS_Controller::DisableFault(HANDLE Handle, int Axis)
{
  if (!acsc_DisableFault(Handle, Axis, ACSC_SAFETY_LL, ACSC_SYNCHRONOUS))
  {
    int Error;
    Error = GetErrorDisconnect();
    return Error;
  }
  if (!acsc_DisableFault(Handle, Axis, ACSC_SAFETY_RL, ACSC_SYNCHRONOUS))
  {
    int Error;
    Error = GetErrorDisconnect();
    return Error;
  }
  return 0;
}

int ACS_Controller::CommuteExt(HANDLE Handle, int Axis)
{
  if (!acsc_CommutExt(Handle, Axis, ACSC_NONE, ACSC_NONE, ACSC_NONE, ACSC_SYNCHRONOUS))
  {
    int Error;
    Error = GetErrorDisconnect();
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
  if (!acsc_GetFPosition(Handle, Axis, &FPOS, ACSC_SYNCHRONOUS))
  {
    int Error;
    Error = GetErrorDisconnect();
    return Error;
  }
  else
  {
    printf("Position of encoder readout: %f\n", FPOS);
  }
  return FPOS;
}

double ACS_Controller::GetVelocity(HANDLE Handle, int Axis)
{
  double Velocity;
  if (!acsc_GetVelocity(Handle, Axis, &Velocity, ACSC_SYNCHRONOUS))
  {
    int Error;
    Error = GetErrorDisconnect();
    return Error;
  }
  return Velocity;
}

double ACS_Controller::GetAcceleration(HANDLE Handle, int Axis)
{
  double Acceleration;
  if (!acsc_GetAcceleration(Handle, Axis, &Acceleration, ACSC_SYNCHRONOUS))
  {
    int Error;
    Error = GetErrorDisconnect();
    return Error;
  }
  else
  {
    printf("Acceleration of axis readout: %f\n", Acceleration);
  }
  return Acceleration;
}
