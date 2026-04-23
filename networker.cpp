#include "networker.h"
#include <QDebug>
#include <QCoreApplication>
#include <qthread.h>

NetWorker::NetWorker(QObject *parent)
    :QObject(parent),is_running(false),should_stop(false),client(nullptr)
{}

NetWorker::~NetWorker()
{
    Stop();
    if (tid) {
        pthread_join(tid, NULL);  // 等待线程结束
    }
    if(client)
    {
        delete client;
    }
}

void NetWorker::Start()
{
    client = new Client();

    if(pthread_create(&tid,NULL,ThreadFunc,this) != 0)
    {
        emit errorOccurred("创建线程失败");
        return;
    }
}

void NetWorker::Stop()
{
    is_running = false;
    should_stop = true;
}

void NetWorker::Run()
{
    //初始化并连接
    if(!client->Init())
    {
        emit errorOccurred("初始化失败");
        return;
    }
    if(!client->Connect())
    {
        emit errorOccurred("连接失败");
        return;
    }
    emit connected();
    is_running = true;

    char buffer[4096];
    while(is_running && !should_stop)
    {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(client->GetSocket(),&fds);
        struct timeval tv = {0,100};       // 100ms超时

        // 使用 select 实现超时接收
        int ret = select(client->GetSocket()+1,&fds,NULL,NULL,&tv);
        if(ret > 0)
        {
            int res = client->Recv(buffer,sizeof(buffer));
            if(res>0)
            {
                buffer[res] = '\0';// 给字符串加结束标志
                emit messageReceived(QString::fromUtf8(buffer));  // 发信号给界面
            }else if(res == 0)// 对端关闭连接
            {
                emit disconnected();
                break;
            }else
            {
                int err = WSAGetLastError();
                if(err != WSAEWOULDBLOCK)   //WSAEWOULDBLOCK 无数据可读，忽略
                {
                    emit errorOccurred("接受数据失败");
                    break;
                }
            }
        }
        // 让出 CPU，避免忙等
        QThread::msleep(10);
    }
    // 清理
    client->DisConnect();
    emit disconnected();
}

void NetWorker::SendMessage(const std::string &msg)
{
    if(client&&client->IsConnected())
    {
        client->Send(msg.c_str(),msg.size());
    }
}

void* NetWorker::ThreadFunc(void* arg)
{
    NetWorker* work = static_cast<NetWorker*>(arg);
    work->Run();
    return NULL;
}