#ifndef QUI_CORE_RECT_H_
#define QUI_CORE_RECT_H_

#include "qui/core/qui_prerequisites.h"

struct QUI_EXPORT QuiRect {
public:
    int left;
    int top;
    int right;
    int bottom;

public:
    QuiRect()
        : left(0)
        , top(0)
        , right(0)
        , bottom(0)
    {
    }
    QuiRect(const RECT& rc)
    {
        left = rc.left;
        top = rc.top;
        right = rc.right;
        bottom = rc.bottom;
    }
    operator RECT()
    {
        return{ left, top, right, bottom };
    }

public:
    int Width() const { return right - left; }
    int Height() const { return bottom - top; }
};


#endif   // QUI_CORE_RECT_H_
