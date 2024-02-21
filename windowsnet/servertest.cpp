#include <stdio.h>
#include <string>
#include <time.h>
#include <cstdio>
#include <cstring> // Add this line to include the cstring header
#include <iostream>

#include "serversocket.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    int port = std::stoi(argv[1]);

    Serversocket server;

    // Initialize Winsock
    WSADATA wsaData = server.initWinSock();

    // Create a socket
    SOCKET serverSocket = server.createSocket();

    // Configure the socket
    sockaddr_in serverAddr = server.configSocket(port);

    // Bind the socket
    server.bindSocket(serverSocket, serverAddr);

    // Listen for incoming connections
    server.listenCon(serverSocket);

    server.getHostInfo();

    while (true)
    {

    printf("Server listening to new client connection...\n");

        // Accept a connection
        int clientSocket = server.acceptCon(serverSocket);

        if (clientSocket == -1)
        {
            // Handle error or continue accepting
            continue;
        }

        printf("Client connected.\n");

        // Example: Use onReceive to handle received data
        std::vector<char> receivedData = server.receiveDataFromClient(clientSocket);

        if (!receivedData.empty())
        {
            // Process the received data (e.g: stage.h ACS control commands)
            // TODO | processing from hex bytes to float...
            printf("Message from client received!\n");
        }

        // Close the client socket when done
        server.closeClientSocket(clientSocket);
    }

    return 0;
}
