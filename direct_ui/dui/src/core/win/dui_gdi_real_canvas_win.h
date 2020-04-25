#ifndef DUI_GDI_REAL_CANVAS_WIN_H_
#define DUI_GDI_REAL_CANVAS_WIN_H_

#include "core/dui_real_canvas.h"

#include <Windows.h>

#include <stack>

/*
 * GDI HBITMAP and HDC wrapper
 * ARGB32 or RGB32
 */

enum GdiSurfaceType {
    kRGB32_GdiSurface,
    kARGB32_GdiSurface,
};

class GdiSurface final {
public:
    GdiSurface(HDC hdc, int w, int h, GdiSurfaceType type = kRGB32_GdiSurface);
    ~GdiSurface();

    GdiSurfaceType GetType() const { return type_; }
    int GetWidth() const { return width_; }
    int GetHeight() const { return height_; }
    HDC SelectDC();
    void UnselectDC();
    HBITMAP GetHBitmap() const { return hbitmap_; }
    COLORREF* GetPixels() const { return hbitmap_pixels_; }

    void Clear(const DuiRect& area, DuiColor color = DUI_ColorTRANSPARENT);
    void ClearAlpha(const DuiRect& area, DuiAlpha alpha = 255);

private:
    GdiSurfaceType type_;
    int width_;
    int height_;
    HDC hdc_;
    HBITMAP hbitmap_;
    COLORREF* hbitmap_pixels_;
    HBITMAP old_hbitmap_;
};

struct GdiRenderClip;
class DuiGdiRealCanvas final : public DuiRealCanvas {
public:
    static HBITMAP CreateARGB32Bitmap(HDC hdc, int width, int height, COLORREF** bits);

public:
    DuiGdiRealCanvas(DuiCanvas * owner, HDC hdc, bool argb = false);

    HDC GetHDC() { return hdc_; }

private:
    void OnSave() override;
    void OnRestore() override;
    void OnClip(const DuiRect& clip_area) override;
    void OnDrawLine(const DuiPoint start, DuiPoint end, int line_size, DuiColor line_color, DuiPenStyle line_style) override;
    void OnDrawRect(const DuiRect& rc, int line_size, DuiColor line_color, DuiPenStyle pen_style) override;
    void OnDrawColor(const DuiRect& rc, DuiColor color) override;
    void DrawBitmap(DuiRealBitmap * bitmap, const DuiRect& dest, const DuiRect* src) override;
private:
    HDC hdc_;
    bool argb_ : 1;
    uint32_t save_count;
    std::stack<GdiRenderClip*> clip_stacks_;

    friend class AutoGdiRealCanvas;
};

class AutoGdiRealCanvas final {
public:
    AutoGdiRealCanvas(HDC hdc, bool argb);
    ~AutoGdiRealCanvas();
    DuiCanvas * GetCanvas();

private:
    DuiCanvas * canvas_;
};

#endif//DUI_GDI_REAL_CANVAS_WIN_H_
