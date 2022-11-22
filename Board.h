/*
 * @author: songJian
 * @finishData:2022/11/19
 * @version:5.0
 *
*/

#ifndef BOARD_H
#define BOARD_H
#include <QMainWindow>
#include<QCloseEvent>
#include"Stone.h"
#include<QPixmap>
#include<QPainter>
#include<QMouseEvent>
#include<QPoint>
#include"Base.h"
#include"Step.h"
#include<QStack>
#include<QTimer>
#include<QtTextToSpeech>//语音

namespace Ui {
class Board;
}

class Board : public QMainWindow
{
    Q_OBJECT

public:
    explicit Board(QWidget *parent = nullptr);
    virtual ~Board();
private:
    void init();
    /*******************************base func*************************************/
    virtual void drawStone(int _id,Color _color);
    virtual void drawBoard();

protected://拙劣的写法，因为懒
    const static int _rowSize = 9;           //行数 0 - 9
    const static int _colSize = 8;           //列数 0 - 8
    const static int _stoneNum = 16;
    Stone* blackStone[_stoneNum];
    Stone* redStone[_stoneNum];
    GameInfo* gameInfo;
    /*************************************************************************/
    virtual Color  TurnToColor(TURN t);//回合转为玩家阵营（颜色
    virtual RowCol rcTransfromPoint(double x,double y);//点->行列
    virtual void clickOn(int _row,int _col);//处理点击
    virtual void clickOn(RowCol rc);
    virtual void backoutStone();//悔棋
    virtual Stone* getStoneObject(Color c,int id);
    virtual Stone* getStoneObject(int row,int col);
    virtual void killStone(Stone* s);//杀死他
    virtual void reliveStone(Stone* s);//复活他
    virtual Color  ldiColor(Color c);//颜色取反
    virtual bool isGeneral(Color player);//将军对方否
    virtual void drawGeneral();//将军语音和画面
private:
    Ui::Board *ui;
    QPixmap* pix;
private://just test
    QStack<Step>step;
private:
    QTextToSpeech *speaker;
    QTimer* timer;
    QTimer* tim;
    /****************************************************************************/
    int _windowHeight;  //窗口高度
    int _windowWidth;   //窗口宽度
    int inLineStoneNum[_rowSize+1][_colSize+1];//维护每一列棋子数量的前缀和，用于判断是否老王通电话

signals:
    void gameover();
protected slots:
    //----------------------系统事件处理---------------------------------------
    virtual void closeEvent(QCloseEvent* e);//窗口关闭事件
    virtual void paintEvent(QPaintEvent* e);//绘画事件
    virtual void mouseReleaseEvent(QMouseEvent* e);//鼠标事件
private:
    /***************************************************************/
    /***************************************************************************/
    virtual QPoint getStonePoint(int row,int col);//获得棋子坐标
    virtual QPoint rcTranstoPoint(int row,int col);//行列->点


    /***************************************************************/
    virtual bool isSameStone(GameInfo*,Stone*);//判断是否是同一个棋子
    /***************************************************************/

    virtual Stone* getStoneObject(RowCol rc);
    virtual Stone* getStoneObject(GameInfo* g);

    /***************************************************************/
    virtual bool isTurn(Stone* s);//判断是否是自己回合
    /***************************************************************/


    /****************************************************************/

    virtual void findSaveWay(Color Player);//找活路,没找到把游戏设为结束

    virtual void drawWin(Color player);//结束画面
private:
    /*********************合法移动判断****************************************/
    bool isLegalMove_CHE(RowCol t,Stone* s);
    bool isLegalMove_MA( RowCol t,Stone* s);
    bool isLegalMove_XIANG( RowCol t,Stone* s);
    bool isLegalMove_SHI( RowCol t,Stone* s);
    bool isLegalMove_SHUAI(RowCol t,Stone* s);
    bool isLegalMove_BING(RowCol t,Stone* s);
    bool isLegalMove_PAO( RowCol t,Stone* s);
    /*************************************************************************/
public:
     /*********************合法移动判断****************************************/
    bool isLegalMove(RowCol t,Stone* s);
    /*********************移动棋子****************************************/
    virtual bool moveStone(RowCol t,Stone* s);//移动成功返回1，否则0，记录每一步,
    virtual void moveStoneEx(RowCol t,Stone* s);//直接移动，但有记录，即已经确认过合法
    virtual void forceMoveStone(RowCol t,Stone* s);//不记录回合，无论如何，直接移动
protected slots:
    virtual void on_action_giveUp_triggered();
    virtual void on_action_Undo_triggered();
};
#endif //Board.h
