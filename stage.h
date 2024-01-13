#include "acscontroller.h"

class Stage
{
public:
    typedef struct Stage_system_s
    {
        void *handle;
        double FPOS_X;
        double FPOS_Y;
        double FPOS_A;
        int FAULT_X;
        int FAULT_Y;
        int FAULT_A;
        ACS_Controller *ACSCptr;
    } Stage_system_t;

    typedef enum Stage_states_e
    {
        INIT_X,
        INIT_Y,
        INIT_A,
        IDLE_X,
        IDLE_Y,
        IDLE_A,
        MOVING_UP_X,
        MOVING_UP_Y,
        MOVING_UP_A,
        MOVING_DOWN_X,
        MOVING_DOWN_y,
        MOVING_DOWN_A,
        ON_TARGET_X,
        ON_TARGET_Y,
        ON_TARGET_A,
        ERROR_X,
        ERROR_Y,
        ERROR_A,
        STOP,
    } Stage_states_t;

    void stage_init(Stage_system_t *stage);
    int commute_axis_x(Stage_system_t *stage);
    int commute_axis_y(Stage_system_t *stage);
    int commute_axis_a(Stage_system_t *stage);

private:
};
