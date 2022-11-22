#include "AskNetworkModelDialog.h"

AskNetworkModelDialog::AskNetworkModelDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    this->setWindowFlag(Qt::Dialog);
    this->setFixedSize(this->width(),this->height());

    this->setWindowTitle("网络匹配");
    this->stackedWidget->setVisible(false);
    this->pushButtonBeginLink->setVisible(false);//note
}

void AskNetworkModelDialog::closeEvent(QCloseEvent *e)
{
    e->accept();
}

void AskNetworkModelDialog::on_pushButtonBeginLink_pressed()
{
    this->pushButtonBeginLink->setEnabled(false);//不准在点了
    QString res = this->lineEdit_IP->text();
    emit linkServer(res);
}

void AskNetworkModelDialog::on_pushButtonClient_pressed()
{
    this->stackedWidget->setCurrentIndex(1);
    this->stackedWidget->setVisible(true);
    this->pushButtonClient->hide();
    this->pushButtonServer->hide();
    this->pushButtonBeginLink->setVisible(true);
}

void AskNetworkModelDialog::on_pushButtonServer_pressed()
{
    this->stackedWidget->setCurrentIndex(0);
    this->stackedWidget->setVisible(true);
    this->pushButtonClient->hide();
    this->pushButtonServer->hide();
    emit linkServer("-1");//作为服务器
}
