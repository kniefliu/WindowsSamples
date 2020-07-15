#include "qui/core/qui_canvas.h"

#include "qui/core/qui_bitmap.h"


QuiCanvas::QuiCanvas()
    : real_canvas_(nullptr)
{

}
QuiCanvas::~QuiCanvas()
{
    if (real_canvas_) {
        delete real_canvas_;
        real_canvas_ = nullptr;
    }
}

int QuiCanvas::Save()
{
    if (real_canvas_) {
        return real_canvas_->Save();
    }
    return -1;
}
void QuiCanvas::Restore(int save_id)
{
    if (real_canvas_) {
        real_canvas_->Restore(save_id);
    }
}
void QuiCanvas::DrawColor(const QuiRect& rect, QuiColor clr)
{
    if (real_canvas_) {
        real_canvas_->DrawColor(rect, clr);
    }
}
void QuiCanvas::DrawBitmap(QuiBitmap * bitmap, const QuiRect& src, const QuiRect& dest)
{
    if (real_canvas_) {
        real_canvas_->DrawBitmap(bitmap, src, dest);
    }
}
