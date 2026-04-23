#ifndef NETWORKER_H
#define NETWORKER_H

#include "client.h"
#include <QObject>
#include <pthread.h>
#include <string.h>

class NetWorker : public QObject
{
    Q_OBJECT
public:
    explicit NetWorker(QObject *parent = nullptr);
    ~NetWorker();

    void Start();
    void Stop();
    void SendMessage(const std::string &msg);

signals:
    void connected();                              // 连接成功
    void disconnected();                           // 断开连接
    void messageReceived(const QString &msg);      // 收到消息
    void errorOccurred(const QString &error);      // 错误

private:
    static void* ThreadFunc(void* arg);     // 线程入口函数
    void Run();     // 线程主进程

    pthread_t tid;
    Client* client;
    bool is_running;
    bool should_stop;

};

#endif // NETWORKER_H
