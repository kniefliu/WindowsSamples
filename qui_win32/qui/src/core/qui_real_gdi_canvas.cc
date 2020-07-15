#include "qui_real_gdi_canvas.h"

#include "qui/core/qui_bitmap.h"
#include "qui_real_gdi_bitmap.h"

#include <Windows.h>

QuiCanvas * QuiCanvas::MakeRaster(QuiBitmap* bitmap)
{
    QuiRealGDIBitmap * gdi_bitmap = static_cast<QuiRealGDIBitmap*>(bitmap->real_bitmap_);
    QuiRealGDICanvas * gdi_canvas = new QuiRealGDICanvas(gdi_bitmap);
    if (!gdi_canvas) {
        return nullptr;
    }
    QuiCanvas * canvas = new QuiCanvas();
    canvas->real_canvas_ = gdi_canvas;
    return canvas;
}

QuiCanvas * QuiCanvas::MakeCanvasFromHDC(void * hdc)
{
    // check
    QuiRealGDICanvas * gdi_canvas = new QuiRealGDICanvas((HDC)hdc);
    if (!gdi_canvas) {
        return nullptr;
    }
    QuiCanvas * canvas = new QuiCanvas();
    canvas->real_canvas_ = gdi_canvas;
    return canvas;
}

QuiCanvas * QuiCanvas::MakeCanvasFromHWND(void * hwnd)
{
    // check
    QuiRealGDICanvas * gdi_canvas = new QuiRealGDICanvas((HWND)hwnd);
    if (!gdi_canvas) {
        return nullptr;
    }
    QuiCanvas * canvas = new QuiCanvas();
    canvas->real_canvas_ = gdi_canvas;
    return canvas;
}

QuiRealGDICanvas::QuiRealGDICanvas(HDC hdc)
    : hdc_(hdc)
    , hwnd_(nullptr)
    , gdi_bitmap_(nullptr)
{

}
QuiRealGDICanvas::QuiRealGDICanvas(HWND hwnd)
    : hdc_(nullptr)
    , hwnd_(hwnd)
    , gdi_bitmap_(nullptr)
{
    hdc_ = ::GetDC(hwnd_);
}
QuiRealGDICanvas::QuiRealGDICanvas(QuiRealGDIBitmap* gdi_bitmap)
    : hdc_(nullptr)
    , hwnd_(nullptr)
    , gdi_bitmap_(gdi_bitmap)
{
    hdc_ = gdi_bitmap_->hdc_;
}
QuiRealGDICanvas::~QuiRealGDICanvas()
{
    if (hwnd_) {
        ::ReleaseDC(hwnd_, hdc_);
        hwnd_ = nullptr;
        hdc_ = nullptr;
    }
    gdi_bitmap_ = nullptr;
}

int QuiRealGDICanvas::Save()
{
    return ::SaveDC(hdc_);
}
void QuiRealGDICanvas::Restore(int save_id)
{
    ::RestoreDC(hdc_, save_id);
}
void QuiRealGDICanvas::DrawColor(const QuiRect& rect, QuiColor clr)
{
    DWORD color = clr;
    RECT rc = { rect.left, rect.top, rect.right, rect.bottom };
    if (color <= 0x00FFFFFF) return;
    if (color >= 0xFF000000) {
#if 1
        ::SetBkColor(hdc_, RGB(GetBValue(color), GetGValue(color), GetRValue(color)));
        ::ExtTextOut(hdc_, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
#else
        HBRUSH hbrush = ::CreateSolidBrush(RGB(GetBValue(color), GetGValue(color), GetRValue(color)));
        ::FillRect(hdc_, &rc, hbrush);
        ::DeleteObject(hbrush);
#endif
    }
    else {
        // draw transparent color
    }
}
void QuiRealGDICanvas::DrawBitmap(QuiBitmap * bitmap, const QuiRect& src, const QuiRect& dest)
{
    QuiRealGDIBitmap * gdi_bitmap = static_cast<QuiRealGDIBitmap*>(bitmap->real_bitmap_);
    ::BitBlt(hdc_, dest.left, dest.top, dest.Width(), dest.Height(), gdi_bitmap->hdc_, src.left, src.top, SRCCOPY);
}
