#include "Stone.h"

Stone::Stone()
{
    init(-1,Color::None);
}
Stone::~Stone()
{

}
QString Stone::getName()
{
    if( this->color == Color::None)
    {
        cout<<" not a right parameter.\n";
        return "error";
    }
    switch (this->type) {
    case TYPE::CHE:
        return "车";
    case TYPE::MA:
        return "马";
    case TYPE::XIANG:
        return "象";
    case TYPE::JIANG:
        if(this->color == Color::Red)
            return "帅";
        return "将";
    case TYPE::SHI:
        return "士";
    case TYPE::PAO:
        return "炮";
    case TYPE::BING:
        if(this->color == Color::Red)
            return "兵";


        return "卒";
    default:
        return "Eror";
    }
}
void Stone::init(int _id,Color _color)
{
    this->id = _id;
    this->color = _color;
    if(color == Color::Red)
    {
        //坐标变换
        row = 9 - pos[id]._row;
        col = 8 - pos[id]._col;
        direction = -1;
    }else if(color == Color::Black){
        row = pos[id]._row;
        col = pos[id]._col;
        direction = 1;
    }
    type = pos[id]._type;
}
