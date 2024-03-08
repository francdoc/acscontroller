#include "stageserver.h"

#define USE_STAGE

int Stageserver::openConnection(int port)
{
    m_pStage = Stage::Stage_getInstance();
    m_pServer = std::unique_ptr<Serversocket>(new Serversocket());
    SOCKET serverSocket = m_pServer->createSocket();
    sockaddr_in serverAddr = m_pServer->configSocket(port);
    m_pServer->bindSocket(serverSocket, serverAddr);
    m_pServer->listenCon(serverSocket);
    clientSocket = m_pServer->acceptCon(serverSocket);
    m_pServer->getHostInfo();
    m_pServer->startReader(clientSocket);
    startProcessor();
    return clientSocket;
}

void Stageserver::startProcessor()
{
    std::thread processorThread(&Stageserver::processQueueThread, this);
    processorThread.detach();
}

void Stageserver::processQueueThread()
{
    while (true)
    {
        std::vector<double> ctrl_data = m_pServer->processQueueElement();

        if (!ctrl_data.empty())
        {
            printf("Received ctrl_data -> number of double values: %lu.\n", ctrl_data.size());
            int msgid = static_cast<int>(ctrl_data[0]);
            double shiftx, shifty, shifta, velocity, acceleration, deceleration, shiftdist, x_value, y_value, a_value;

            // System response to network client
            int res = 0; // Initialize to a default value

            // Stage system callbacks
            try
            {
                switch (msgid)
                {
                case CONNECT_ACS_SIMULATOR:
                    res = m_pStage->stage_simulator_connect(&stage_sys);
                    m_pServer->respond_to_client(clientSocket, res);
                    break;

                case CONNECT_ACS_REAL_HARDWARE:
                    res = m_pStage->stage_connect(&stage_sys);
                    m_pServer->respond_to_client(clientSocket, res);
                    break;

                case CLEAR_FAULTS_ACS_STAGE:
                    res = m_pStage->clear_fault_axes_xya(&stage_sys);
                    m_pServer->respond_to_client(clientSocket, res);
                    break;

                case ENABLE_ACS_STAGE:
                    res = m_pStage->enable_axes_xya(&stage_sys);
                    m_pServer->respond_to_client(clientSocket, res);
                    break;

                case COMMUTE_ACS_STAGE:
                    m_pStage->commute_axes_xya(&stage_sys, PAUSE_COMMUTE);
                    m_pStage->set_kill_decel_axes_xya(&stage_sys, 80); // Configuring Kill Deceleration with a hardcoded value for safety reasons
                    res = m_pStage->recover_stage_position_reference(&stage_sys);
                    m_pServer->respond_to_client(clientSocket, res);
                    break;

                case SET_ACCEL_ACS_STAGE:
                    acceleration = ctrl_data[3];
                    res = m_pStage->set_accel_axes_xya(&stage_sys, acceleration);
                    m_pServer->respond_to_client(clientSocket, res);
                    break;

                case SET_VEL_ACS_STAGE:
                    velocity = ctrl_data[2];
                    res = m_pStage->set_vel_axes_xya(&stage_sys, velocity);
                    m_pServer->respond_to_client(clientSocket, res);
                    break;

                case MOVE_ACS_STAGE:
                    shiftx = ctrl_data[1]; // relative shift in mm
                    shifty = ctrl_data[2]; // relative shift in mm
                    shifta = ctrl_data[3]; // relative shift in mm
                    res = m_pStage->to_point_stage(&stage_sys, shiftx, shifty, shifta);
                    m_pServer->respond_to_client(clientSocket, res);
                    break;

                case HALT_ACS_STAGE:
                    res = m_pStage->halt_stage(&stage_sys);
                    m_pServer->respond_to_client(clientSocket, res);
                    break;

                case SET_DECEL_ACS_STAGE:
                    deceleration = ctrl_data[3];
                    res = m_pStage->set_decel_axes_xya(&stage_sys, deceleration);
                    m_pServer->respond_to_client(clientSocket, res);
                    break;

                case SEND_POS_ACS_STAGE:
                    m_pStage->query_stage_pos(&stage_sys, &pos_sys);
                    x_value = pos_sys.x;
                    y_value = pos_sys.y;
                    a_value = pos_sys.a;
                    m_pServer->send_pos_to_client(clientSocket, x_value, y_value, a_value);
                    break; 

                default:
                    printf("Error, unknown msgid.\n");
                }
            }
            catch (const std::exception &e)
            {
                printf("Exception caught: %s\n", e.what());
            }
        }
    }
}
