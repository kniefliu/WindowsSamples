#ifndef QUI_CORE_CANVAS_H_
#define QUI_CORE_CANVAS_H_

#include "qui/core/qui_prerequisites.h"
#include "qui/core/qui_canvas_base.h"

class QuiBitmap;
class QUI_EXPORT QuiCanvas final : public QuiCanvasBase {
public:
    static QuiCanvas * MakeRaster(QuiBitmap* bitmap);

    static QuiCanvas * MakeCanvasFromHDC(void * hdc);
    static QuiCanvas * MakeCanvasFromHWND(void * hwnd);

public:
    ~QuiCanvas() override;
  
public:
    int Save() override;
    void Restore(int save_id) override;
    void DrawColor(const QuiRect& rect, QuiColor clr) override;
    void DrawBitmap(QuiBitmap * bitmap, const QuiRect& src, const QuiRect& dest) override;

private:
    QuiCanvas();

private:
    QuiCanvasBase * real_canvas_;
};

#endif   // QUI_CORE_CANVAS_H_
