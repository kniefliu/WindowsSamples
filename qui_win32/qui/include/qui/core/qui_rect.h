#ifndef QUI_CORE_RECT_H_
#define QUI_CORE_RECT_H_

#include "qui/core/qui_prerequisites.h"

struct QUI_EXPORT QuiRect {
    int left;
    int top;
    int right;
    int bottom;

    int Width() const { return right - left; }
    int Height() const { return bottom - top; }
};


#endif   // QUI_CORE_RECT_H_
