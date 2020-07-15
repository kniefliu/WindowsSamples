#pragma once

#include "qui/core/qui_bitmap.h"

#include <Windows.h>

class QuiRealGDIBitmap : public QuiBitmapBase {
public:
    QuiRealGDIBitmap(int width, int height, QuiBitmapColorType color_type, HDC hdc);
    ~QuiRealGDIBitmap() override;

public:
    int GetWidth() override { return width_; }
    int GetHeight() override { return height_; }
    QuiBitmapColorType GetColorType() override { return color_type_; }
    void BeginDraw() override;
    void EndDraw() override;

private:
    int width_;
    int height_;
    QuiBitmapColorType color_type_;
    HDC hdc_;
    HBITMAP hbitmap_;
    COLORREF* bits_;
    HBITMAP old_hbitmap_;

private:
    friend class QuiRealGDICanvas;
};
