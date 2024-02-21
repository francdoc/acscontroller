#include <memory>
#include <vector>
#include <iostream>

#include "windowsnet/serversocket.h"
#include "stage.h"

class Stage;

class Stageserver
{
public:
    Stage::Stage_system_t stage_sys;

    int openConnection(int port);
    int stageConnectSimu();
    int stageInit();
    void startProcessor();
    void processQueueThread();
    // Constructor
    Stageserver() : m_pServer(std::make_unique<Serversocket>()){};

    Stage *m_pStage;

private:
    std::unique_ptr<Serversocket> m_pServer;
};