#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include "networker.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class ChatWidget;
}
QT_END_NAMESPACE

class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWidget(QWidget *parent = nullptr);
    ~ChatWidget() override;

private slots:
    void on_send_btn_clicked();
    void onConnected();
    void onDisconnected();
    void onMessageReceived(const QString &msg);
    void onError(const QString &error);

private:
    Ui::ChatWidget *ui;
    NetWorker* networker;
};
#endif // CHATWIDGET_H
