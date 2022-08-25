#include "ShareInherit.h"

ShareInherit::ShareInherit():Sharelib()
{

}

ShareInherit::~ShareInherit()
{

}

int ShareInherit::multiply(const int x, const int y)
{
    return x*y;
}


int ShareInherit::subtraction(int x,int y)
{
    return x -y-5;
}
