#ifndef QUI_CANVAS_BASE_H_
#define QUI_CANVAS_BASE_H_

#include "qui/core/qui_prerequisites.h"
#include "qui/core/qui_color.h"
#include "qui/core/qui_rect.h"
#include "qui/core/qui_bitmap.h"

class QUI_EXPORT QuiCanvasBase {
public:
    virtual ~QuiCanvasBase() { }
    virtual int Save() = 0;
    virtual void Restore(int save_id) = 0;
    virtual void DrawColor(const QuiRect& rect, QuiColor color) = 0;
    virtual void DrawBitmap(QuiBitmap * bitmap, const QuiRect& src, const QuiRect& dest) = 0;
};

#endif   // QUI_CANVAS_BASE_H_
