#ifndef QUI_CORE_SIZE_H_
#define QUI_CORE_SIZE_H_

#include "qui/core/qui_prerequisites.h"

struct QUI_EXPORT QuiSize {
public:
    int width;
    int height;

public:
    QuiSize()
        : width(0)
        , height(0)
    {

    }
    QuiSize(int w, int h)
    {
        width = w;
        height = h;
    }
    QuiSize(const SIZE& sz)
    {
        width = sz.cx;
        height = sz.cy;
    }

    operator SIZE()
    {
        return{ width, height };
    }
};


#endif   // QUI_CORE_SIZE_H_