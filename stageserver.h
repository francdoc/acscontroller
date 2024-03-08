#include <memory>
#include <vector>
#include <iostream>
#include "windowsnet/serversocket.h"
#include "stage.h"

/*
DATA FORMAT
+----------------------------------------+-------------+-------------+------------------+
| msgeid (8 bytes)                       | double_1 (8B)| double_2 (8B)| double_3 (8B)    |
+----------------------------------------+-------------+-------------+------------------+
| CONNECT_ACS_SIMULATOR (1)              |      0      |      0      |        0           |
| CONNECT_ACS_REAL_HARDWARE (2)          |      0      |      0      |        0           |
| CLEAR_FAULTS_ACS_STAGE (3)             |      0      |      0      |        0           |
| ENABLE_ACS_STAGE (4)                   |      0      |      0      |        0           |
| COMMUTE_ACS_STAGE (5)                  |      0      |      0      |        0           |
| SET_ACCEL_ACS_STAGE (6)                |      0      |      0      |   acceleration     |
| SET_VEL_ACS_STAGE (7)                  |      0      |  velocity   |        0           |
| MOVE_ACS_STAGE (8)                     | targetpos   |      0      |        0           |
| HALT_ACS_STAGE (9)                     |      0      |      0      |        0           |
| SET_DECEL_ACS_STAGE (10)               |      0      |      0      |   deceleration     |
| SEND_POS_ACS_STAGE (11)                |     posx    |     posy    |       posa         |
+----------------------------------------+-------------+-------------+------------------+
*/

class Stage;

const int CONNECT_ACS_SIMULATOR = 1;
const int CONNECT_ACS_REAL_HARDWARE = 2;
const int CLEAR_FAULTS_ACS_STAGE = 3;
const int ENABLE_ACS_STAGE = 4;
const int COMMUTE_ACS_STAGE = 5;
const int SET_ACCEL_ACS_STAGE = 6;
const int SET_VEL_ACS_STAGE = 7;
const int MOVE_ACS_STAGE = 8;
const int HALT_ACS_STAGE = 9;
const int SET_DECEL_ACS_STAGE = 10;
const int SEND_POS_ACS_STAGE = 11;

const int SERVER_PORT = 1000;
const int PAUSE_COMMUTE = 4000;

class Stageserver
{
public:
    Stage::Stage_system_t stage_sys;
    Stage::fpos3 pos_sys;

    int clientSocket;

    int openConnection(int port);
    void startProcessor();
    void processQueueThread();
    Stageserver() : m_pServer(std::make_unique<Serversocket>()){};
    Stage *m_pStage;

private:
    std::unique_ptr<Serversocket> m_pServer;
};