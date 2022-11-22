/*
 *目前是内网通信,用内网IP通信,默认端口 8888
 * 后期可改为与服务器连接，获取玩家列表并匹配
*/
#pragma once

#include "ui_AskNetworkModelDialog.h"
#include <QCloseEvent>
class AskNetworkModelDialog : public QDialog, private Ui::AskNetworkModelDialog
{
    Q_OBJECT

public:
    explicit AskNetworkModelDialog(QWidget *parent = nullptr);

private:

signals:
    void linkServer(QString ip);
private slots:
    void closeEvent(QCloseEvent *) override;
    void on_pushButtonBeginLink_pressed();
    void on_pushButtonClient_pressed();
    void on_pushButtonServer_pressed();
};

