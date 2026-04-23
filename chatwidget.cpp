#include "chatwidget.h"
#include "./ui_ChatWidget.h"

ChatWidget::ChatWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatWidget)
{
    ui->setupUi(this);
    setWindowTitle("聊天客户端");

    networker = new NetWorker;

    //连接信号与槽
    connect(networker,&NetWorker::connected,this,&ChatWidget::onConnected);
    connect(networker,&NetWorker::disconnected,this,&ChatWidget::onDisconnected);
    connect(networker,&NetWorker::messageReceived,this,&ChatWidget::onMessageReceived);
    connect(networker,&NetWorker::errorOccurred,this,&ChatWidget::onError);

    //连接发送按钮
    connect(ui->send_btn,&QPushButton::clicked,this,&ChatWidget::on_send_btn_clicked);

    networker->Start();
    ui->input_lineE->setFocus();
}

ChatWidget::~ChatWidget()
{
    networker->Stop();
    delete ui;
}


void ChatWidget::on_send_btn_clicked()
{
    QString qmessage = ui->input_lineE->text();
    if(qmessage.trimmed().isEmpty())
    {
        return;
    }

    // 将消息显示在界面上
    ui->display_textE->append("我：" + qmessage);

    // 将消息发送给服务器（QString 转 std::string）
    std::string message = qmessage.toStdString();
    networker->SendMessageW(message);

    ui->input_lineE->clear();
    ui->input_lineE->setFocus();
}

void ChatWidget::onConnected()
{
    ui->display_textE->append("系统：已连接到服务器");
}

void ChatWidget::onDisconnected()
{
    ui->display_textE->append("系统：与服务器断开连接");
}

void ChatWidget::onMessageReceived(const QString &msg)
{
    ui->display_textE->append("服务器：" + msg);
}

void ChatWidget::onError(const QString &error)
{
    ui->display_textE->append("错误：" + error);
}