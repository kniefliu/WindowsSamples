#pragma once

#include "qui/core/qui_canvas.h"

#include <Windows.h>

class QuiRealGDIBitmap;
class QuiRealGDICanvas : public QuiCanvasBase {
public:
    explicit QuiRealGDICanvas(HDC hdc);
    explicit QuiRealGDICanvas(HWND hwnd);
    explicit QuiRealGDICanvas(QuiRealGDIBitmap* gdi_bitmap);
    ~QuiRealGDICanvas() override;

public:
    int Save() override;
    void Restore(int save_id) override;
    void DrawColor(const QuiRect& rect, QuiColor color) override;
    void DrawBitmap(QuiBitmap * bitmap, const QuiRect& src, const QuiRect& dest) override;

private:
    HDC hdc_;
    HWND hwnd_;
    QuiRealGDIBitmap* gdi_bitmap_;
};
