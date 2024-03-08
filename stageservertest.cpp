#include <stdio.h>
#include <string>
#include <time.h>
#include <cstdio>
#include <queue>
#include <iostream>
#include <cstring>

#include "stageserver.h"

int main()
{
    Stageserver stageserver;
    int clientSocket = stageserver.openConnection(SERVER_PORT);
    while (true)
    {
    }
    return 0;
}
