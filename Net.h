#ifndef NET_H
#define NET_H

#include <QMainWindow>
#include <QObject>
#include"Board.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QRandomGenerator>
#include <QMessageBox>
class Net : public Board
{
    Q_OBJECT
public:
    Net() = delete;
    Net(QMainWindow* parent,bool isSever = 1,QString ip = "");
    ~Net();

private:
    void init(Color c);
    virtual void mouseReleaseEvent(QMouseEvent* e)override;//鼠标事件
    virtual void on_action_giveUp_triggered()override;
    virtual void on_action_Undo_triggered()override;
private:
    QTcpServer* server;
    QTcpSocket* socket;
    const quint16 port = 8888;//默认端口
    Color selfColor = Color::None;
    char command[3];/*
                    * command[0] -> r ->red , b -> black , o -> operation用于传递谁先手，以及点击哪里的头 , g -> giveup , u -> undo
                    * command[1] - > row;
                    * command[2] -> col;
                    */
private  slots:
    virtual void dealGamever();
    virtual void dealNewConnection();
    virtual void dealDisconnected();
    virtual void dealRecv();
    virtual void dealConnected();
signals:
    void linked();
};
#endif //Net.h
