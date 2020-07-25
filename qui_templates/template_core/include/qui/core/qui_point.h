#ifndef QUI_CORE_POINT_H_
#define QUI_CORE_POINT_H_

#include "qui/core/qui_prerequisites.h"

struct QUI_EXPORT QuiPoint {
public:
    int x;
    int y;

public:
    QuiPoint()
        : x(0)
        , y(0)
    {

    }
    QuiPoint(int x_value, int y_value)
    {
        x = x_value;
        y = y_value;
    }
    QuiPoint(const POINT& pt) 
    {
        x = pt.x;
        y = pt.y;
    }
    operator POINT()
    {
        return{ x, y };
    }
};


#endif   // QUI_CORE_POINT_H_