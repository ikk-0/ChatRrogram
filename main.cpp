//#include "ClientWidget.h"
#include <QApplication>
#include "client.h"

#define SER_PORT 8888            // 服务器端口号
#define SER_IP "192.168.233.129" // 服务器IP地址

int main(int argc, char *argv[])
{
    // QApplication a(argc, argv);
    // Widget w;
    // w.show();

    TcpClient cli(SER_IP,SER_PORT);
    if (!cli.Init()) {
        cerr << "Init failed" << endl;
        return -1;
    }

    if (!cli.ConnectStart()) {
        cerr << "Connect failed" << endl;
        return -1;
    }
    cli.Run();
    //return QCoreApplication::exec();
    return 0;
}
