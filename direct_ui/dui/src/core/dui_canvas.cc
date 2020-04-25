#include "dui/core/dui_canvas.h"

#include "core/dui_real_canvas.h"

DuiCanvas::DuiCanvas()
    : real_canvas_(nullptr)
{

}

void DuiCanvas::Save()
{
    real_canvas_->Save();
}
void DuiCanvas::Restore()
{
    real_canvas_->Restore();
}
void DuiCanvas::Clip(const DuiRect& area)
{
    real_canvas_->Clip(area);
}
void DuiCanvas::DrawLine(const DuiPoint start, DuiPoint stop, int line_size, DuiColor line_color, DuiPenStyle pen_style)
{
    real_canvas_->DrawLine(start, stop, line_size, line_color, pen_style);
}
void DuiCanvas::DrawRect(const DuiRect& rc, int line_size, DuiColor line_color, DuiPenStyle pen_style)
{
    real_canvas_->DrawRect(rc, line_size, line_color, pen_style);
}
void DuiCanvas::DrawColor(const DuiRect& rc, DuiColor color)
{
    real_canvas_->DrawColor(rc, color);
}
void DuiCanvas::DrawBitmap(DuiBitmap * bitmap, const DuiRect& dest, const DuiRect* src)
{
    real_canvas_->DrawBitmap(bitmap, dest, src);
}
