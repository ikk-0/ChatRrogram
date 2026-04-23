#ifndef CLIENT_H
#define CLIENT_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#pragma comment(lib, "ws2_32.lib")

class Client
{
public:
    Client();
    ~Client();

    bool Init();
    bool Connect();
    void DisConnect();
    bool IsConnected() const;

    int Send(const char* data,int len);
    int Recv(char* buffer, int buffer_size);

    SOCKET GetSocket();

private:
    std::string ip_;
    int port_;
    SOCKET connect_fd;
};

#endif // CLIENT_H
