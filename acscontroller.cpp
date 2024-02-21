#include "acscontroller.h"

#include <conio.h>
#include <stdio.h>
#include <string>
#include <time.h>
#include <cstdio>
#include <iostream>
#include <ostream>
#include <cstdio>
#include <cstdlib>

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
void ACS_Controller::ErrorsHandler(const char *ErrorMessage, BOOL fCloseComm, BOOL fStopMotors)
{
  printf(ErrorMessage);
  printf("Press any key to exit.\n");
  if (fCloseComm)
    acsc_CloseComm(hComm);
  if (fStopMotors)
    EmergencyStop(hComm);
  _getch();
}

void ACS_Controller::EmergencyStop(HANDLE Handle)
{
  printf("Emergency disable of X axis.\n");
  acsc_Disable(Handle, ACSC_AXIS_X, ACSC_ASYNCHRONOUS);
  printf("Emergency disable of Y axis.\n");
  acsc_Disable(Handle, ACSC_AXIS_Y, ACSC_ASYNCHRONOUS);
  printf("Emergency disable of A axis.\n");
  acsc_Disable(Handle, ACSC_AXIS_A, ACSC_ASYNCHRONOUS);
}

int ACS_Controller::GetErrorDisconnect(HANDLE Handle)
{
  int Error;
  Error = acsc_GetLastError();
  printf("Last system error: %d\n", Error);
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
    ErrorsHandler("Error while opening communication.\n", TRUE, TRUE);
    exit(EXIT_FAILURE);
  }
  printf("Communication with ACS controller hardware was established successfully.\n");
  return hComm;
}

HANDLE ACS_Controller::ConnectSimulatorACS()
{
  char ipAddress[] = DEFAULT_IP;
  hComm = acsc_OpenCommSimulator();
  if (hComm == ACSC_INVALID)
  {
    ErrorsHandler("Error while opening communication.\n", TRUE, TRUE);
    exit(EXIT_FAILURE);
  }
  printf("Communication with ACS simulation hardware was established successfully.\n");
  return hComm;
}

ACSC_CONNECTION_INFO ACS_Controller::GetConnInfo(HANDLE Handle)
{
  ACSC_CONNECTION_INFO ConnectionInfo;
  acsc_GetConnectionInfo(Handle, &ConnectionInfo);
  printf("Got connection info.\n");
  return ConnectionInfo;
}

int ACS_Controller::StopProgram(HANDLE Handle, int programId)
{
  if (!acsc_StopBuffer(Handle, programId, ACSC_SYNCHRONOUS))
  {
    ErrorsHandler("Error trying to stop program.\n", TRUE, TRUE);
    int Error;
    Error = GetErrorDisconnect(Handle);
    return Error;
  }
  printf("Stopped program.\n");
  return 0;
}

int ACS_Controller::RunBufferProgram(HANDLE Handle, int Buffer, char *Label)
{
  if (!acsc_RunBuffer(Handle, Buffer, Label, ACSC_SYNCHRONOUS))
  {
    ErrorsHandler("Error trying to run program.\n", TRUE, TRUE);
    int Error;
    Error = GetErrorDisconnect(Handle);
    return Error;

    return 1;
  }
  printf("Ran buffer program.\n");
  return 0;
}

int ACS_Controller::DisconnectACS(HANDLE Handle)
{
  if (!acsc_CloseComm(Handle))
  {
    int Error;
    Error = GetErrorDisconnect(Handle);
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
    Error = GetErrorDisconnect(Handle);
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
  printf("Requesting fault status.\n");
  return Fault;
}

int ACS_Controller::ClearFault(HANDLE Handle, int Axis)
{
  if (!acsc_FaultClear(Handle, Axis, ACSC_SYNCHRONOUS))
  {
    int Error;
    Error = GetErrorDisconnect(Handle);
    return Error;
  }
  printf("Fault cleared on axis %d.\n", Axis);
  return 0;
}

int ACS_Controller::Enable(HANDLE Handle, int Axis)
{
  if (!acsc_Enable(Handle, Axis, ACSC_SYNCHRONOUS))
  {
    ErrorsHandler("Stop program error.\n", TRUE, TRUE);
    int Error;
    Error = GetErrorDisconnect(Handle);
    return Error;
  }
  printf("Enabled axis %d.\n", Axis);
  return 0;
}

int ACS_Controller::Disable(HANDLE Handle, int Axis)
{
  if (!acsc_Disable(Handle, Axis, ACSC_SYNCHRONOUS))
  {
    ErrorsHandler("Stop program error.\n", TRUE, TRUE);
    int Error;
    Error = GetErrorDisconnect(Handle);
    return Error;
  }
  printf("Disabled axis %d.\n", Axis);
  return 0;
}

int ACS_Controller::DisableFault(HANDLE Handle, int Axis)
{
  if (!acsc_DisableFault(Handle, Axis, ACSC_SAFETY_LL, ACSC_SYNCHRONOUS))
  {
    int Error;
    Error = GetErrorDisconnect(Handle);
    return Error;
  }
  if (!acsc_DisableFault(Handle, Axis, ACSC_SAFETY_RL, ACSC_SYNCHRONOUS))
  {
    int Error;
    Error = GetErrorDisconnect(Handle);
    return Error;
  }
  printf("Faults disabled.\n");
  return 0;
}

int ACS_Controller::CommuteExt(HANDLE Handle, int Axis)
{
  if (!acsc_CommutExt(Handle, Axis, ACSC_NONE, ACSC_NONE, ACSC_NONE, ACSC_SYNCHRONOUS))
  {
    int Error;
    Error = GetErrorDisconnect(Handle);
    return Error;
  }
  printf("Commuted axis %d.\n", Axis);
  return 0;
}

double ACS_Controller::GetPosition(HANDLE Handle, int Axis)
{
  double FPOS;
  if (!acsc_GetFPosition(Handle, Axis, &FPOS, ACSC_SYNCHRONOUS))
  {
    int Error;
    Error = GetErrorDisconnect(Handle);
    return Error;
  }
  printf("Position of encoder readout: %f\n", FPOS);
  return FPOS;
}

double ACS_Controller::GetVelocity(HANDLE Handle, int Axis)
{
  double Velocity;
  if (!acsc_GetVelocity(Handle, Axis, &Velocity, ACSC_SYNCHRONOUS))
  {
    int Error;
    Error = GetErrorDisconnect(Handle);
    return Error;
  }
  printf("Velocity of axis readout: %f\n", Velocity);
  return Velocity;
}

double ACS_Controller::GetAcceleration(HANDLE Handle, int Axis)
{
  double Acceleration;
  if (!acsc_GetAcceleration(Handle, Axis, &Acceleration, ACSC_SYNCHRONOUS))
  {
    int Error;
    Error = GetErrorDisconnect(Handle);
    return Error;
  }
  printf("Acceleration of axis readout: %f\n", Acceleration);
  return Acceleration;
}

int ACS_Controller::SetAcceleration(HANDLE Handle, int Axis, double Acceleration)
{
  if (!acsc_SetAcceleration(Handle, Axis, Acceleration, ACSC_SYNCHRONOUS))
  {
    int Error;
    Error = GetErrorDisconnect(Handle);
    return Error;
  }
  return 0;
}

// example of the waiting call of acsc_SetAcceleration

int ACS_Controller::ExtToPointM_mm(HANDLE Handle, double abs_point_mm, double vel, double endvel)
{
  int Axes[] = {ACSC_AXIS_X, ACSC_AXIS_Y, ACSC_AXIS_A, -1};
  double Target[]= {abs_point_mm, abs_point_mm, abs_point_mm, -1};

  if (!acsc_ExtToPointM(Handle, ACSC_AMF_VELOCITY | ACSC_AMF_ENDVELOCITY, Axes, Target, vel, endvel, ACSC_SYNCHRONOUS))
  {
    int Error;
    Error = GetErrorDisconnect(Handle);
    return Error;
  }
  return 0;
}

int ACS_Controller::SmoothPointToPointMotion_mm(HANDLE Handle, double abs_point_mm, double vel)
{
  int Axes[] = {ACSC_AXIS_X, ACSC_AXIS_Y, ACSC_AXIS_A, -1};
  double Target[] = {abs_point_mm, abs_point_mm, abs_point_mm,-1};

  if (!acsc_SmoothPointToPointMotion(Handle, ACSC_AMF_ENVELOPE, Axes, Target, vel, ACSC_SYNCHRONOUS))
  {
    int Error;
    Error = GetErrorDisconnect(Handle);
    return Error;
  }
  return 0;
}

int ACS_Controller::TrackAxisMotion(HANDLE Handle, int Axis)
{
  if (!acsc_Track(Handle, ACSC_AMF_WAIT, Axis, ACSC_SYNCHRONOUS))
  {
    int Error;
    Error = GetErrorDisconnect(Handle);
    return Error;
  }
  return 0;
}

int ACS_Controller::AxisGoMotion(HANDLE Handle, int Axis)
{
  if (!acsc_Go(Handle, Axis, ACSC_SYNCHRONOUS))
  {
    int Error;
    Error = GetErrorDisconnect(Handle);
    return Error;
  }
  return 0;
}

int ACS_Controller::HaltAxes(HANDLE Handle)
{
  int Axes[] = {ACSC_AXIS_X, ACSC_AXIS_Y, ACSC_AXIS_A, -1};
  if (!acsc_HaltM(Handle, Axes, ACSC_SYNCHRONOUS))
  {
    int Error;
    Error = GetErrorDisconnect(Handle);
    return Error;
  }
  return 0;
}
