#pragma once

#include "qui/core/qui_canvas.h"

#include <Windows.h>

class QuiRealGDICanvas : public QuiCanvasBase {
public:
    explicit QuiRealGDICanvas(HDC hdc);
    explicit QuiRealGDICanvas(HWND hwnd);
    ~QuiRealGDICanvas() override;

public:
    int Save() override;
    void Restore(int save_id) override;
    void DrawColor(const QuiRect& rect, QuiColor color) override;

private:
    HDC hdc_;
    HWND hwnd_;
};
