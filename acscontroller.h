#include "ACSC.h"

class ACS_Controller
{
public:
    HANDLE hComm;

    int X = ACSC_AXIS_X;
    int Y = ACSC_AXIS_Y;
    int Z = ACSC_AXIS_Z;
    int T = ACSC_AXIS_T;
    int A = ACSC_AXIS_A;
    int B = ACSC_AXIS_B;
    int C = ACSC_AXIS_C;
    int D = ACSC_AXIS_D;

    static ACS_Controller *ACS_getInstance();

    HANDLE ConnectACS();
    HANDLE ConnectSimulatorACS();
    ACSC_CONNECTION_INFO GetConnInfo(HANDLE Handle);
    void ErrorsHandler(const char *ErrorMessage, BOOL fCloseComm, BOOL fStopMotors);
    int StopProgram(HANDLE Handle, int programId);
    int RunBufferProgram(HANDLE Handle, int Buffer, char *Label);
    int DisconnectACS(HANDLE Handle);
    int GetFault(HANDLE Handle, int Axis);
    int ClearFault(HANDLE Handle, int Axis);
    int Disable(HANDLE Handle, int Axis);
    int Enable(HANDLE Handle, int Axis);
    int DisableFault(HANDLE Handle, int Axis);
    int CommuteExt(HANDLE Handle, int Axis);
    double GetPosition(HANDLE Handle, int Axis);
    double GetVelocity(HANDLE Handle, int Axis);
    double GetAcceleration(HANDLE Handle, int Axis);
    int GetErrorDisconnect(HANDLE Handle);
    void EmergencyStop(HANDLE Handle);
    int ExtToPointM_mm(HANDLE Handle, double abs_point_mm, double vel, double endvel);
    int SmoothPointToPointMotion_mm(HANDLE Handle, double abs_point_mm, double vel);
    int HaltAxes(HANDLE Handle);
    int SetAcceleration(HANDLE Handle, int Axis, double Acceleration);
    int TrackAxisMotion(HANDLE Handle, int Axis);
    int AxisGoMotion(HANDLE Handle, int Axis);
private:
    static ACS_Controller *ControllerPtr;
    ACS_Controller();
};