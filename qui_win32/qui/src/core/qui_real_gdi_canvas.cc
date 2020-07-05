#include "qui_real_gdi_canvas.h"

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
{

}
QuiRealGDICanvas::QuiRealGDICanvas(HWND hwnd)
    : hdc_(nullptr)
    , hwnd_(hwnd)
{
    hdc_ = ::GetDC(hwnd_);
}
QuiRealGDICanvas::~QuiRealGDICanvas()
{
    if (hwnd_) {
        ::ReleaseDC(hwnd_, hdc_);
        hwnd_ = nullptr;
        hdc_ = nullptr;
    }
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
        ::SetBkColor(hdc_, RGB(GetBValue(color), GetGValue(color), GetRValue(color)));
        ::ExtTextOut(hdc_, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
    }
    else {
        // draw transparent color
    }
}
