#include <windows.h>
#include "acscontroller.h"

// SPiiPlus MMI ACS buffer commands dictionary
#define MOVE_CMD "S_P"

// SPiiPlus MMI ACS buffer programs dictionary
#define MOVE_BUFFER_ID 7

// SPiiPlus MMI ACS buffer variables dictionary
#define VEL_VAR "velACS"
#define ACCEL_VAR "accelACS"
#define SHIFT_DISTANCE_VAR "distACS"

// Movement timeout
#define TIMEOUT_CONSTANT 100000 // ms

class Stage
{
public:
    typedef struct Stage_system_s
    {
        void *handle;
        int FAULT_X;
        int FAULT_Y;
        int FAULT_A;
        double FPOS_X;
        double FPOS_Y;
        double FPOS_A;
        ACS_Controller *ACSCptr;
    } Stage_system_t;

    struct fpos3
    {
        double x;
        double y;
        double a; // Assuming 'a' represents the third dimension
    };

    Stage::Stage_system_t S_system;

    static Stage *Stage_getInstance();

    int stage_connect(Stage_system_t *stage);
    int stage_disconnect(Stage_system_t *stage);
    int stage_simulator_connect(Stage_system_t *stage);
    int clear_fault_axes_xya(Stage_system_t *stage);
    int enable_axes_xya(Stage_system_t *stage);
    int commute_axes_xya(Stage_system_t *stage, int pause);
    int get_fault_axes_xya(Stage_system_t *stage);
    int get_pos_axes_xya(Stage_system_t *stage);
    int set_accel_axes_xya(Stage_system_t *stage, double acceleration);
    int set_decel_axes_xya(Stage_system_t *stage, double deceleration);
    int set_kill_decel_axes_xya(Stage_system_t *stage, double kill_deceleration);
    int set_vel_axes_xya(Stage_system_t *stage, double velocity);
    int move_stage_mm(Stage_system_t *stage, double abs_point_mm, double vel, double endvel);
    int move_stage_smooth_mm(Stage_system_t *stage, double abs_point_mm, double vel);
    int halt_stage(Stage_system_t *stage);
    bool is_target_position_reached(double abs_point_mm, double target_tolerance, double position);
    int run_move_stage_program(Stage_system_t *stage);
    int set_vel_stage(Stage_system_t *stage, double vel);
    int set_shift_dist_stage(Stage_system_t *stage, double shiftdist);
    int to_point_stage(Stage_system_t *stage, double shiftx_mm, double shifty_mm, double shifta_mm);    int createDirectory(const std::wstring &path);
    void writeToFile(double FPOS_X, double FPOS_Y, double FPOS_A);
    void readFromFile(const std::wstring &fileName);
    int recover_stage_position_reference(Stage_system_t *stage);
    int update_stage_position(Stage_system_t *stage, double x, double y, double a);
    int query_stage_pos(Stage_system_t *stage, fpos3 *abspos);
    Stage();

private:
    static Stage *ControllerPtr;
};
