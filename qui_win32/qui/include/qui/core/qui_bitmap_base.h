#ifndef QUI_CORE_BITMAP_BASE_H_
#define QUI_CORE_BITMAP_BASE_H_

#include "qui/core/qui_prerequisites.h"

enum QuiBitmapColorType {
    kARGB8888_BitmapColor,
    kRGB8888_BitmapColor,
};
class QUI_EXPORT QuiBitmapBase {
public:
    virtual ~QuiBitmapBase() { }

    virtual int GetWidth() = 0;
    virtual int GetHeight() = 0;
    virtual QuiBitmapColorType GetColorType() = 0;
    virtual void BeginDraw() = 0;
    virtual void EndDraw() = 0;
};

#endif   // QUI_CORE_BITMAP_BASE_H_
