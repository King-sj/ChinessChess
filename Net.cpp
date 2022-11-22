#include "Net.h"

Net::Net(QMainWindow *parent, bool isSever,QString ip):Board(parent)
{
    socket = NULL;
    server = NULL;
    connect(this,&Net::gameover,this,&Net::dealGamever);

    command[0]=(command[1] =( command[2] = 0));
    this->setWindowTitle("网络对战");
    if(isSever)//服务端
    {
        this->setWindowTitle("server");
        server = new QTcpServer(this);
        server->listen(QHostAddress::Any,port);
        connect(server,&QTcpServer::newConnection,this,&Net::dealNewConnection);
    }else{//客户端
        cout<<QHostAddress(ip)<<Qt::endl;
        this->setWindowTitle("client");
        socket = new QTcpSocket(this);
        socket->connectToHost(QHostAddress(ip),port);
        connect(socket,&QTcpSocket::readyRead,this,&Net::dealRecv);
        connect(socket,&QTcpSocket::connected,this,&Net::dealConnected);
        connect(socket,&QTcpSocket::disconnected,this,&Net::dealDisconnected);
        if(!socket->waitForConnected())//默认30s
        {
            //todo:连接超时
            cout<<"连接超时";
            emit gameover();
        }
    }

}

Net::~Net()
{
    if(socket != NULL){
        socket->close();
        delete socket;
        socket = NULL;
    }
    if(server != NULL)
    {
        delete server;
        server = NULL;
    }
}

void Net::init(Color c)
{
    if(c == Color::Black)
    {
        for(int i = 0 ; i < _stoneNum ; i++)
        {
            //中心对称变换
            redStone[i]->row = _rowSize - redStone[i]->row;
            redStone[i]->col = _colSize - redStone[i]->col;
            redStone[i]->direction = -redStone[i]->direction;//方向也要变

            blackStone[i]->row = _rowSize - blackStone[i]->row;
            blackStone[i]->col = _colSize - blackStone[i]->col;
            blackStone[i]->direction = -blackStone[i]->direction;//方向也要变
        }

    }
}

void Net::mouseReleaseEvent(QMouseEvent *e)
{
//    cout<<(selfColor == Color::Red ? 'r' : 'b')<<(gameInfo->turn == TURN::Red ? 'r' : 'b')<<Qt::endl;
//    cout<<e->pos()<<Qt::endl;
//    cout<<rcTransfromPoint(e->pos().x(),e->pos().y()).row<<" "<<rcTransfromPoint(e->pos().x(),e->pos().y()).row<<Qt::endl;
    if(selfColor != TurnToColor(gameInfo->turn))
    {
            return;//不允许你点
    }
    Board::mouseReleaseEvent(e);

    command[0] = 'o';//operation
    //不必转换
    command[1] = _rowSize-rcTransfromPoint(e->pos().x(),e->pos().y()).row;//row
    command[2] = _colSize-rcTransfromPoint(e->pos().x(),e->pos().y()).col;//col
    socket->write(command,3);
}

void Net::on_action_giveUp_triggered()
{
    command[0] = 'g';//give
    command[1] = 'a';//ask
    socket->write(command,3);
}

void Net::on_action_Undo_triggered()
{
    command[0] = 'u';//undo
    command[1] = 'a';//ask
    socket->write(command,3);
}

void Net::dealGamever()
{
    if(socket != NULL)
        socket->close();
}




void Net::dealNewConnection()
{
    if(socket != NULL)return;
    emit linked();
    this->show();
    update();

    socket = server->nextPendingConnection();//下一位
    connect(socket,&QTcpSocket::readyRead,this,&Net::dealRecv);
    connect(socket,&QTcpSocket::connected,this,&Net::dealConnected);
    connect(socket,&QTcpSocket::disconnected,this,&Net::dealDisconnected);

    int t = QRandomGenerator::global()->bounded(0,100);
    if( t%2 == 0){//玄学
        command[0] = 'r';//red
        selfColor = Color::Black;
        init(selfColor);
    }
    else {
        command[0] = 'b';//black
        selfColor = Color::Red;
        init(selfColor);
    }
    socket->write(command,3);
}

void Net::dealDisconnected()
{
    QMessageBox msb(this);
    msb.setText("对方掉线");
    msb.exec();
    emit gameover();
}

void Net::dealRecv()
{
    auto buffer = socket->read(3);
    command[0] = buffer[0];
    command[1] = buffer[1];
    command[2] = buffer[2];
    cout<<command[0]<<" "<<command[1]<<" "<<command[2]<<Qt::endl;
    switch (command[0]) {
    case 'o'://operation
        clickOn(command[1],command[2]);//因为传递的时候没转换为字符，所以直接用
        update();
        break;
    case 'r'://red
        emit linked();
        this->show();
        update();
        selfColor = Color::Red;
        init(selfColor);
        break;
    case 'b'://black
        emit linked();
        this->show();
        update();
        selfColor = Color::Black;
        init(selfColor);
        break;
    case 'u'://undo
        if(command[1] == 'a')
        {
            QMessageBox msb(this);
            msb.setText("对方脑阔抽筋了");
            msb.setInformativeText("是否让他悔棋");
            msb.setStandardButtons(QMessageBox::Ok|QMessageBox::No);
            //msb.setDefaultButton(QMessageBox::No);
            auto res = msb.exec();
            if(res == QMessageBox::Ok)
            {
                if((TurnToColor(gameInfo->turn) != selfColor ))backoutStone();
                backoutStone();
                command[0] = 'u';//undo
                command[1] = 'y';//yes
            }else{
                command[0] = 'u';//undo
                command[1] = 'n';//no
            }
            socket->write(command,3);
        }else if(command[1] == 'y')
        {
            QMessageBox msb(this);
            msb.setText("对方原谅了你的大意");
            msb.exec();
            if((TurnToColor(gameInfo->turn) == selfColor ))backoutStone();
            backoutStone();
        }else{
            QMessageBox msb(this);
            msb.setText("落子不悔，年轻人要讲武德");
            msb.exec();
        }
        break;
    case 'g'://give up
        if(command[1] == 'a')//ask
        {
            QMessageBox msb(this);
            msb.setText("对方请求投降");
            msb.setInformativeText("是否同意");
            msb.setStandardButtons(QMessageBox::Ok|QMessageBox::No);
            //msb.setDefaultButton(QMessageBox::No);
            auto res = msb.exec();
            if( res  == QMessageBox::Ok)
            {
                command[0] = 'g';//give up
                command[1] = 'y';//yes
                socket->write(command,3);
                emit gameover();
            }else if(res == QMessageBox::No)
            {
                command[0] = 'g';//give up
                command[1] = 'n';//no
                socket->write(command,3);
            }

        }else if(command[1] == 'y')
        {
            QMessageBox msb(this);
            msb.setText("对方原谅了你的弱小");
            msb.exec();
            emit gameover();
        }else if(command[1] == 'n'){
            QMessageBox msb(this);
            msb.setText("对方拒绝并让你不要放弃希望");
            msb.exec();
        }
        break;
    default:
        cout<<"error message: "<<command[0]<<command[1]<<command[2]<<Qt::endl;
        break;
    }
}

void Net::dealConnected()
{
    //似乎 connected 不是 双端都触发
//    this->setWindowTitle("连接成功");
//    emit linked();
//    this->show();
}
