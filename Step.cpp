#include "Step.h"

Step::Step()
{

}

Step::Step(RowCol _s, RowCol _t, Stone *_stone, Stone* _killStone)
{
    s = _s;
    t = _t;
    stone = _stone;
    killStone = _killStone;
}
