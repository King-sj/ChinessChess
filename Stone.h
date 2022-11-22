#ifndef STONE_H
#define STONE_H

#include<QString>
#include"Base.h"

class Stone
{
public:
    Stone();
    virtual ~Stone();

    virtual QString getName();
    virtual void init(int _id,Color _color);
public:
    const static int _r = 30;//半径
    int id;
    int row;
    int col;
    Color color;
    TYPE type;
    bool isDied = 0;//默认没死
    bool isCrossRiver = 0;//默认没过河
    int direction;//用于小兵不能后退
public:
    const struct{
        int _row;
        int _col;
        TYPE _type;
    }pos[16] = {
        {0,0,TYPE::CHE},        //0
        {0,1,TYPE::MA},
        {0,2,TYPE::XIANG},
        {0,3,TYPE::SHI},
        {0,4,TYPE::JIANG},      //4
        {0,5,TYPE::SHI},
        {0,6,TYPE::XIANG},
        {0,7,TYPE::MA},
        {0,8,TYPE::CHE},
        {2,1,TYPE::PAO},
        {2,7,TYPE::PAO},
        {3,0,TYPE::BING},
        {3,2,TYPE::BING},
        {3,4,TYPE::BING},
        {3,6,TYPE::BING},
        {3,8,TYPE::BING}
    };
};
#endif //Stone.h
