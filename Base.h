#ifndef BASE_H
#define BASE_H
#include<QDebug>
#include<QtMath>
#define cout qDebug()<<"["<<__LINE__<<":"<<__FILE__<<":"<<__FUNCTION__<<"]"
enum class TYPE{CHE = 0,MA,XIANG,SHI,JIANG,BING,PAO};
enum class Color{Red = 0 , Black , None };
enum class TURN{Red = 0,Black};

struct GameInfo
{
    GameInfo()
    {
    }
private:

public:
    int selectedId = -1;//目前被选中的棋子id
    Color selectedColor = Color::None;//与其颜色
    TURN turn = TURN::Red;//谁的回合
    bool over = 0;
    bool isByGen = 0;//是否被将军
public:
    void changeTurn()
    {
        if(turn == TURN::Red)turn = TURN::Black;
        else turn = TURN::Red;
        selectedId = -1;
        selectedColor = Color::None;
    }

};
struct RowCol{
    int row,col;

    RowCol(int r = 0,int c = 0){
        row = r;
        col = c;
    }
    RowCol getE(){//获得该方向单位向量
        if(len() == 0)throw "RowCol len is 0";
        return RowCol(int(this->row/len()),int(this->col/len()));
    }
    int len(){//返回模长
        return qSqrt(this->row*this->row + this->col*this->col);
    }
    friend RowCol operator-(const RowCol& l ,const RowCol& r)
    {
        return RowCol(l.row - r.row , l.col - r.col);
    }
//    RowCol operator-(){
//        return RowCol(-row,-col);
//    }
    RowCol operator+(const RowCol& h)
    {
        return RowCol(row+h.row,col+h.col);
    }
    bool operator==(const RowCol& h)
    {
        return row == h.row && col == h.col;
    }
    bool operator!=(const RowCol& h)
    {
        return !(*this == h);
    }
    bool operator<(const RowCol& h)
    {
        if(row*h.col - col*h.row != 0){//叉积
            throw "两向量不平行";
        }
        return (row < h.row || col < h.col);//可能会水平或者垂直,加一层判断即可
    }
    bool operator<=(const RowCol& h){
        return *this<h || *this == h;
    }
    RowCol operator*(int k)
    {
        return RowCol(k*row,k*col);
    }
};
//struct IP{
//    int first = -1;
//    int second = -1;
//    int third = -1;
//    int forth = -1;
//    IP(int _first,int _second,int _third,int _forth)
//    {
//        first = _first;
//        second = _second;
//        third = _third;
//        forth = _forth;
//    };
//};

#endif //Base.h
