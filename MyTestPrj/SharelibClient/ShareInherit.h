#pragma once

#include "Sharelib.h"

class ShareInherit:public Sharelib
{
public:
    ShareInherit();
    ~ShareInherit();

    int subtraction(int x,int y)override;



    int multiply(const int x, const int y);

};

