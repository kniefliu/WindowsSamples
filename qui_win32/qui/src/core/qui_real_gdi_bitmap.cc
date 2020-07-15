#include "qui_real_gdi_bitmap.h"

#include <Windows.h>

HBITMAP CreateARGB32Bitmap(HDC hdc, int w, int h, COLORREF** bits)
{
    BITMAPINFO bitmap_info;
    ::ZeroMemory(&bitmap_info, sizeof(bitmap_info));

    bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmap_info.bmiHeader.biWidth = w;
    bitmap_info.bmiHeader.biHeight = h;
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;
    bitmap_info.bmiHeader.biSizeImage = w * h;
    bitmap_info.bmiHeader.biXPelsPerMeter = 0;
    bitmap_info.bmiHeader.biYPelsPerMeter = 0;
    bitmap_info.bmiHeader.biClrUsed = 0;
    bitmap_info.bmiHeader.biClrImportant = 0;

    HBITMAP bitmap = ::CreateDIBSection(hdc, &bitmap_info, DIB_RGB_COLORS, (void**)bits, 0, 0);
    return bitmap;
}
HBITMAP CreateRGB32Bitmap(HDC hdc, int w, int h)
{
    return ::CreateCompatibleBitmap(hdc, w, h);
}

QuiBitmap * QuiBitmap::MakeGDIBitmap(int width, int height, QuiBitmapColorType color_type, QuiWindowHandle window_handle/* = nullptr*/)
{
    QuiBitmap* bitmap = new QuiBitmap();

    HWND hwnd = (HWND)window_handle;
    if (!hwnd) {
        hwnd = ::GetDesktopWindow();
    }
    HDC window_hdc = ::GetDC(hwnd);

    QuiRealGDIBitmap * real_bitmap = new QuiRealGDIBitmap(width, height, color_type, window_hdc);
    bitmap->real_bitmap_ = real_bitmap;

    ::ReleaseDC(hwnd, window_hdc);
    return bitmap;
}

QuiRealGDIBitmap::QuiRealGDIBitmap(int width, int height, QuiBitmapColorType color_type, HDC window_hdc)
    : width_(width)
    , height_(height)
    , color_type_(color_type)
    , hdc_(nullptr)
    , bits_(nullptr)
{
    hdc_ = ::CreateCompatibleDC(window_hdc);
    if (color_type_ == kARGB8888_BitmapColor) {
        hbitmap_ = CreateARGB32Bitmap(window_hdc, width, height, &bits_);
    }
    else {
        hbitmap_ = CreateRGB32Bitmap(window_hdc, width, height);
    }
}
QuiRealGDIBitmap::~QuiRealGDIBitmap()
{
    if (hdc_) {
        ::DeleteDC(hdc_);
    }
    if (hbitmap_) {
        ::DeleteObject(hbitmap_);
    }
}

void QuiRealGDIBitmap::BeginDraw()
{
    old_hbitmap_ = (HBITMAP)::SelectObject(hdc_, hbitmap_);
}
void QuiRealGDIBitmap::EndDraw()
{
    ::SelectObject(hdc_, old_hbitmap_);
}

