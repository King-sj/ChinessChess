#ifndef STEP_H
#define STEP_H
#include"Base.h"
class Stone;
class Step//储存其中一步
{
public:
    Step();
    Step(RowCol _s,RowCol _t ,Stone* _stone,Stone* _killStone = NULL);
public:
    RowCol s,t;// move: s -> t
    Stone* stone ;
    Stone* killStone ;//move stone to kill killStone
};
#endif // Step.h
