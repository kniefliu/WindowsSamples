#ifndef DUI_CORE_GDI_REAL_BITMAP_WIN_H_
#define DUI_CORE_GDI_REAL_BITMAP_WIN_H_

#include "core/dui_real_bitmap.h"

class GdiRealBitmap : public DuiRealBitmap {
public:
    GdiRealBitmap(DuiBitmap * owner, int width, int height);
    ~GdiRealBitmap() override;

    void Clear(DuiColor color) override;

public:
    HBITMAP hbitmap_;
    int width_;
    int height_;
    COLORREF * hbitmap_pixels_;
};

#endif//DUI_CORE_GDI_REAL_BITMAP_WIN_H_
