#include "server.h"

TcpServer::TcpServer(const string &ip, int port)
    : ip_(ip), port_(port), listen_fd(-1), epfd_(-1), is_running(false)
{
    cout << "TcpServer created" << endl;
}

TcpServer::~TcpServer()
{
    if (listen_fd != -1)
    {
        close(listen_fd);
        cout << "listen_fd closed" << endl;
    }
    if (epfd_ != -1)
    {
        close(epfd_);
        cout << "epfd_ closed" << endl;
    }
    cout << "TcpServer destroyed!" << endl;
}

int TcpServer::CreateSocket()
{
    // 创建套接字sfd
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
    {
        cerr << "socket error" << endl;
        return -1;
    }
    cout << "socket success! sfd = " << sfd << endl;
    return sfd;
}

bool TcpServer::BindInfo()
{
    /****************************************************/
    // 添加地址复用选项
    int opt = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        cerr << "setsockopt error" << endl;
        // 不是致命错误，继续执行
    }
    /*当你的服务器程序异常退出（比如用 Ctrl+C 强制结束）时，操作系统会将端口锁定一段时间，
    通常为 2-4 分钟(防止旧连接的数据包干扰新连接)。在这段时间内，任何程序（包括重新启动的服务器）
    都无法绑定这个端口，导致 bind() 失败。*/
    /****************************************************/

    // 创建地址信息结构体
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_addr.s_addr = inet_addr(ip_.c_str()); // c_str() 获取 C 风格字符串
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port_);
    // 绑定ip和port
    if (bind(listen_fd, (struct sockaddr *)&sin, sizeof(sin)) == -1)
    {
        cerr << "bind error" << endl;
        return false;
    }
    cout << "bind success!" << endl;
    return true;
}

bool TcpServer::Init()
{
    listen_fd = CreateSocket();
    if (listen_fd == -1)
        return false;
    if (!BindInfo())
        return false;
    cout << "server information : " << "[" << ip_ << ":" << port_ << "]" << endl;
    return true;
}

bool TcpServer::StartListen()
{
    // 监听客户端连接
    if (listen(listen_fd, 128) == -1)
    {
        cerr << "listen error" << endl;
        return false;
    }
    cout << "listen success!" << endl;
    return true;
}

bool TcpServer::HandleClientInfo(int fd)
{
    // 处理集合中所有客户端的通讯，通过 for（i）区分
    char rbuf[128] = "";
    int res = recv(fd, rbuf, sizeof(rbuf), 0);
    // 错误处理
    if (res <= 0)
    {
        if (res == 0)
            cerr << "对端已下线,客户端 [fd=" << fd << "] 正常断开" << endl;
        else
            cerr << "recv error [fd=" << fd << "]" << endl;
        RemoveClient(fd);
        return false;
    }
    rbuf[res] = '\0';
    cout << "收到数据 : " << rbuf << endl;
    // 处理数据
    strcat(rbuf, "*_*");
    // 回传给客户端
    if (send(fd, rbuf, strlen(rbuf), 0) == -1)
    {
        cerr << "send error [fd=" << fd << "]" << endl;
        RemoveClient(fd);
        return false;
    }
    cout << "send success" << endl;
    return true;
}

bool TcpServer::HandleNewConnection()
{
    struct sockaddr_in cin;
    socklen_t c_socklen = sizeof(cin);
    // 与客户端建立连接
    int newfd = accept(listen_fd, (struct sockaddr *)&cin, &c_socklen);
    if (newfd == -1)
    {
        cerr << "accept error" << endl;
        return false;
    }
    // 打印客户端IP和端口
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &cin.sin_addr, ip, sizeof(ip));
    cout << "新客户端连接: " << ip << ":" << ntohs(cin.sin_port)
         << " [fd=" << newfd << "]" << endl;

    // 将客户端文件描述符添加到集合中
    epoll_event ev;
    ev.data.fd = newfd;
    ev.events = EPOLLIN;
    epoll_ctl(epfd_, EPOLL_CTL_ADD, newfd, &ev);
    return true;
}

void TcpServer::RemoveClient(int fd)
{
    epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
    cout << "移除客户端 [fd=" << fd << "]" << endl;
}

void TcpServer::Stop()
{
    is_running = false;
    cout << "Server stopping..." << endl;
}

bool TcpServer::Run()
{
    // 创建epoll实例
    epfd_ = epoll_create(1);
    if (epfd_ == -1)
    {
        cerr << "epoll_create error" << endl;
        return false;
    }
    epoll_event ev;
    ev.data.fd = listen_fd;
    ev.events = EPOLLIN;
    epoll_ctl(epfd_, EPOLL_CTL_ADD, listen_fd, &ev);

    // 创建处理事件集合
    epoll_event evs[1024];
    int evs_size = sizeof(evs) / sizeof(evs[0]);

    cout << "Server running... waiting for events" << endl;

    // 循环 连接 通讯
    is_running = true;
    while (is_running)
    {
        int num = epoll_wait(epfd_, evs, evs_size, -1);
        if (num == -1)
        {
            if (errno == EINTR)
                continue; // 被信号中断，继续
            cerr << "epoll_wait error: " << strerror(errno) << endl;
            break;
        }
        cout << "本次触发的事件个数 num = " << num << endl;

        for (int i = 0; i < num; i++)
        {
            // 可能是sfd,也可能是newfd
            int fd = evs[i].data.fd; // 不需要判断 0、1、2，因为根本没添加到 epoll 中
            if (fd == listen_fd)
            {
                HandleNewConnection();
                continue;
            }
            else
            {
                HandleClientInfo(fd);
            }
        }
    }
    close(epfd_);
    return true;
}