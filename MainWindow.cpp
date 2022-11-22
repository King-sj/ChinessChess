#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("中国象棋");

    board = NULL;
    dialog = new AskNetworkModelDialog(this);



}

MainWindow::~MainWindow()
{
    delete board;
    board = NULL;
    delete ui;
}


void MainWindow::on_pushButton_board_pressed()
{
    if(board != NULL ){
        delete board;
    }
   board = new Board(this);
   connect(board,&Board::gameover,this,&MainWindow::dealGameover);
   this->hide();
   board->show();
}

void MainWindow::on_pushButton_ai_pressed()
{
    try {
        if(board != NULL ){
            delete board;
        }
       board = new AIGame(this);
       connect(board,&Net::gameover,this,&MainWindow::dealGameover);
       this->hide();
       board->show();

    }catch(QString e){
        cout<<e<<Qt::endl;
    }
    catch (...) {
        cout<<"error"<<Qt::endl;
    }

}

void MainWindow::on_pushButton_net_pressed()
{
    if(board != NULL ){
        //static_cast<Net*>(board)->socket->close();
        delete board;
    }
    if(dialog != NULL)
    {
        delete dialog;
    }
    dialog = new AskNetworkModelDialog(this);
    connect(dialog,SIGNAL(linkServer(QString)),this,SLOT(dealNetGame(QString)));
    dialog->exec();//模式对话
}
void MainWindow::dealGameover()
{
    board->hide();
    this->show();
//    if(board != NULL)
//        delete board;
//    board = NULL;
}

void MainWindow::dealNetGame(QString ip)
{
    //cout<<ip<<Qt::endl;
    if(ip == "-1")//服务端
    {
        //todo：完善好，连接成功则关闭dialog
        board = new Net(this);
        connect(board,&Net::gameover,this,&MainWindow::dealGameover);
        //此处类型转换都是为了多态
        connect(static_cast<Net*>(board),&Net::linked,this,&MainWindow::dealLinked);

    }else{//客户端
        board = new Net(this,0,ip);
        connect(board,&Net::gameover,this,&MainWindow::dealGameover);
        connect(static_cast<Net*>(board),&Net::linked,this,&MainWindow::dealLinked);
    }
}

void MainWindow::dealLinked()
{
    this->dialog->close();
    this->hide();
}
