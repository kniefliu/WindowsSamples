#include "dui_real_canvas.h"

DuiRealCanvas::DuiRealCanvas(DuiCanvas * owner)
    : owner_(owner)
{
    if (owner_) {
        owner_->real_canvas_ = this;
    }
}

void DuiRealCanvas::Save()
{
    OnSave();
}
void DuiRealCanvas::Restore()
{
    OnRestore();
}
void DuiRealCanvas::Clip(const DuiRect& clip_area)
{
    OnClip(clip_area);
}
void DuiRealCanvas::DrawLine(const DuiPoint start, DuiPoint end, int line_size, DuiColor line_color, DuiPenStyle line_style)
{
    this->OnDrawLine(start, end, line_size, line_color, line_style);
}
void DuiRealCanvas::DrawRect(const DuiRect& rc, int line_size, DuiColor line_color, DuiPenStyle pen_style)
{
    this->OnDrawRect(rc, line_size, line_color, pen_style);
}
void DuiRealCanvas::DrawColor(const DuiRect& rc, DuiColor color)
{
    this->OnDrawColor(rc, color);
}
void DuiRealCanvas::DrawBitmap(DuiBitmap * bitmap, const DuiRect& dest, const DuiRect* src)
{
    this->DrawBitmap(bitmap->real_bitmap_, dest, src);
}

DuiCanvasPriv::DuiCanvasPriv(DuiCanvas * canvas)
    : canvas_(canvas)
{

}
DuiCanvasPriv::~DuiCanvasPriv()
{
    canvas_ = nullptr;
}

DuiRealCanvas * DuiCanvasPriv::GetRealCanvas()
{
    return canvas_->real_canvas_;
}
