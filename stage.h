#include "acscontroller.h"

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

    int stage_connect(Stage_system_t *stage);
    int commute_axes_xya(Stage_system_t *stage, int pause);
    int get_fault_axes_xya(Stage_system_t *stage);
    int clear_fault_axes_xya(Stage_system_t *stage);
    int get_pos_axes_xya(Stage_system_t *stage);
    int enable_axes_xya(Stage_system_t *stage);
    int shift_stage_mm(Stage_system_t *stage, double shift_mm, double vel, double endvel);
    
private:
};
