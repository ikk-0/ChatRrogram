#include "client.h"

TcpClient::TcpClient(const string &ip,int port)
    :port_(port),ip_(ip),connect_fd(INVALID_SOCKET),is_running(false)
{
    // 设置控制台编码为 UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    cout << "TcpClient created" << endl;
}
TcpClient::~TcpClient()
{
    Stop();
    cout << "TcpServer destroyed!" << endl;
}

SOCKET TcpClient::SocketCreate()
{
    SOCKET fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd == INVALID_SOCKET)
    {
        return -1;
    }
    cout << "socket success! fd = " << fd << endl;
    return fd;
}

bool TcpClient::Connect()
{
    if (connect_fd == INVALID_SOCKET) {
        cerr << "Error: socket is invalid before connect" << endl;
        return false;
    }

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));//清空结构体，防止垃圾数据

    sin.sin_family = AF_INET;
    sin.sin_port = htons(port_);
    sin.sin_addr.s_addr = inet_addr(ip_.c_str());
    cout << "Connecting to " << ip_ << " : " << port_ << endl;
    cout << "sin_addr = " << inet_ntoa(sin.sin_addr) << endl;

    if(connect(connect_fd,(struct sockaddr*)&sin,sizeof(sin)) == SOCKET_ERROR)
    {
        int err = WSAGetLastError();
        cerr << "connect error: " << err << endl;

        // 错误码解释
        if (err == 10061)
            cerr << "  原因: 连接被拒绝，服务器没有监听" << endl;
        else if (err == 10060)
            cerr << "  原因: 连接超时" << endl;
        else if (err == 10047)
            cerr << "  原因: 地址族不匹配" << endl;
        else if (err == 0)
            cerr << "  原因: 没有错误，但连接失败？" << endl;
        return false;
    }
    return true;
}

bool TcpClient::Init()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup error" << endl;
        return false;
    }

    connect_fd = SocketCreate();
    if(connect_fd == INVALID_SOCKET)
    {
        cerr<<"socket error"<<endl;
        return false;
    }
    return true;
}

bool TcpClient::ConnectStart()
{
    if(!Connect())
    {
        cerr<<"ConnectStart error"<<endl;
        return false;
    }
    return true;
}

bool TcpClient::HandleData()
{
    char wbuf[128] = "";
    memset(wbuf,0,sizeof(wbuf));

    cout << "请输入消息: ";
    fgets(wbuf,sizeof(wbuf),stdin);

    //去掉换行符
    size_t len = strlen(wbuf);
    if (len > 0 && wbuf[len-1] == '\n') {
        wbuf[len-1] = '\0';
    }

    if(send(connect_fd,wbuf,strlen(wbuf),0) == SOCKET_ERROR)
    {
        cerr << "send error: " << WSAGetLastError() << endl;
        return false;
    }

    int res = recv(connect_fd,wbuf,sizeof(wbuf),0);//接收服务器返回的数据
    if(res == SOCKET_ERROR)
    {
        cerr << "recv error: " << WSAGetLastError() << endl;
        return false;
    }
    else if (res == 0) {
        cout << "服务器已断开连接" << endl;
        return false;
    }

    cout << "服务器回复: " << wbuf << endl;
    return true;
}

void TcpClient::Stop()
{
    is_running = false;
    if(connect_fd != INVALID_SOCKET)
    {
        closesocket(connect_fd);
        cout<<"connect_fd closed"<<endl;
    }
    WSACleanup();
}

void TcpClient::Run()
{
    is_running = true;
    while (is_running) {
        if (!HandleData()) {
            break;
        }
    }
}
