/*
 * BUG:
 * 1.小兵莫名消失
 * 2.小兵前进造成的将军未触发
*/
#include "Board.h"
#include "ui_Board.h"

Board::Board(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Board)
{
    init();
    ui->setupUi(this);
    this->setWindowTitle("双人对战");
    this->setWindowFlag(Qt::Tool,true);
    pix = new QPixmap(this->_windowWidth,this->_windowHeight);
    //note:否则开始直接吃会creash
    step.append(Step());
}

Board::~Board()
{
    delete pix;
    pix = NULL;
    for(int i = 0 ; i < _stoneNum ; i++)
    {
        delete blackStone[i];
        delete redStone[i];
        blackStone[i] = NULL;
        redStone[i] = NULL;
    }
    delete gameInfo;
    gameInfo = NULL;
    delete speaker;
    speaker = NULL;
    delete ui;
    delete timer;
    timer = NULL;
}

void Board::init()
{
    _windowHeight = 2*Stone::_r*(_rowSize+2);
    _windowWidth = 2*Stone::_r*(_colSize+2);
    //this->setWindowFlags(this->windowFlags()& Qt::WindowMinMaxButtonsHint);
    this->setFixedSize(_windowWidth,_windowHeight);         //设置窗口大小
    for (int i = 0; i < _stoneNum ; i++) {
        redStone[i] =new  Stone();
        blackStone[i] = new Stone();
        redStone[i]->init(i,Color::Red);
        blackStone[i]->init(i,Color::Black);
    }
    this->setAutoFillBackground(true);    //允许调整控件背景颜色
    pix = new QPixmap(this->size());
    pix->fill(Qt::transparent);
    gameInfo = new GameInfo();
    step.clear();
    //维护inLineStoneNum数组
    for (int j = 0; j <= _colSize;j++) {
        inLineStoneNum[0][j] = 1;//第一行刚开始一定有棋子
        for (int i = 1;i <= _rowSize;i++) {
            if(getStoneObject(i,j) != NULL)inLineStoneNum[i][j] = inLineStoneNum[i-1][j] + 1;
            else inLineStoneNum[i][j] = inLineStoneNum[i-1][j];
        }
    }

    speaker = new QTextToSpeech();
    timer = new QTimer(this);
    tim = new QTimer();
    connect(timer,&QTimer::timeout,
            [=]{
        ui->Turn->setVisible(true);
        timer->stop();
        update();
    });
}
/******************************system slot*******************************************/
void Board::closeEvent(QCloseEvent* e)
{
    e->ignore();
}
void Board::paintEvent(QPaintEvent* e)
{
    drawBoard();
    for (int i = 0; i < _stoneNum;i++) {
        drawStone(i,Color::Red);
        drawStone(i,Color::Black);
    }
    if(gameInfo->turn == TURN::Red)
    {
        //
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::red);
        ui->Turn->setPalette(pa);
        ui->Turn->setText("红方回合");
    }else{
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::black);
        ui->Turn->setPalette(pa);
        ui->Turn->setText("黑方回合");
    }
    QPainter p(this);
    p.drawPixmap(0,0,this->width(),this->height(),*pix);//绘制
    pix->fill(Qt::transparent);
    e->accept();
}
void Board::mouseReleaseEvent(QMouseEvent* e)
{
    clickOn( rcTransfromPoint(e->pos().x(),e->pos().y()));

    update();

    e->accept();
}
//***********************************base func**************************************//
void Board::drawBoard()//绘制棋盘
{
    QPainter painter(this);
    int d = Stone::_r*2;
    //Draw line
    for ( int i = 1 ; i <= 10 ; i++) {
        painter.drawLine( QPoint( d , i*d) , QPoint( 9 * d, i * d));
    }
    for ( int i = 1 ; i <= 9 ; i++) {
        if( i == 1 || i == 9)
            painter.drawLine( QPoint( i * d , d) , QPoint( i * d, 10 * d));
        else {
             painter.drawLine( QPoint( i * d , d) , QPoint( i * d, 5 * d));
             painter.drawLine( QPoint( i * d , 6*d) , QPoint( i * d, 10 * d));
        }
    }
    painter.drawLine( QPoint( 4 *d , d ) , QPoint( 6 * d, 3 * d ));
    painter.drawLine( QPoint( 6 *d , d ) , QPoint( 4 * d, 3 * d ));
    painter.drawLine( QPoint( 4 *d , 8 * d ) , QPoint( 6 * d, 10 * d ));
    painter.drawLine( QPoint( 6 * d , 8 * d ) , QPoint( 4 * d, 10 * d ));
}
void Board::drawStone(int _id,Color _color)
{
    QPainter painter(this);
    if(_id < 0 || _id > _stoneNum || _color == Color::None)return;
    Stone* s = NULL;
    if(_color == Color::Red)
    {
        if(redStone[_id]->isDied)return;//此子已死
         s = redStone[_id];
    }else{
        if(blackStone[_id]->isDied)return;
         s = blackStone[_id];
    }

    QPoint p = getStonePoint(s->row,s->col);   //获得棋子原本的坐标
    QRect rect = QRect( p.x() - Stone::_r , p.y() - Stone::_r , Stone::_r*2, Stone::_r*2 );     //写字的区域


    if( isSameStone(gameInfo,s))        // 被选中
    {
        //cout<<s->getName()<<endl;
        painter.setBrush(Qt::gray);     //略显颜色不同，表示被选中
    }
    else
        painter.setBrush(Qt::yellow);   //默认颜色

    if(s->color == Color::Red)
        painter.setPen(Qt::red);        //红子红字
    else
        painter.setPen( Qt::black);      //其他黑的就好

    painter.setFont(QFont("SimSun", Stone::_r , 700));    //默认字体

// /////////////////////////
    painter.drawEllipse(p , Stone::_r , Stone::_r);          //画圆
    painter.drawText(rect, s->getName() , QTextOption(Qt::AlignCenter));
}
QPoint Board::getStonePoint(int _row,int _col)
{
    return rcTranstoPoint(_row,_col);
}
QPoint Board::rcTranstoPoint(int _row,int _col)
{
    return (
                QPoint(
                ((_col+1)*Stone::_r*2),
                ((_row+1)*Stone::_r*2)
                )
            );
}
RowCol Board::rcTransfromPoint(double x,double y)
 {
    return (RowCol(
                static_cast<int>((int( y ) - Stone::_r ) / (2*Stone::_r)),
                static_cast<int>((int( x ) - Stone::_r ) / (2*Stone::_r))
                ));
}

Color Board::TurnToColor(TURN t)
{
    if(t == TURN::Red)return Color::Red;
    else if(t == TURN::Black)return Color::Black;
    throw "Turn to Color error";
}

Color Board::ldiColor(Color c)
{
    if(c == Color::Red)return Color::Black;
    if(c == Color::Black)return Color::Red;
    return Color::None;
}
bool Board::isSameStone(GameInfo* g,Stone* s)
{
//    cout<<"id:"<<g->selectedId<<" "<<s->id<<endl;
//    cout<<"color:"<<int(g->selectedColor)<<" "<<int(s->color)<<endl;
    if(g->selectedId == s->id && g->selectedColor == s->color)return 1;
    //cout<<0<<endl;
    return 0;
}

void Board::clickOn(int row,int col)
{
    if( row < 0 || row > _rowSize || col < 0 || col > _colSize )    //越界
        return;
    //cout<<"Select "<<row<<","<<col<<endl;
    Stone* s = getStoneObject(row,col);
    auto ss = getStoneObject(gameInfo);
    //cout<<s->getName()<<endl;
    if(s == NULL)//点击处无棋子
    {
        if(gameInfo->selectedId == -1)return;//无意义的操作
        if(gameInfo->selectedId != -1)//移动操作
        {
            if(ss == NULL || !moveStone( RowCol(row,col) ,ss) )
            {//移动失败
                return;
            }
        }
    }else{//有棋子
        if(isTurn(s))//自己的回合，选择自己的棋子
        {//切换选中棋子
            gameInfo->selectedId = s->id;
            gameInfo->selectedColor = s->color;
            return;//直接返回
        }
        //试图吃子
        else if(ss != NULL && isLegalMove(RowCol(row,col),ss))
        {
            Stone* temp = NULL;
            if(!step.isEmpty())
                temp = step.top().killStone;
            else temp = NULL;
            killStone(s);//先吃子,否则在判断老王通电话时，inLineStoneNum更新慢一步
            //NOTE:因为不好修改只能写下如此拙劣的代码
            step.top().killStone = temp;
            moveStoneEx( RowCol(row,col),ss );
            step.top().killStone = s;

        }else return;
    }
    //走完棋后看是否造成将军局面
    if(isGeneral(ldiColor(TurnToColor(gameInfo->turn))))//这一步自寻死路
    {
        backoutStone();//悔棋
        gameInfo->changeTurn();
        drawGeneral();//bug: 双炮对打，之后移开，没有将军的画面
        return;
    }
    else if(isGeneral(TurnToColor(gameInfo->turn))){//自己将军对面
        findSaveWay(ldiColor(TurnToColor(gameInfo->turn)));//看对方有没有解
        if(gameInfo->over)//游戏结束
        {
            drawWin(TurnToColor(gameInfo->turn));
            return ;//后面不弄了
        }
        drawGeneral();
    }

    gameInfo->changeTurn();//轮流操作
}
void Board::clickOn(RowCol rc)
{
    clickOn(rc.row,rc.col);
}
/******************************************************************/
Stone* Board::getStoneObject(int row,int col)
{
    for(int i = 0 ; i < _stoneNum ; i++)
    {
        if(redStone[i]->row == row && redStone[i]->col == col && !redStone[i]->isDied)return redStone[i];
        if(blackStone[i]->row == row && blackStone[i]->col == col && !blackStone[i]->isDied)return blackStone[i];
    }
    return NULL;
}

Stone *Board::getStoneObject(RowCol rc)
{
    return getStoneObject(rc.row,rc.col);
}
Stone* Board::getStoneObject(GameInfo* g)
{
    if(g->selectedId > -1 && g->selectedId < _stoneNum)
    {
        if(g->selectedColor == Color::Red && !redStone[g->selectedId]->isDied)return redStone[g->selectedId];
        else if(g->selectedColor == Color::Black && !blackStone[g->selectedId]->isDied)return blackStone[g->selectedId];
    }
    return NULL;
}

Stone *Board::getStoneObject(Color c, int id)
{
    if(id < 0 || id >= _stoneNum)return NULL;
    if(c == Color::Red)return redStone[id];
    else if(c == Color::Black)return blackStone[id];
    return NULL;
}
/******************************************************************/
bool Board::isTurn(Stone* s)
{
    //return gameInfo->turn == s->color;
    if(gameInfo->turn == TURN::Red && s->color == Color::Red)return 1;
    if(gameInfo->turn == TURN::Black && s->color == Color::Black)return 1;
    return 0;
}
/******************************************************************/
void Board::killStone(Stone* s)//杀死他
{
    if(s != NULL){
        s->isDied = 1;
        step.top().killStone = s;//记录吃子操作//todo:append
        for (int i =s->row;i <= _rowSize;i++) {//动态维护
            inLineStoneNum[i][s->col]--;
        }
    }
}
void Board::reliveStone(Stone* s)//复活他
{
    if(s != NULL){
        //动态维护
        for (int i = s->row;i <= _rowSize;i++) {
            inLineStoneNum[i][s->col]++;
        }
        s->isDied = 0;
    }
}
void Board::backoutStone()//悔棋
{
    //单机版悔一步，联网版看情况一或二步
    if(!step.isEmpty())
    {
        auto s= step.top();
        forceMoveStone(s.s,s.stone);//undo
        if(s.killStone != NULL)reliveStone(s.killStone);
        if(s.stone->type == TYPE::BING && s.stone->isCrossRiver &&
                ((s.s.row == 5 && s.t.row == 4)||(s.s.row == 4 && s.t.row == 5)))//过河状态回溯
        {
            s.stone->isCrossRiver = 0;
        }
        step.pop();
        gameInfo->changeTurn();
    }
}

bool Board::isGeneral(Color player)//判断下一步能否吃掉对方王
{
    if(player == Color::Black)
    {
        for (int i = 0;i < _stoneNum;i++) {
            if(isLegalMove(RowCol(redStone[4]->row,redStone[4]->col),blackStone[i]))//判断下一步能否吃掉将
            {
                return 1;
            }
        }
    }else{
        for (int i = 0;i < _stoneNum;i++) {
            if(isLegalMove(RowCol(blackStone[4]->row,blackStone[4]->col),redStone[i]))
            {
                return 1;
            }
        }
    }
    return 0;
}

void Board::findSaveWay(Color player)
{
    Stone* temp;
    RowCol sPoint;
    for (int k = 0;k < _stoneNum;k++) {
        temp = getStoneObject(player,k);
        if(temp->isDied == 0)//没死
        {
            for (int i = 0 ; i <= _rowSize;i++) {
                for (int j = 0 ; j <= _colSize;j++) {
                    try {
                        if(isLegalMove(RowCol(i,j),temp) )
                        {
                            sPoint = RowCol(temp->row,temp->col);
                            forceMoveStone(RowCol(i,j),temp);
                            if(!isGeneral(ldiColor(player)))//没被对方将军
                            {
                                forceMoveStone(sPoint,temp);
                                return;
                            }
                            forceMoveStone(sPoint,temp);
                        }
                    } catch (QString e) {
                        cout<<e;
                    }catch(...){
                        continue;
                    }
                }
            }
        }
    }
    gameInfo->over = 1;
}

void Board::drawGeneral()
{
    QPainter p(pix);
    QRect rec(this->_windowWidth/2 - 30 ,this->_windowHeight/2 - 15,60,30);
    p.setPen(Qt::red);
    p.setFont(QFont("SimSun",20));//宋体
    ui->Turn->setVisible(false);
    p.drawText(rec,"将军");
    speaker->say("将军");
    timer->start(1000);
}


void Board::drawWin(Color player)
{
    //todo：游戏结束画面绘制
    pix->fill(Qt::transparent);
    QPainter painter(pix);
    QRect rect = QRect(10 ,10, this->_windowWidth - 10 ,this->_windowHeight-10);
    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPointSize(50);
    font.setLetterSpacing(QFont::AbsoluteSpacing,0);
    painter.setFont(font);
    painter.setBrush(Qt::transparent);
    if(player == Color::Red)
    {
        painter.setPen(Qt::red);
        painter.drawText(rect,"红方胜",QTextOption(Qt::AlignCenter));
    }
    else{
        painter.setPen(Qt::black);
        painter.drawText(rect,"黑方胜",QTextOption(Qt::AlignCenter));
    }
    connect(tim,&QTimer::timeout,[=](){
            tim->stop();
            emit gameover();
            });
    tim->start(2000);
}

 /*********************合法移动判断****************************************/
bool Board::isLegalMove_CHE(RowCol t,Stone* s)
{
    auto d = t - RowCol(s->row,s->col);
    if(d.row*d.col != 0)//走斜线
        return 0;
    auto e = d.getE();//单位方向向量
   // cout<<"d :"<<d.row<<" "<<d.col<<endl;
    for(int i = 1 ; e*i != d; i++)//检查运动轨迹上是否有其他棋子
    {
       // cout<<(e*i).row<<" "<<(e*i).col<<endl;
        auto st = getStoneObject(e*i + RowCol(s->row,s->col));
        //cout<<st;
        if(st != NULL){return 0;}
    }
    return 1;
}

bool Board::isLegalMove_MA(RowCol t,Stone* s)
{
    auto d = t - RowCol(s->row,s->col);
    if(qAbs( d.row*d.col) != 2)return 0;//马走日
    //绊马脚
    if(qAbs(d.row) == 1)
    {
        if( getStoneObject(s->row,s->col + d.col/2) != NULL)return 0;
    }else{
        if( getStoneObject(s->row+d.row/2,s->col) != NULL)return 0;
    }
    return 1;
}

bool Board::isLegalMove_XIANG(RowCol t,Stone* s)
{
    auto d = t - RowCol(s->row,s->col);
    if(qAbs(d.row) != 2 || qAbs(d.col) != 2)return  0;//象飞田
    if( getStoneObject(s->row+d.row/2,s->col+d.col/2) != NULL)return 0;//塞象心
    if( (s->row == 4 && t.row == 6) || (s->row == 5 && t.row == 3))return 0;//不能过河
    return 1;
}

bool Board::isLegalMove_SHI(RowCol t,Stone* s)
{
    auto d = t - RowCol(s->row,s->col);
    if(qAbs(d.row*d.col ) != 1)return 0;//士斜滑
    //不能超过九宫格
    if(t.col < 3 || t.col>5)return 0;
    if((s->row == 7 && t.row == 6) || (s->row == 2 && t.row ==3))return 0;
    return 1;
}

bool Board::isLegalMove_SHUAI(RowCol t,Stone* s)
{
    auto d = t - RowCol(s->row,s->col);
    if( d.row*d.col != 0 || qAbs(d.row+d.col) != 1)return 0;
    //不能超过九宫格
    if(t.col < 3 || t.col>5)return 0;
    if((s->row == 7 && t.row == 6) || (s->row == 2 && t.row ==3))return 0;
    return 1;
}

bool Board::isLegalMove_BING(RowCol t,Stone* s)
{
    auto d = t - RowCol(s->row,s->col);
    if( (d.row*d.col != 0) || (qAbs(d.row + d.col) != 1))return 0;//走一格，直线
    if(s->direction*d.row < 0)return 0;//不能后退
    if(!s->isCrossRiver)//还没过河
    {
        if(qAbs(d.col) == 1)//不能左右
        return 0;
    }
    return 1;
}

bool Board::isLegalMove_PAO(RowCol t,Stone* s)
{
    auto d = t - RowCol(s->row,s->col);
    if(d.row*d.col != 0)return 0;//走直线
    int flag = 0;//路上是否有棋子
    auto e = d.getE();
    for(int i = 1 ; e*i != d; i++)//检查运动轨迹上是否有其他棋子
    {
        auto st = getStoneObject(e*i + RowCol(s->row,s->col));
        if(st != NULL)
            flag ++;
    }
    auto st = getStoneObject(t);
    if(flag > 1)return 0;
    if(flag == 0)
    {
        if(st != NULL)return 0;//
    }else{
        if(st == NULL)return 0;
    }
    return 1;
}

bool Board::isLegalMove(RowCol t,Stone* s)
{
    //note:貌似有点bug,回合未切换，但几次的运行结果不一致
    if(s == NULL)return 0;
    if(s != NULL && s->isDied)return 0;//
    Stone* temp = getStoneObject(t);
    if( temp != NULL && !temp->isDied && temp->color == s->color)return 0;//点击了相同的阵营的棋子

    if(redStone[4]->col == blackStone[4]->col)
    {
        moveStoneEx(t,s);
        gameInfo->changeTurn();//模拟回合切换
        int stNum = inLineStoneNum[redStone[4]->row][redStone[4]->col] - inLineStoneNum[blackStone[4]->row][blackStone[4]->col];
        backoutStone();
        if(qAbs(stNum) == 1)//老王通电话,因为是半开半闭区间，所以是==1
            return 0;
    }

    if( s->color == Color::None)
    {
        cout<<" not a right parameter.\n";
        return 0;
    }
    switch (s->type) {
    case TYPE::CHE:
        return isLegalMove_CHE(t,s);
    case TYPE::MA:
        return isLegalMove_MA(t,s);
    case TYPE::XIANG:
        return isLegalMove_XIANG(t,s);
    case TYPE::JIANG:
        return isLegalMove_SHUAI(t,s);
    case TYPE::SHI:
        return isLegalMove_SHI(t,s);
    case TYPE::PAO:
        return isLegalMove_PAO(t,s);
    case TYPE::BING:
        return isLegalMove_BING(t,s);
    default:
        return 0;
    }
}

bool Board::moveStone(RowCol t,Stone* s)
{
    if( isLegalMove(t,s) )
    {
        moveStoneEx(t,s);
        return 1;
    }
    return 0;
}

void Board::moveStoneEx(RowCol t, Stone *s)
{
    if(s->type == TYPE::BING && !s->isCrossRiver)
    {//验证是否过河
        if((s->row == 5 && t.row == 4) || (s->row == 4 && t.row == 5))
        {
            s->isCrossRiver = 1;//标记过河
        }
    }
    step.append(Step(RowCol(s->row,s->col),RowCol(t.row,t.col),s));//记录移动操作
    forceMoveStone(t,s);

}
//强制移动
void Board::forceMoveStone(RowCol t, Stone *s)
{
    if(s == NULL)return;
    //动态维护inLineStoneNum
    if(s->col == t.col)
    {
        if(s->row < t.row){
            for(int i = s->row ; i < t.row ; i++)
                inLineStoneNum[i][s->col]--;
        }else{
            for (int i = s->row - 1; i >= t.row; i--)
                inLineStoneNum[i][s->col]++;
        }
    }else{
        for (int i = s->row;i <= _rowSize;i++) {
            inLineStoneNum[i][s->col]--;
        }
        for (int i = t.row;i <= _rowSize;i++) {
            inLineStoneNum[i][t.col]++;
        }
    }
    s->row = t.row;
    s->col = t.col;
    if(s->type == TYPE::PAO && t.col == 7 && s->color == Color::Red)
        cout<<"stone site change: color:"<<step.count()<<" source row,col:"<<step.top().s.row<<" "<<step.top().s.col;
}

void Board::on_action_giveUp_triggered()
{
    //note:投降,单机模式不用显示征求意见
    emit gameover();
}

void Board::on_action_Undo_triggered()
{
    backoutStone();
    update();
}

