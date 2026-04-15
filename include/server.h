#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include <signal.h>

using namespace std;

class TcpServer
{
private:
    int port_;
    string ip_;
    int listen_fd;  //sfd
    int epfd_;
    bool is_running;
    
public:
    TcpServer(const string &ip, int port);
    ~TcpServer();

    bool Init();
    bool StartListen();
    bool Run();
    void Stop();

private:
    int CreateSocket();
    bool BindInfo();
    bool HandleClientInfo(int fd);
    bool HandleNewConnection();
    void RemoveClient(int fd); 
};

#endif