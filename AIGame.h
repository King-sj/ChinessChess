#ifndef AIGAME_H
#define AIGAME_H
#include <QMainWindow>
#include <QObject>
#include"Board.h"
#include <QRandomGenerator>
#include<QMessageBox>

class AIGame : public Board
{
    Q_OBJECT
public:
    AIGame() = delete;
    AIGame(QMainWindow* parent);
    ~AIGame();
signals:

private:
    struct Strategic{
        int selectId = -1;
        RowCol t;
        //int mark = 0;
        Strategic(int id,int r,int c){
            selectId = id;
            t = RowCol(r,c);
        }
    };
private:
    Color selfColor = Color::None;
    Color computerColor = Color::None;
protected slots:
    virtual void paintEvent(QPaintEvent* e)override;//绘画事件
private:
    virtual void mouseReleaseEvent(QMouseEvent* e)override;//鼠标事件
    virtual void on_action_Undo_triggered()override;
    virtual void killStone(Stone* s)override;//杀死他
    virtual void reliveStone(Stone* s)override;//复活他
    virtual bool isGeneral(Color player)override;
//    virtual void backoutStone()override;
    virtual void drawGeneral()override;
//    virtual void backoutStone(bool isAI);

    virtual Strategic computerDecisonMoveWeak();//小白
    virtual Strategic computerDecisonMoveEx();//博弈搜索版本
private:
    //MaxMin搜索
    QTimer* comTimer;
    const int Inf = 0x3f3f3f3f;
    int liveStoneNum = 32;//存活棋子数量
    const int _level = 100;//走一步看32步
    int loop = 0 ;//防止死循环
    bool flagInSearch = 0;
    virtual std::pair<int,int> MaxMinSearch(Color color,int alpha ,int beta ,int level = 0,int mark = 0);//<inf,sup>
    virtual int marked(Strategic ,Color color,int level);//评分函数

};
#endif //AIGame.h
