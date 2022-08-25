#pragma once

#include "Sharelib_global.h"


SHARELIB_EXPORT int subtract(int x, int y);


class SHARELIB_EXPORT Sharelib
{
public:
    Sharelib();
    virtual ~Sharelib();
    int add(int x, int y);

    virtual int subtraction(int x,int y);
};
