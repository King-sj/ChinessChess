#include "AIGame.h"

/*
 * 弱人工智障初步成功
 * todo:添加人工智能
 * 添加将军时的求解
 * bug:搜索过程造成的将军一下子回退了很多
 * 需要出现isGeneral,可能,以避免搜索过程造成的将军导致的回退
 * 修改isgeneral -> return 0后仍有回退
 * 而且 ai 选中了 red的士
 * AIGame外多调用了backou
 * 产生了走了会将军的走法(可能原因，计算分数不准确
*/
AIGame::AIGame(QMainWindow *parent):Board(parent)
{
    this->setWindowTitle("对战人机");
    comTimer = new QTimer(this);
    comTimer->setSingleShot(true);
    int t = QRandomGenerator::global()->bounded(0,100);
    //test
    t = 0;
    if( t%2 == 0){//玄学
        selfColor = Color::Red;
        computerColor = Color::Black;
    }
    else {
        selfColor = Color::Black;
        computerColor = Color::Red;
    }

    if(selfColor == Color::Black)
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

    connect(comTimer,&QTimer::timeout,[=]{
        cout<<"ai Turn";
        loop++;
        if(loop > 2){
            QMessageBox msb;
            msb.setText("game over");
            msb.exec();
            emit gameover();
        }
        auto s = computerDecisonMoveEx();//加强版
        gameInfo->selectedColor = computerColor;
        gameInfo->selectedId = s.selectId;
        flagInSearch = false;
        clickOn(s.t);
        //gameInfo->changeTurn();//玄学
        flagInSearch = 0;
        //test
        //std::swap(computerColor,selfColor);

        update();
        //test
        //comTimer->start(1000);

    });
    //comTimer->start(1000);
}

AIGame::~AIGame()
{
    delete comTimer;
    comTimer = NULL;
}

void AIGame::paintEvent(QPaintEvent *e)
{
    Board::paintEvent(e);
    //test
    //comTimer->start(2000);
    if(computerColor == TurnToColor(gameInfo->turn) )
    {
        comTimer->start(100);
    }

}

void AIGame::mouseReleaseEvent(QMouseEvent *e)
{
    if(selfColor != TurnToColor(gameInfo->turn))return;
    loop = 0;
    Board::mouseReleaseEvent(e);
}


void AIGame::on_action_Undo_triggered()
{
    if(selfColor == TurnToColor(gameInfo->turn))backoutStone();
    backoutStone();
    update();
}

void AIGame::killStone(Stone *s)
{
    if(s != NULL)
    {
        liveStoneNum--;
        Board::killStone(s);
    }
}

void AIGame::reliveStone(Stone *s)
{
    if(s != NULL)
    {
        liveStoneNum++;
        Board::reliveStone(s);
    }
}

bool AIGame::isGeneral(Color player)
{
    if(flagInSearch)
    {//ai在搜索中
        return 0;
    }
    return Board::isGeneral(player);

}



void AIGame::drawGeneral()
{
    Board::drawGeneral();
    //test
    //update();
}



AIGame::Strategic AIGame::computerDecisonMoveWeak()
{//此函数只是用作初步测试
    for(int k = 0 ; k < _stoneNum ; k++)
    {
        auto s = getStoneObject(computerColor,k);
        if(!s->isDied)
        {
            for(int i = 0 ; i <= _rowSize ; i++)
                for(int j = 0 ; j <= _colSize ; j++)
                {
                    if(isLegalMove(RowCol(i,j),s))
                    {
                        return AIGame::Strategic(k,i,j);
                    }
                }
        }
    }
    throw "computer think error";
}

AIGame::Strategic AIGame::computerDecisonMoveEx()
{
    flagInSearch = 1;
    int markArr[_stoneNum][_rowSize+1][_colSize + 1];

    for(int k = 0; k < _stoneNum ; k++)
    {
        for(int i = 0 ; i <= _rowSize ; i++ )
        {
            for(int j = 0 ; j <= _colSize ; j++)
            {
                markArr[k][i][j] = -2*Inf;

                gameInfo->selectedColor = computerColor;
                gameInfo->selectedId = k;
                flagInSearch = false;
                if(isLegalMove(RowCol(i,j),getStoneObject(computerColor,k)))
                {//保证了决策的合法性
                    flagInSearch = true;
                    markArr[k][i][j] = -Inf;
                    //cout<<"red pao col:"<<redStone[9]->col<<Qt::endl;
                    auto tempStone = getStoneObject(i,j);//
                    //if(tempStone != NULL && tempStone->color == computerColor)continue;//无效运动
                    if(tempStone != NULL && tempStone->type == TYPE::JIANG)return Strategic(k,i,j);//必胜局面

                    markArr[k][i][j] = marked(Strategic(k,i,j),computerColor,0);//先打分，否则目标点棋子已经变了
                    gameInfo->selectedColor = computerColor;
                    gameInfo->selectedId = k;
//                    cout<<"cilick :red pao col:"<<redStone[9]->col<<Qt::endl;
//                    cout<<step.count()<<Qt::endl;
                    clickOn(RowCol(i,j));
                    flagInSearch = false;
                    if(isGeneral(selfColor))
                    {//被将军
                        //note:可能会多步骤，或者少了
                        flagInSearch = true;
                        markArr[k][i][j] = -Inf;
                        backoutStone();
                        continue;
                    }
//                    cout<<"red pao col:"<<redStone[9]->col<<Qt::endl;
//                    cout<<step.count()<<Qt::endl;
                    std::pair<int,int> mm = MaxMinSearch(selfColor,-Inf,Inf,1,markArr[k][i][j]);//只改变下界
                    markArr[k][i][j] = std::max(mm.first,mm.second);
                    //markArr[k][i][j] = mm.first;
                    //cout<<"ai think process:(k,i,j) "<<k<<i<<j<<Qt::endl;
//                    cout<<"undo:red pao col:"<<redStone[9]->col<<Qt::endl;
//                    cout<<step.count()<<Qt::endl;
                    backoutStone();
//                    cout<<"red pao col:"<<redStone[9]->col<<Qt::endl;
//                    cout<<step.count()<<Qt::endl;
                }
            }
        }
    }
    int id,r,c,m = -Inf;
    for(int k = 0; k < _stoneNum ; k++)
    {
        for(int i = 0 ; i <= _rowSize ; i++ )
        {
            for(int j = 0 ; j <= _colSize ; j++)
            {
                //todo : erase"="
                if(markArr[k][i][j] >= m)//寻找最大下界，及其决策
                {
                    //cout<<"maxInf and inf "<<m<<" "<<markArr[k][i][j]<<Qt::endl;
                    m = markArr[k][i][j];
                    cout<<"max profite:"<<m<<Qt::endl;
                    id = k;
                    r = i;
                    c = j;
                }
            }
        }
    }
    flagInSearch = false;
    return Strategic(id,r,c);
}
//当前操作者阵营，下界，上界，搜索深度，累计分数
std::pair<int,int> AIGame::MaxMinSearch(Color color,int alpha,int beta,int level,int mark )
{
    if(level == _level)//0 ->  _level - 1;_level层不再做决策
    {//叶节点
        cout<<level<<Qt::endl;
        return std::make_pair(mark,mark);
    }
    if(alpha >= beta)return std::make_pair(alpha,beta);//alpha-beta剪枝
    //注意模拟过程造成的必胜局面，可能导致游戏提前结束，需要处理
    for(int k = 0; k < _stoneNum ; k++)
    {
        for(int i = 0 ; i <= _rowSize ; i++ )
        {
            for(int j = 0 ; j <= _colSize ; j++)
            {
                gameInfo->selectedColor = color;
                gameInfo->selectedId = k;
                flagInSearch = false;
                if(isLegalMove(RowCol(i,j),getStoneObject(color,k)))
                {
                    flagInSearch = true;
                    //cout<<"red pao col:"<<redStone[9]->col<<Qt::endl;
                    auto tempStone = getStoneObject(i,j);
                    if(tempStone != NULL && tempStone->type == TYPE::JIANG)
                    {
                        if(color == computerColor)return std::make_pair(Inf,Inf);
                        else return std::make_pair(-Inf,-Inf);
                    }



//                    cout<<"cilick:red pao col:"<<redStone[9]->col<<Qt::endl;
//                    cout<<step.count()<<Qt::endl;
                    int tempM = marked(Strategic(k,i,j),color,0);//先打分
                    gameInfo->selectedColor = color;
                    gameInfo->selectedId = k;
                    clickOn(i,j);
//                    cout<<"red pao col:"<<redStone[9]->col<<Qt::endl;
//                    cout<<step.count()<<Qt::endl;
                    flagInSearch = false;
                    if(isGeneral(ldiColor(color)))
                    {//被将军
                        //note:可能会多步骤，或者少了
                        //tempM = -2*Inf;
                        flagInSearch = true;
                        backoutStone();
                        continue;
                    }
                    auto stra = MaxMinSearch(ldiColor(color),alpha,beta,level+1,
                                   mark + tempM );
//                    cout<<"undo:red pao col:"<<redStone[9]->col<<Qt::endl;
//                    cout<<step.count()<<Qt::endl;
                    backoutStone();//回溯
//                    cout<<"red pao col:"<<redStone[9]->col<<Qt::endl;
//                    cout<<step.count()<<Qt::endl;
                    if(color == computerColor)//回溯过了
                    {//max

                        alpha = std::max(stra.first,alpha);
                        alpha = std::max(stra.second,alpha);
                        return std::make_pair(alpha,beta);
                    }else{
                        //min
                        beta = std::min(stra.first,beta);
                        beta = std::min(stra.second,beta);
                        return std::make_pair(alpha,beta);
                    }
                }
            }
        }
    }
    throw "error: min-max don't find answer";
}

int AIGame::marked(Strategic s,Color color, int level)//note:出现没考虑自身安危的情况
{
    if(level < 0)return 0;
    int res =0 ;
    //搜索深度越深，分数越低
    if(level >= 10)res = -level;
    auto stone = getStoneObject(s.t.row,s.t.col);//目标位置的棋子

    if(stone == NULL || stone->isDied)//单纯移动
    {
        //todo:目前不加分，但应该有分，需要个更好的评价函数
        return res;
    }
    //if(stone->color == color)return -2*Inf;//不被允许的方案
    int temp = 0;
    //杀死了对方棋子
    switch (stone->type) {
        case TYPE::CHE:
            temp = 10000;
            break;
        case TYPE::MA:
            temp = 1000 + (2*_stoneNum - liveStoneNum)*40;
            break;
        case TYPE::XIANG:
            temp = 900;
            break;
        case TYPE::JIANG:
            temp = 100000;//此时应该为必胜策略
            break;
        case TYPE::SHI:
            temp = 900;
            break;
        case TYPE::PAO:
            temp = qAbs(2000 - (2*_stoneNum - liveStoneNum)*40) + 10;
            break;
        case TYPE::BING:
            if(stone->isCrossRiver)temp = 500;
            else temp = 50;
            break;
        default:
            throw "Eror";
    }
    if(color == computerColor)//自己的被杀了
    {
        return res + temp;
    }else if(color == selfColor)//电脑自己的被杀了
    {
        return res - temp;
    }
    throw "marked error";
}

