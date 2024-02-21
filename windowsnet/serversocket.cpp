#include <stdio.h>
#include <string>
#include <time.h>
#include <cstdio>
#include <vector>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <unistd.h>

#include "serversocket.h"

const int MAX_MSG_LEN = 0xFFFFFF; // 16Mb - 1byte

WSADATA Serversocket::initWinSock()
{
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        fprintf(stderr, "WSAStartup failed\n");
        exit(EXIT_FAILURE);
    }
    return wsaData;
}

// Function to get host name and IP address
bool Serversocket::getHostInfo()
{
    char hostName[256];

    // Get the host name
    if (gethostname(hostName, sizeof(hostName)) == 0)
    {
        printf("Host name: %s\n", hostName);

        // Get host information
        hostent *hostInfo = gethostbyname(hostName);

        if (hostInfo != nullptr)
        {
            // Get the first IP address from the list
            in_addr *hostAddress = reinterpret_cast<in_addr *>(hostInfo->h_addr_list[0]);

            if (hostAddress != nullptr)
            {
                // Print the IP address
                printf("Host IP address: %s\n", inet_ntoa(*hostAddress));
                return true; // Successfully obtained host information
            }
            else
            {
                printf("Failed to get host IP address.\n");
            }
        }
        else
        {
            printf("Failed to get host information.\n");
        }
    }
    else
    {
        printf("Failed to get host name.\n");
    }

    return false; // Failed to obtain host information
}

SOCKET Serversocket::createSocket()
{
    // Create a socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        fprintf(stderr, "Socket creation failed\n");
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    return serverSocket;
}

sockaddr_in Serversocket::configSocket(int port)
{
    // Bind the socket
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    return serverAddr;
}

int Serversocket::bindSocket(SOCKET serversocket, sockaddr_in serveraddr)
{
    if (bind(serversocket, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR)
    {
        fprintf(stderr, "Bind failed.\n");
        closesocket(serversocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    return 0;
}

int Serversocket::listenCon(SOCKET serversocket)
{
    printf("Server listening to new client connection...\n");
    // Listen for incoming connections
    if (listen(serversocket, SOMAXCONN) == SOCKET_ERROR)
    {
        fprintf(stderr, "Listen failed.\n");
        closesocket(serversocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    return 0;
}

SOCKET Serversocket::acceptCon(SOCKET serversocket)
{
    // Accept a connection
    SOCKET clientSocket = accept(serversocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET)
    {
        fprintf(stderr, "Accept failed.\n");
        closesocket(serversocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    printf("Client connected!\n");
    return clientSocket; // Return the accepted client socket
}

int Serversocket::closeSocket(SOCKET clientsocket, SOCKET serversocket)
{
    // Cleanup
    closesocket(clientsocket);
    closesocket(serversocket);
    WSACleanup();
    return 0;
}

int Serversocket::closeClientSocket(SOCKET clientsocket)
{
    closesocket(clientsocket);
    return 0;
}

int Serversocket::receive(int clientsocket, char *buf, size_t sz)
{
    if (sz <= 0)
        return 0;

    // Receive data from the client
    int bytesRead = recv(clientsocket, buf, sz, 0);

    if (bytesRead == -1)
    {
        perror("Receive failed");
        close(clientsocket);
        exit(EXIT_FAILURE);
    }
    else if (bytesRead <= 0)
    {
        // Connection closed by the client
        close(clientsocket);
        exit(EXIT_FAILURE);
    }

    // Print the value of bytesRead
    printf("Bytes read: %d\n", bytesRead);

    // Data received successfully
    return bytesRead;
}

float Serversocket::unpackFloat(const uint8_t *bytes, int offset)
{
    /*Unpacks a 4 byte big-endian float variable.*/
    uint32_t value = (bytes[offset] << 24) | (bytes[offset + 1] << 16) | (bytes[offset + 2] << 8) | bytes[offset + 3];
    return *((float *)&value);
}

void Serversocket::printHex(const std::vector<char> &data, int nres)
{
    printf("Hex data: ");
    for (ssize_t i = 0; i < nres; i++)
    {
        printf("%02X ", (unsigned char)data[i]);
    }
    printf("\n");
}

std::vector<char> Serversocket::receiveDataFromClient(int clientsocket)
{
    bufferContainer.m_buf.clear(); // Clear the buffer for a new connection

    int nOffset = bufferContainer.m_buf.size();
    bufferContainer.m_buf.resize(bufferContainer.m_nMaxBufSize);

    int nRes = receive(clientsocket, bufferContainer.m_buf.data() + nOffset, bufferContainer.m_buf.size() - nOffset);

    if (nRes <= 0)
        return std::vector<char>();

    printf("Received message from socket\n.");

    printHex(bufferContainer.m_buf, nRes);

    // Use a loop to unpack float values dynamically
    for (int i = 0; i < nRes / sizeof(float); ++i)
    {
        float floatValue = unpackFloat(reinterpret_cast<const uint8_t *>(&bufferContainer.m_buf[0]), i * sizeof(float));
        printf("Unpacked float %d: %f\n", i + 1, floatValue);
    }

    return bufferContainer.m_buf;
}

void Serversocket::printReceivedData(const std::vector<char> &data, int nres)
{
    printf("Hex data: ");
    for (ssize_t i = 0; i < nres; i++)
    {
        printf("%02X ", (unsigned char)data[i]);
    }
    printf("\n");
}

void Serversocket::unpackFloatValues(const std::vector<char> &data, int nres)
{
    // Use a loop to unpack float values dynamically
    for (int i = 0; i < nres / sizeof(float); ++i)
    {
        float floatValue = unpackFloat(reinterpret_cast<const uint8_t *>(&data[0]), i * sizeof(float));
        printf("Unpacked float %d: %f\n", i + 1, floatValue);
    }
}

void Serversocket::startReader(int clientSocket)
{
    // Start a new thread for the reader
    std::thread readerThread(&Serversocket::readToQueueThread, this, this, clientSocket);

    // Detach the thread if it can run independently
    readerThread.detach();
}

void Serversocket::readToQueueThread(Serversocket *server, int clientSocket)
{
    while (true)
    {
        std::vector<char> data = server->receiveDataFromClient(clientSocket);

        if (data.empty())
        {
            // Optionally sleep or perform other actions if no data is received
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        else
        {
            // Lock to ensure thread safety when modifying the dataQueue
            std::lock_guard<std::mutex> lock(server->dataQueueMutex);
            server->dataQueue.push(data);
        }
    }
}

std::vector<float> Serversocket::processQueueElement()
{
    // Lock to ensure thread safety when accessing the dataQueue
    std::lock_guard<std::mutex> lock(dataQueueMutex);

    if (!dataQueue.empty())
    {
        // Get the front message from the queue
        std::vector<char> message = dataQueue.front();
        dataQueue.pop(); // Remove the front message from the queue

        printf("Received message from queue -> size: %lu.\n", message.size());

        // Unpack float values and store them in a vector
        std::vector<float> floatValues;

        for (int i = 0; i < message.size() / sizeof(float); ++i)
        {
            float floatValue = unpackFloat(reinterpret_cast<const uint8_t *>(&message[0]), i * sizeof(float));
            #ifdef DEBUG_PROCESS_QUEUE_ELEMENT
                printf("Unpacked float %d: %f\n", i + 1, floatValue);
            #endif
            floatValues.push_back(floatValue);
        }

        return floatValues;
    }

    // Return an empty vector if the queue is empty
    return std::vector<float>();
}