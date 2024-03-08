#include <stdio.h>
#include <string>
#include <time.h>
#include <cstdio>
#include <queue>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <cwchar>
#include <sstream>
#include <vector>
#include <algorithm>

#include "stage.h"

#define GETPOS 0
#define CRITICAL_PAUSE 5000                 // millis (do not edit; key to correct hardware configuration)
#define TARGET_POSITION_TOLERANCE 0.0000001 // mm

Stage *Stage::ControllerPtr = nullptr;

Stage *Stage::Stage_getInstance()
{
    if (ControllerPtr == nullptr)
    {
        ControllerPtr = new Stage(); // singleton pattern
        return ControllerPtr;
    }
    else
    {
        return ControllerPtr;
    }
};

Stage::Stage()
{
    printf("Stage_Controller init.\n");
};

int Stage::stage_connect(Stage_system_t *stage)
{
    printf("Connecting to real hardware stage.\n");
    S_system.ACSCptr = ACS_Controller::ACS_getInstance();
    stage->handle = S_system.ACSCptr->ConnectACS();
    return 0;
}

int Stage::stage_disconnect(Stage_system_t *stage)
{
    printf("Disconnecting from stage.\n");
    S_system.ACSCptr->DisconnectACS(stage->handle);
    return 0;
}

int Stage::stage_simulator_connect(Stage_system_t *stage)
{
    printf("Connecting to stage simulation.\n");
    S_system.ACSCptr = ACS_Controller::ACS_getInstance();
    stage->handle = S_system.ACSCptr->ConnectSimulatorACS();
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

    double posx = S_system.ACSCptr->GetPosition(stage->handle, S_system.ACSCptr->X);
    double posy = S_system.ACSCptr->GetPosition(stage->handle, S_system.ACSCptr->Y);
    double posa = S_system.ACSCptr->GetPosition(stage->handle, S_system.ACSCptr->A);

    stage->FPOS_X = posx;
    stage->FPOS_Y = posy;
    stage->FPOS_A = posa;

    return 0;
}

int Stage::set_accel_axes_xya(Stage_system_t *stage, double acceleration)
{
    printf("Setting axes acceleration.\n");

    int accx = S_system.ACSCptr->SetAcceleration(stage->handle, S_system.ACSCptr->X, acceleration);
    int accy = S_system.ACSCptr->SetAcceleration(stage->handle, S_system.ACSCptr->Y, acceleration);
    int acca = S_system.ACSCptr->SetAcceleration(stage->handle, S_system.ACSCptr->A, acceleration);

    if (accx != 0)
    {
        printf("Error setting X axis acceleration.\n");
        return -1;
    }

    if (accy != 0)
    {
        printf("Error setting Y axis acceleration.\n");
        return -1;
    }

    if (acca != 0)
    {
        printf("Error setting A axis acceleration.\n");
        return -1;
    }

    return 0;
}

int Stage::set_decel_axes_xya(Stage_system_t *stage, double deceleration)
{
    printf("Setting axes deceleration.\n");

    int decex = S_system.ACSCptr->SetDeceleration(stage->handle, S_system.ACSCptr->X, deceleration);
    int decey = S_system.ACSCptr->SetDeceleration(stage->handle, S_system.ACSCptr->Y, deceleration);
    int decea = S_system.ACSCptr->SetDeceleration(stage->handle, S_system.ACSCptr->A, deceleration);

    if (decex != 0)
    {
        printf("Error setting X axis deceleration.\n");
        return -1;
    }

    if (decey != 0)
    {
        printf("Error setting Y axis deceleration.\n");
        return -1;
    }

    if (decea != 0)
    {
        printf("Error setting A axis deceleration.\n");
        return -1;
    }

    return 0;
}

int Stage::set_kill_decel_axes_xya(Stage_system_t *stage, double kill_deceleration)
{
    printf("Setting axes deceleration.\n");

    int killdecex = S_system.ACSCptr->SetKillDeceleration(stage->handle, S_system.ACSCptr->X, kill_deceleration);
    int killdecey = S_system.ACSCptr->SetKillDeceleration(stage->handle, S_system.ACSCptr->Y, kill_deceleration);
    int killdecea = S_system.ACSCptr->SetKillDeceleration(stage->handle, S_system.ACSCptr->A, kill_deceleration);

    if (killdecex != 0)
    {
        printf("Error setting X axis kill deceleration.\n");
        return -1;
    }

    if (killdecey != 0)
    {
        printf("Error setting Y axis kill deceleration.\n");
        return -1;
    }

    if (killdecea != 0)
    {
        printf("Error setting A axis kill deceleration.\n");
        return -1;
    }

    return 0;
}

int Stage::set_vel_axes_xya(Stage_system_t *stage, double velocity)
{
    printf("Setting axes velocity.\n");

    int velx = S_system.ACSCptr->SetVelocity(stage->handle, S_system.ACSCptr->X, velocity);
    int vely = S_system.ACSCptr->SetVelocity(stage->handle, S_system.ACSCptr->Y, velocity);
    int vela = S_system.ACSCptr->SetVelocity(stage->handle, S_system.ACSCptr->A, velocity);

    if (velx != 0)
    {
        printf("Error setting X axis velocity.\n");
        return -1;
    }

    if (vely != 0)
    {
        printf("Error setting Y axis velocity.\n");
        return -1;
    }

    if (vela != 0)
    {
        printf("Error setting A axis velocity.\n");
        return -1;
    }

    return 0;
}

// untested
int Stage::move_stage_mm(Stage_system_t *stage, double abs_point_mm, double vel, double endvel)
{
    // Absolute position movement.
    if (S_system.ACSCptr->ExtToPointM_mm(stage->handle, abs_point_mm, vel, endvel) != 0)
    {
        printf("Error executing command to shift stage.\n");
        return -1;
    }
    printf("Command to shift stage successful.\n");
    return 0;
}

bool Stage::is_target_position_reached(double abs_point_mm, double target_tolerance, double position)
{
    printf("---------------------------- target position check -----------------------------------\n");

    bool result = (fabs(abs_point_mm - position) <= target_tolerance);

    printf("abs_point_mm: %.6f\n", abs_point_mm);
    printf("target_tolerance: %.6f\n", target_tolerance);
    printf("position: %.6f\n", position);
    printf("Result: %s\n", result ? "True" : "False");

    return result;
}

int Stage::createDirectory(const std::wstring &path)
{
    CreateDirectoryW(path.c_str(), NULL);
    return 0;
}

void Stage::writeToFile(double FPOS_X, double FPOS_Y, double FPOS_A)
{
    // Get the current date
    auto now = std::chrono::system_clock::now();
    auto date = std::chrono::system_clock::to_time_t(now);

    // Create a string with the date in "YYYYMMDD" format
    std::wstringstream ss;
    ss << std::put_time(std::localtime(&date), L"%Y%m%d");

    // Construct the folder name and file name with the date
    std::wstring folderName = L"position_logs";
    std::wstring fileName = folderName + L"\\STAGE_" + ss.str() + L".txt";

    // Create the folder if it doesn't exist
    createDirectory(folderName);

    // Open the file for writing using fwprintf
    FILE *outputFile = _wfopen(fileName.c_str(), L"w");

    if (outputFile != nullptr)
    {
        // Set the precision you want (e.g., 12 digits after the decimal point)
        int precision = 12;

        // Write the formatted string to the file with increased precision
        fwprintf(outputFile, L"FPOS %.12f %.12f %.12f", FPOS_X, FPOS_Y, FPOS_A);

        // Close the file
        fclose(outputFile);

        // Write to console using fwprintf
        fwprintf(stdout, L"Values written to file '%s' successfully.\n", fileName.c_str());
    }
    else
    {
        fwprintf(stderr, L"Unable to open file for writing.\n");
    }
}

void Stage::readFromFile(const std::wstring &fileName)
{
    // Open the file for reading
    FILE *inputFile = _wfopen(fileName.c_str(), L"r");

    if (inputFile != nullptr)
    {
        // Read the values from the file
        double read_FPOS_X, read_FPOS_Y, read_FPOS_A;
        int result = fwscanf(inputFile, L"FPOS %lf %lf %lf", &read_FPOS_X, &read_FPOS_Y, &read_FPOS_A);

        // Close the file
        fclose(inputFile);

        if (result == 3)
        {
            // Print the read values
            wprintf(L"Read values from file '%s': FPOS_X=%.12f, FPOS_Y=%.12f, FPOS_A=%.12f\n",
                    fileName.c_str(), read_FPOS_X, read_FPOS_Y, read_FPOS_A);
        }
        else
        {
            fwprintf(stderr, L"Error reading values from file '%s'.\n", fileName.c_str());
        }
    }
    else
    {
        fwprintf(stderr, L"Unable to open file for reading.\n");
    }
}

int Stage::recover_stage_position_reference(Stage_system_t *stage)
{
    // Specify the folder name
    std::wstring folderName = L"position_logs";

    // Create a search pattern to find files in the folder
    std::wstring searchPattern = folderName + L"\\STAGE_*.txt";

    WIN32_FIND_DATAW findFileData; // Use wide-character version of WIN32_FIND_DATA
    HANDLE hFind = FindFirstFileW(searchPattern.c_str(), &findFileData);

    double read_FPOS_X, read_FPOS_Y, read_FPOS_A;

    if (hFind != INVALID_HANDLE_VALUE)
    {
        // Store file names and dates in a vector
        std::vector<std::wstring> fileNames;

        do
        {
            fileNames.push_back(findFileData.cFileName);
        } while (FindNextFileW(hFind, &findFileData) != 0);

        // Close the search handle
        FindClose(hFind);

        // Sort file names based on date (assuming the file name format is consistent)
        std::sort(fileNames.begin(), fileNames.end());

        // Get the latest file name
        std::wstring latestFileName = folderName + L"\\" + fileNames.back();

        // Open the latest file for reading
        FILE *inputFile = _wfopen(latestFileName.c_str(), L"r");

        if (inputFile != nullptr)
        {
            // Read the values from the file
            int result = fwscanf(inputFile, L"FPOS %lf %lf %lf", &read_FPOS_X, &read_FPOS_Y, &read_FPOS_A);

            // Close the file
            fclose(inputFile);

            if (result == 3)
            {
                // Print the read values
                wprintf(L"Read values from the latest file '%s': FPOS_X=%.12f, FPOS_Y=%.12f, FPOS_A=%.12f\n",
                        latestFileName.c_str(), read_FPOS_X, read_FPOS_Y, read_FPOS_A);
                update_stage_position(stage, read_FPOS_X, read_FPOS_Y, read_FPOS_A);
            }
            else
            {
                fwprintf(stderr, L"Error reading values from the latest file '%s'.\n", latestFileName.c_str());
            }
        }
        else
        {
            fwprintf(stderr, L"Unable to open the latest file for reading.\n");
        }
    }
    else
    {
        fwprintf(stderr, L"No files found in the folder '%s'.\n", folderName.c_str());
    }
    return 0;
}

int Stage::to_point_stage(Stage_system_t *stage, double shiftx_mm, double shifty_mm, double shifta_mm)
{
    double fpos_x_init = 0;
    double fpos_y_init = 0;
    double fpos_a_init = 0;

    get_pos_axes_xya(stage); // get initial stage position variables

    fpos_x_init = stage->FPOS_X;
    fpos_y_init = stage->FPOS_Y;
    fpos_a_init = stage->FPOS_A;

    if (S_system.ACSCptr->ToPointM(stage->handle, shiftx_mm, shifty_mm, shifta_mm) != 0)
    {
        printf("Error executing command to shift stage.\n");
        return -1;
    }

    int timx = S_system.ACSCptr->AxisMovement_timeout_ms(stage->handle, S_system.ACSCptr->X, TIMEOUT_CONSTANT);
    int timy = S_system.ACSCptr->AxisMovement_timeout_ms(stage->handle, S_system.ACSCptr->Y, TIMEOUT_CONSTANT);
    int tima = S_system.ACSCptr->AxisMovement_timeout_ms(stage->handle, S_system.ACSCptr->A, TIMEOUT_CONSTANT);

    if (timx != 0)
    {
        printf("Error setting X axis movement timeout.\n");
        return -1;
    }

    if (timy != 0)
    {
        printf("Error setting Y axis movement timeout.\n");
        return -1;
    }

    if (tima != 0)
    {
        printf("Error setting A axis movement timeout.\n");
        return -1;
    }

    double fpos_x_post = 0;
    double fpos_y_post = 0;
    double fpos_a_post = 0;

    get_pos_axes_xya(stage); // update stage position variables

    fpos_x_post = stage->FPOS_X;
    fpos_y_post = stage->FPOS_Y;
    fpos_a_post = stage->FPOS_A;

    // Call the function to write values to the file
    writeToFile(fpos_x_post, fpos_y_post, fpos_a_post);

    if (is_target_position_reached(fpos_x_init + shiftx_mm, TARGET_POSITION_TOLERANCE, fpos_x_post) &&
        is_target_position_reached(fpos_y_init + shifty_mm, TARGET_POSITION_TOLERANCE, fpos_y_post) &&
        is_target_position_reached(fpos_a_init + shifta_mm, TARGET_POSITION_TOLERANCE, fpos_a_post))
    {
        printf("Stage reached desired position.\n");
        return 0;
    }
    else
    {
        printf("Stage has not reached desired position yet.\n");
        return -1;
    }
}

int Stage::query_stage_pos(Stage_system_t *stage, fpos3 *abspos)
{
    double fpos_x = 0;
    double fpos_y = 0;
    double fpos_a = 0;

    get_pos_axes_xya(stage); // Assuming this function updates stage position variables

    fpos_x = stage->FPOS_X;
    fpos_y = stage->FPOS_Y;
    fpos_a = stage->FPOS_A;

    // Update the abspos vector with the stage position values
    abspos->x = fpos_x;
    abspos->y = fpos_y;
    abspos->a = fpos_a;
    return 0;
}

// untested
int Stage::move_stage_smooth_mm(Stage_system_t *stage, double abs_point_mm, double vel)
{
    // Absolute position movement.
    if (S_system.ACSCptr->SmoothPointToPointMotion_mm(stage->handle, abs_point_mm, vel) != 0)
    {
        printf("Error executing command to shift stage.\n");
        return -1;
    }
    printf("Command to shift stage successful.\n");
    return 0;
}

int Stage::halt_stage(Stage_system_t *stage)
{
    if (S_system.ACSCptr->HaltAxes(stage->handle) != 0)
    {
        printf("Error halting stage.\n");
        return -1;
    }
    printf("Halted stage.\n");
    return 0;
}

// untested
int Stage::run_move_stage_program(Stage_system_t *stage)
{
    char cmd[] = MOVE_CMD;
    if (S_system.ACSCptr->RunBufferProgram(stage->handle, MOVE_BUFFER_ID, cmd) != 0)
    {
        printf("Error running stage movement buffer.\n");
        return -1;
    }
    printf("Halted stage.\n");
    return 0;
}

int Stage::set_vel_stage(Stage_system_t *stage, double vel)
{
    char cmd[] = VEL_VAR;
    printf("Setting stage velocity to: %lf\n", vel);
    if (S_system.ACSCptr->WriteInternalVar(stage->handle, cmd, vel) != 0)
    {
        printf("Error setting stage velocity value.\n");
        return -1;
    }
    printf("Setted stage velocity.\n");
    return 0;
}

// untested
int Stage::set_shift_dist_stage(Stage_system_t *stage, double shiftdist)
{
    char cmd[] = SHIFT_DISTANCE_VAR;
    if (S_system.ACSCptr->WriteInternalVar(stage->handle, cmd, shiftdist) != 0)
    {
        printf("Error setting stage shift distance value.\n");
        return -1;
    }
    printf("Setted stage shift distance value.\n");
    return 0;
}

int Stage::update_stage_position(Stage_system_t *stage, double x, double y, double a)
{
    int setpx = S_system.ACSCptr->SetPosition(stage->handle, S_system.ACSCptr->X, x);
    int setpy = S_system.ACSCptr->SetPosition(stage->handle, S_system.ACSCptr->Y, y);
    int setpa = S_system.ACSCptr->SetPosition(stage->handle, S_system.ACSCptr->A, a);

    if (setpx != 0)
    {
        printf("Error setting X axis position feedback value.\n");
        return -1;
    }

    if (setpy != 0)
    {
        printf("Error setting Y axis position feedback value.\n");
        return -1;
    }

    if (setpa != 0)
    {
        printf("Error setting Y axis position feedback value.\n");
        return -1;
    }

    return 0;
}