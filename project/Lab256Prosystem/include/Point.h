#pragma once

#include <iostream>
#include <QDebug>

class Point//Point类的声明
{
public://外部接口
    Point(double x = 0, double y = 0) { m_x = x; m_y = y; }
    double GetX() const { return m_x; }
    double GetY() const { return m_y; }
    void SetX(double value) { m_x = value; }
    void SetY(double value) { m_y = value; }

    inline friend std::ostream& operator<<(std::ostream& os, const Point& np)
    {
        os << "(" << np.m_x << "," << np.m_y << ")";
        return os;
    }

    inline friend QDebug operator<<(QDebug dbg, const Point& np)
    {
        dbg << "(" << np.m_x << "," << np.m_y << ")";
        return dbg;
    }

private: //私有数据成员
    double m_x;
    double m_y;
};

