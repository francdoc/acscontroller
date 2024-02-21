#include "stageserver.h"

int Stageserver::openConnection(int port)
{
    m_pServer = std::unique_ptr<Serversocket>(new Serversocket());
    SOCKET serverSocket = m_pServer->createSocket();
    sockaddr_in serverAddr = m_pServer->configSocket(port);
    m_pServer->bindSocket(serverSocket, serverAddr);
    m_pServer->listenCon(serverSocket);
    int clientSocket = m_pServer->acceptCon(serverSocket);
    m_pServer->getHostInfo();
    m_pServer->startReader(clientSocket);
    startProcessor();
    return clientSocket;
}

int Stageserver::stageConnectSimu()
{
    m_pStage = Stage::Stage_getInstance();
    m_pStage->stage_simulator_connect(&stage_sys);
    return 0;
}

int Stageserver::stageInit()
{
    m_pStage->clear_fault_axes_xya(&stage_sys);
    m_pStage->enable_axes_xya(&stage_sys);
    m_pStage->commute_axes_xya(&stage_sys, 4000);
    return 0;
}

void Stageserver::startProcessor()
{
    // Start a new thread for the message processing
    std::thread processorThread(&Stageserver::processQueueThread, this);

    // Detach the thread if it can run independently
    processorThread.detach();
}

void Stageserver::processQueueThread()
{
    while (true)
    {
        std::vector<float> ctrl_data = m_pServer->processQueueElement();

        if (!ctrl_data.empty())
        {
            // Print the unpacked float values
            printf("Received ctrl_data from queue -> size: %lu.\n", ctrl_data.size());

            for (size_t i = 0; i < ctrl_data.size(); ++i)
            {
                printf("ctrl_data[%lu]: %f\n", i, ctrl_data[i]);

                float target_pos = ctrl_data[0];
                float velocity = ctrl_data[1];
                float acceleration = ctrl_data[2];

                // TODO: debug accel and speed config, non responsive
                m_pStage->set_accel_axes_xya(&stage_sys, acceleration); 
                m_pStage->move_stage_smooth_mm(&stage_sys, target_pos, velocity);
            }
        }
        else
        {
            // Optionally sleep or perform other actions if no data is received
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}