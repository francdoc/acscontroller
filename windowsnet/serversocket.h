#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#include <cstdint>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>

#define IN_BUF_SIZE_DEFAULT 32 // bytes

class Serversocket
{
public:
    enum connstate
    {
        CS_DISCONNECTED,
        CS_CONNECTING,
        CS_CONNECTED,
        CS_REDIRECT
    };

    class BufferContainer
    {
    public:
        std::vector<char> m_buf;
        unsigned int m_nMaxBufSize = IN_BUF_SIZE_DEFAULT;
    };

    BufferContainer bufferContainer;         // Encapsulate m_buf in a structure
    std::queue<std::vector<char>> dataQueue; // Queue to store received data
    HANDLE m_hReadThread;                    // Thread handle for the reader thread
    std::mutex dataQueueMutex;               // Add this mutex to the class for thread safety

    WSADATA initWinSock();
    SOCKET createSocket();
    bool getHostInfo();
    sockaddr_in configSocket(int port);
    int bindSocket(SOCKET serversocket, sockaddr_in serveraddr);
    int listenCon(SOCKET serversocket);
    SOCKET acceptCon(SOCKET serversocket);
    int closeSocket(SOCKET clientsocket, SOCKET serversocket);
    int closeClientSocket(SOCKET clientsocket);
    int receive(int clientsocket, char *buf, size_t sz);
    int respond_to_client(int clientsocket, int res);
    std::vector<char> receiveDataFromClient(int clientsocket);
    void printReceivedData(const std::vector<char> &data, int nres);
    void unpackDoubleValues(const std::vector<char> &data, int nres);
    double unpackDouble(const uint8_t *bytes, int offset);
    void printHex(const std::vector<char> &data, int nres);
    void startReader(int clientSocket);
    void readToQueueThread(Serversocket *server, int clientSocket);
    std::vector<double> processQueueElement();
    int send_pos_to_client(int clientsocket, double x_value, double y_value, double a_value);
};