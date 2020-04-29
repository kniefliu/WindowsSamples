#include "dui_gdi_real_bitmap_win.h"

#include "dui_gdi_real_canvas_win.h"

DuiBitmap * DuiRealBitmap::MakePlatformBitmap(int width, int height, DuiColor pure_color, DuiPixelType pixel_type)
{
    DuiBitmap * bitmap = new DuiBitmap();
    GdiRealBitmap * gdi_bitmap = new GdiRealBitmap(bitmap, width, height);
    gdi_bitmap->Clear(pure_color);
    return bitmap;
}

GdiRealBitmap::GdiRealBitmap(DuiBitmap * owner, int width, int height)
    : DuiRealBitmap(owner)
    , width_(width)
    , height_(height)
{
    hbitmap_ = DuiGdiRealCanvas::CreateARGB32Bitmap(::GetDC(::GetDesktopWindow()), width, height, &hbitmap_pixels_);
}
GdiRealBitmap::~GdiRealBitmap()
{

}

void GdiRealBitmap::Clear(DuiColor color)
{
    DuiRect area = { 0, 0, width_, height_ };
    COLORREF * pixel = nullptr;
    for (int y = area.top; y < area.bottom; ++y) {
        for (int x = area.left; x < area.right; ++x) {
            pixel = hbitmap_pixels_ + (height_ - y - 1) * width_ + x;
            DuiColor old_color = *pixel;
            *pixel = color;
        }
    }
}
