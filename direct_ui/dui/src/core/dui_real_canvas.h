#ifndef DUI_CORE_REAL_CANVAS_H_
#define DUI_CORE_REAL_CANVAS_H_

#include "dui/core/dui_canvas.h"


class DuiRealCanvas {
public:
    void Save();
    void Restore();
    void Clip(const DuiRect& clip_area);
    void DrawLine(const DuiPoint start, DuiPoint end, int line_size, DuiColor line_color, DuiPenStyle line_style);
    void DrawRect(const DuiRect& rc, int line_size, DuiColor line_color, DuiPenStyle pen_style);
    void DrawColor(const DuiRect& rc, DuiColor color);
    void DrawBitmap(DuiBitmap * bitmap, const DuiRect& dest, const DuiRect* src);

protected:
    DuiRealCanvas(DuiCanvas * owner);

protected:
    virtual void OnSave() = 0;
    virtual void OnRestore() = 0;
    virtual void OnClip(const DuiRect& clip_area) = 0;
    virtual void OnDrawLine(const DuiPoint start, DuiPoint end, int line_size, DuiColor line_color, DuiPenStyle line_style) = 0;
    virtual void OnDrawRect(const DuiRect& rc, int line_size, DuiColor line_color, DuiPenStyle pen_style) = 0;
    virtual void OnDrawColor(const DuiRect& rc, DuiColor color) = 0;
    virtual void DrawBitmap(DuiRealBitmap * bitmap, const DuiRect& dest, const DuiRect* src) = 0;

protected:
    DuiCanvas * owner_;
};

class DuiCanvasPriv final {
public:
    DuiCanvasPriv(DuiCanvas * canvas);
    ~DuiCanvasPriv();

    DuiRealCanvas * GetRealCanvas();

private:
    DuiCanvas* canvas_;
};

#endif//DUI_CORE_REAL_CANVAS_H_
