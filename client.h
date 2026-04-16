#ifndef CLIENT_H
#define CLIENT_H

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") // 链接 Winsock 库
#include <string>
#include <iostream>
using namespace std;

class TcpClient
{
private:
    SOCKET connect_fd;
    int port_;
    string ip_;
    bool is_running;

public:
    TcpClient(const string &ip,int port);
    ~TcpClient();

    bool Init();
    bool ConnectStart();
    void Run();
    void Stop();

private:
    SOCKET SocketCreate();
    bool Connect();
    bool HandleData();
};


#endif // CLIENT_H
