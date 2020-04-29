#include "dui_gdi_real_canvas_win.h"

#include "dui_gdi_real_bitmap_win.h"

#include <crtdbg.h>

#include <sstream>

using AlphaBlendFunction = BOOL(WINAPI * )(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION);

static COLORREF PixelAlpha(COLORREF src, double src_darken, COLORREF dest, double dest_darken)
{
    return RGB(GetRValue(src) * src_darken + GetRValue(dest) * dest_darken,
        GetGValue(src) * src_darken + GetGValue(dest) * dest_darken,
        GetBValue(src) * src_darken + GetBValue(dest) * dest_darken);
}
static BOOL WINAPI AlphaBitBlt(HDC hdc, int dest_x, int dest_y, int width, int height, HDC src_hdc, int src_x, int src_y, int src_w, int src_h, BLENDFUNCTION blend_fun)
{
    HDC temp_hdc = ::CreateCompatibleDC(hdc);
    if (!temp_hdc) {
        return FALSE;
    }
    BITMAPINFO src_bitmap_info;
    ::ZeroMemory(&src_bitmap_info, sizeof(BITMAPINFO));
    src_bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    src_bitmap_info.bmiHeader.biWidth = width;
    src_bitmap_info.bmiHeader.biHeight = height;
    src_bitmap_info.bmiHeader.biPlanes = 1;
    src_bitmap_info.bmiHeader.biBitCount = 32;
    src_bitmap_info.bmiHeader.biCompression = BI_RGB;
    src_bitmap_info.bmiHeader.biSizeImage = width * height;
    src_bitmap_info.bmiHeader.biXPelsPerMeter = 0;
    src_bitmap_info.bmiHeader.biYPelsPerMeter = 0;
    src_bitmap_info.bmiHeader.biClrUsed = 0;
    src_bitmap_info.bmiHeader.biClrImportant = 0;

    COLORREF * src_bits = nullptr;
    HBITMAP src_hbitmap = ::CreateDIBSection(src_hdc, &src_bitmap_info, DIB_RGB_COLORS, (void**)&src_bits, 0, 0);
    if (!src_hbitmap || !src_bits) {
        ::DeleteDC(temp_hdc);
        return FALSE;
    }

    HBITMAP old_temp_hbitmap = (HBITMAP)::SelectObject(temp_hdc, src_hbitmap);
    ::StretchBlt(temp_hdc, 0, 0, width, height, src_hdc, src_x, src_y, src_w, src_h, SRCCOPY);
    ::SelectObject(temp_hdc, old_temp_hbitmap);

    BITMAPINFO dest_bitmap_info;
    ::ZeroMemory(&dest_bitmap_info, sizeof(BITMAPINFO));
    dest_bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    dest_bitmap_info.bmiHeader.biWidth = width;
    dest_bitmap_info.bmiHeader.biHeight = height;
    dest_bitmap_info.bmiHeader.biPlanes = 1;
    dest_bitmap_info.bmiHeader.biBitCount = 32;
    dest_bitmap_info.bmiHeader.biCompression = BI_RGB;
    dest_bitmap_info.bmiHeader.biSizeImage = width * height;
    dest_bitmap_info.bmiHeader.biXPelsPerMeter = 0;
    dest_bitmap_info.bmiHeader.biYPelsPerMeter = 0;
    dest_bitmap_info.bmiHeader.biClrUsed = 0;
    dest_bitmap_info.bmiHeader.biClrImportant = 0;

    COLORREF * dest_bits = nullptr;
    HBITMAP dest_hbitmap = ::CreateDIBSection(hdc, &dest_bitmap_info, DIB_RGB_COLORS, (void **)&dest_bits, 0, 0);
    if (!dest_hbitmap || !dest_bits) {
        ::DeleteObject(src_hbitmap);
        ::DeleteDC(temp_hdc);
        return FALSE;
    }

    ::SelectObject(temp_hdc, dest_hbitmap);
    ::BitBlt(temp_hdc, 0, 0, width, height, hdc, dest_x, dest_y, SRCCOPY);
    ::SelectObject(temp_hdc, old_temp_hbitmap);

    double src_darken;
    BYTE alpha;
    for (int pixel = 0; pixel < width * height; pixel++, src_bits++, dest_bits++) {
        alpha = LOBYTE(*src_bits >> 24);
        src_darken = (double)(alpha * blend_fun.SourceConstantAlpha) / 255.0 / 255.0;
        if (src_darken < 0.0) {
            src_darken = 0.0;
        }
        *dest_bits = PixelAlpha(*src_bits, src_darken, *dest_bits, 1.0 - src_darken);
    }

    ::SelectObject(temp_hdc, dest_hbitmap);
    ::BitBlt(hdc, dest_x, dest_y, width, height, temp_hdc, 0, 0, SRCCOPY);
    ::SelectObject(temp_hdc, old_temp_hbitmap);

    ::DeleteObject(dest_hbitmap);
    ::DeleteObject(src_hbitmap);
    ::DeleteDC(temp_hdc);
    return TRUE;
}

static void GdiBlt(HDC hdc, const DuiRect& dest, HDC src_hdc, const DuiRect& src, bool is_alpha, BLENDFUNCTION blend_func)
{
    if (is_alpha) {
        static AlphaBlendFunction alpha_blend = nullptr;
        if (!alpha_blend) {
            HMODULE hmodule = ::LoadLibraryW(L"msimg32.dll");
            if (hmodule) {
                alpha_blend = (AlphaBlendFunction)::GetProcAddress(hmodule, "AlphaBlend");
                //::FreeLibrary(hmodule);
            }
        }
        if (!alpha_blend) {
            alpha_blend = AlphaBitBlt;
        }
        alpha_blend(hdc, dest.x(), dest.y(), dest.width(), dest.height(), src_hdc, src.x(), src.y(), src.width(), src.height(), blend_func);
    }
    else {
        if (dest.width() == src.width() && dest.height() == src.height()) {
            ::BitBlt(hdc, dest.x(), dest.y(), dest.width(), dest.height(), src_hdc, src.x(), src.y(), SRCCOPY);
        }
        else {
            ::StretchBlt(hdc, dest.x(), dest.y(), dest.width(), dest.height(), src_hdc, src.x(), src.y(), src.width(), src.height(), SRCCOPY);
        }
    }
}

static void DrawImage(HDC hdc, HBITMAP hbitmap, const RECT& rc, const RECT& rc_paint,
    const RECT& rc_bmp_part, const RECT& rc_scale9, bool is_alpha,
    DuiAlpha fade = 255, bool is_hole = false, bool is_tiled_x = false, bool is_tiled_y = false)
{
    _ASSERT(::GetObjectType(hdc) == OBJ_DC || ::GetObjectType(hdc) == OBJ_MEMDC);

    if (!hbitmap) {
        return;
    }

    HDC clone_hdc = ::CreateCompatibleDC(hdc);
    HBITMAP old_hbitmap = (HBITMAP)::SelectObject(clone_hdc, hbitmap);
    ::SetStretchBltMode(hdc, COLORONCOLOR);

    RECT temp_rc = { 0 };
    RECT dest_rc = { 0 };

    BLENDFUNCTION blend_func = { AC_SRC_OVER, 0, fade, AC_SRC_ALPHA };
    is_alpha = (is_alpha || fade < 255);
    {
        // middle
        if (!is_hole) {
            dest_rc.left = rc.left + rc_scale9.left;
            dest_rc.top = rc.top + rc_scale9.top;
            dest_rc.right = rc.right - rc.left - rc_scale9.left - rc_scale9.right;
            dest_rc.bottom = rc.bottom - rc.top - rc_scale9.top - rc_scale9.bottom;
            dest_rc.right += dest_rc.left;
            dest_rc.bottom += dest_rc.top;
            if (::IntersectRect(&temp_rc, &rc_paint, &dest_rc)) {
                if (!is_tiled_x && !is_tiled_y) {
                    DuiRect tmp_dest = { dest_rc.left, dest_rc.top, dest_rc.right, dest_rc.bottom };
                    DuiRect tmp_src = { rc_bmp_part.left + rc_scale9.left, rc_bmp_part.top + rc_scale9.top, rc_bmp_part.right - rc_scale9.right, rc_bmp_part.bottom - rc_scale9.bottom };
                    GdiBlt(hdc, tmp_dest, clone_hdc, tmp_src, is_alpha, blend_func);                    
                }
                else if (is_tiled_x && is_tiled_y) {
                    int src_center_x = rc_bmp_part.left + rc_scale9.left;
                    int src_center_y = rc_bmp_part.top + rc_scale9.top;
                    int src_center_w = rc_bmp_part.right - rc_bmp_part.left - rc_scale9.left - rc_scale9.right;
                    int src_center_h = rc_bmp_part.bottom - rc_bmp_part.top - rc_scale9.bottom - rc_scale9.top;
                    int hori_times = (dest_rc.right - dest_rc.left + src_center_w - 1) / src_center_w;
                    int vert_times = (dest_rc.bottom - dest_rc.top + src_center_h - 1) / src_center_h;
                    for (int j = 0; j < vert_times; ++j) {
                        int dest_top = dest_rc.top + src_center_h * j;
                        int dest_bottom = dest_rc.top + src_center_h * (j + 1);
                        int draw_h = src_center_h;
                        if (dest_bottom > dest_rc.bottom) {
                            draw_h -= (dest_bottom - dest_rc.bottom);
                            dest_bottom = dest_rc.bottom;
                        }
                        for (int i = 0; i < hori_times; ++i) {
                            int dest_left = dest_rc.left + src_center_w * i;
                            int dest_right = dest_rc.left + src_center_w * (i + 1);
                            int draw_w = src_center_w;
                            if (dest_right > dest_rc.right) {
                                draw_w -= (dest_right - dest_rc.right);
                                dest_right = dest_rc.right;
                            }
                            DuiRect tmp_dest = { dest_left, dest_top, dest_right, dest_bottom };
                            DuiRect tmp_src = { src_center_x, src_center_y, src_center_x + draw_w, src_center_y + draw_h };
                            GdiBlt(hdc, tmp_dest, clone_hdc, tmp_src, is_alpha, blend_func);
                        }
                    }
                }
                else if (is_tiled_x) {
                    int src_center_x = rc_bmp_part.left + rc_scale9.left;
                    int src_center_y = rc_bmp_part.top + rc_scale9.top;
                    int src_center_w = rc_bmp_part.right - rc_bmp_part.left - rc_scale9.right - rc_scale9.left;
                    int src_center_h = rc_bmp_part.bottom - rc_bmp_part.top - rc_scale9.bottom - rc_scale9.top;
                    int hori_times = (dest_rc.right - dest_rc.left + src_center_w - 1) / src_center_w;
                    for (int i = 0; i < hori_times; ++i) {
                        int dest_left = dest_rc.left + src_center_w * i;
                        int dest_right = dest_rc.left + src_center_w * (i + 1);
                        int draw_w = src_center_w;
                        if (dest_right > dest_rc.right) {
                            draw_w -= (dest_right - dest_rc.right);
                            dest_right = dest_rc.right;
                        }
                        int dest_h = dest_rc.right - dest_rc.left;
                        DuiRect tmp_dest = { dest_left, dest_rc.top, dest_right, dest_rc.bottom };
                        DuiRect tmp_src = { src_center_x, src_center_y, src_center_x + draw_w, src_center_y + src_center_h };
                        GdiBlt(hdc, tmp_dest, clone_hdc, tmp_src, is_alpha, blend_func);
                    }
                }
            }
        }
    }

    ::SelectObject(clone_hdc, old_hbitmap);
    ::DeleteDC(clone_hdc);
}

GdiSurface::GdiSurface(HDC hdc, int w, int h, GdiSurfaceType type/* = kRGB32_GdiSurface*/)
    : type_(type)
    , width_(w)
    , height_(h)
    , hdc_(nullptr)
    , hbitmap_(nullptr)
    , hbitmap_pixels_(nullptr)
    , old_hbitmap_(nullptr)
{
    hdc_ = ::CreateCompatibleDC(hdc);
    if (type == kARGB32_GdiSurface) {
        hbitmap_ = DuiGdiRealCanvas::CreateARGB32Bitmap(hdc, w, h, &hbitmap_pixels_);
        ::ZeroMemory(hbitmap_pixels_, width_ * height_ * 4);
    }
    else {
        hbitmap_ = ::CreateCompatibleBitmap(hdc, w, h);
    }
}
GdiSurface::~GdiSurface()
{
    if (hbitmap_) {
        ::DeleteObject(hbitmap_);
        hbitmap_ = nullptr;
    }
    if (hdc_) {
        ::DeleteDC(hdc_);
        hdc_ = nullptr;
    }
}
HDC GdiSurface::SelectDC()
{
    if (old_hbitmap_) {
        return nullptr;
    }
    old_hbitmap_ = (HBITMAP)::SelectObject(hdc_, hbitmap_);
    return hdc_;
}
void GdiSurface::UnselectDC()
{
    if (!old_hbitmap_) {
        return;
    }
    ::SelectObject(hdc_, old_hbitmap_);
    old_hbitmap_ = nullptr;
}
void GdiSurface::Clear(const DuiRect& area, DuiColor color/* = DUI_ColorTRANSPARENT*/)
{
    COLORREF * pixel = nullptr;
    for (int y = area.top; y < area.bottom; ++y) {
        for (int x = area.left; x < area.right; ++x) {
            pixel = hbitmap_pixels_ + (height_ - y - 1) * width_ + x;
            DuiColor old_color = *pixel;
            *pixel = color;
        }
    }
}
void GdiSurface::ClearAlpha(const DuiRect& area, DuiAlpha alpha/* = 255*/)
{
    COLORREF * pixel = nullptr;
    for (int y = area.top; y < area.bottom; ++y) {
        for (int x = area.left; x < area.right; ++x) {
            pixel = hbitmap_pixels_ + (height_ - y - 1) * width_ + x;
            DuiColor old_color = *pixel;
            DuiColor color = DuiColorSetARGB(alpha, DuiColorGetR(old_color), DuiColorGetG(old_color), DuiColorGetB(old_color));
            *pixel = color;
        }
    }
}

HBITMAP DuiGdiRealCanvas::CreateARGB32Bitmap(HDC hdc, int width, int height, COLORREF** bits)
{
    BITMAPINFO bitmap_info = { 0 };
    bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmap_info.bmiHeader.biWidth = width;
    bitmap_info.bmiHeader.biHeight = height;
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;
    bitmap_info.bmiHeader.biSizeImage = width * height;

    HBITMAP hbitmap = ::CreateDIBSection(hdc, &bitmap_info, DIB_RGB_COLORS, (void**)bits, 0, 0);
    return hbitmap;
}

DuiGdiRealCanvas::DuiGdiRealCanvas(DuiCanvas * owner, HDC hdc, bool argb/* = false*/)
    : DuiRealCanvas(owner)
    , hdc_(hdc)
    , argb_(argb)
    , save_count(0)
{

}

struct GdiRenderClip final {
    RECT expected_clip_area;
    HDC hdc;
    HRGN hrgn;
    HRGN old_hrgn;
    uint32_t save_count;

    ~GdiRenderClip()
    {
        _ASSERT(::GetObjectType(hdc) == OBJ_DC || ::GetObjectType(hdc) == OBJ_MEMDC);
        _ASSERT(::GetObjectType(hrgn) == OBJ_REGION);
        _ASSERT(::GetObjectType(old_hrgn) == OBJ_REGION);
        ::SelectClipRgn(hdc, old_hrgn);
        ::DeleteObject(old_hrgn);
        ::DeleteObject(hrgn);
    }

    static GdiRenderClip* MakeClip(HDC hdc, RECT expected_clip_area, uint32_t save_count)
    {
        GdiRenderClip * clip_ptr = new GdiRenderClip();
        GdiRenderClip& clip = *clip_ptr;
        RECT old_clip_area = { 0 };
        ::GetClipBox(hdc, &old_clip_area);
        clip.old_hrgn = ::CreateRectRgnIndirect(&old_clip_area);
        clip.hrgn = ::CreateRectRgnIndirect(&expected_clip_area);
        ::CombineRgn(clip.hrgn, clip.hrgn, clip.old_hrgn, RGN_AND);
        ::SelectClipRgn(hdc, clip.hrgn);
        clip.hdc = hdc;
        clip.expected_clip_area = expected_clip_area;
        clip.save_count = save_count;
        return clip_ptr;
    }
};

void DuiGdiRealCanvas::OnSave()
{
    save_count++;
}
void DuiGdiRealCanvas::OnRestore()
{
    // clipstack
    while (clip_stacks_.size() != 0 && clip_stacks_.top()) {
        GdiRenderClip * top_clip = clip_stacks_.top();
        if (top_clip->save_count == save_count) {
            clip_stacks_.pop();
            delete top_clip;
        }
        else {
            break;
        }
    }

    save_count--;
}
void DuiGdiRealCanvas::OnClip(const DuiRect& clip_area)
{
    RECT expected_clip_area = { clip_area.left, clip_area.top, clip_area.right, clip_area.bottom };

    clip_stacks_.push(::GdiRenderClip::MakeClip(hdc_, expected_clip_area, save_count));
}
void DuiGdiRealCanvas::OnDrawLine(const DuiPoint start, DuiPoint stop, int line_size, DuiColor line_color, DuiPenStyle line_style)
{
    LOGPEN lg;
    lg.lopnColor = RGB(DuiColorGetR(line_color), DuiColorGetG(line_color), DuiColorGetB(line_color));
    lg.lopnStyle = line_style;
    lg.lopnWidth.x = line_size;
    HPEN pen = ::CreatePenIndirect(&lg);
    HPEN old_pen = (HPEN)::SelectObject(hdc_, pen);
    POINT pt_temp = { 0 };
    ::MoveToEx(hdc_, start.x, start.y, &pt_temp);
    ::LineTo(hdc_, stop.x, stop.y);
    ::SelectObject(hdc_, old_pen);
    ::DeleteObject(pen);
}
void DuiGdiRealCanvas::OnDrawRect(const DuiRect& rc, int line_size, DuiColor line_color, DuiPenStyle pen_style)
{
    HPEN pen = ::CreatePen(pen_style | PS_INSIDEFRAME, line_size, RGB(DuiColorGetR(line_color), DuiColorGetG(line_color), DuiColorGetB(line_color)));
    HPEN old_pen = (HPEN)::SelectObject(hdc_, pen);
    ::SelectObject(hdc_, ::GetStockObject(HOLLOW_BRUSH));
    ::Rectangle(hdc_, rc.left, rc.top, rc.right, rc.bottom);
    ::SelectObject(hdc_, old_pen);
    ::DeleteObject(pen);
}
static inline DuiColor DuiColorSetABGRInline(DuiColor8BitComponent a, DuiColor8BitComponent b, DuiColor8BitComponent g, DuiColor8BitComponent r)
{
    return ((a << 24) | (b << 16) | (g << 8) | (r << 0));
}
void DuiGdiRealCanvas::OnDrawColor(const DuiRect& rc, DuiColor color)
{
    if (DuiColorGetA(color) == 0) {
        return;
    }

    RECT dest = { rc.left, rc.top, rc.right, rc.bottom };
    if (!argb_) {
        ::SetBkColor(hdc_, RGB(DuiColorGetR(color), DuiColorGetG(color), DuiColorGetB(color)));
        ::ExtTextOut(hdc_, 0, 0, ETO_OPAQUE, &dest, 0, 0, 0);
    }
    else {
        BITMAPINFO bitmap_info = { 0 };
        bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bitmap_info.bmiHeader.biWidth = 1;
        bitmap_info.bmiHeader.biHeight = 1;
        bitmap_info.bmiHeader.biPlanes = 1;
        bitmap_info.bmiHeader.biBitCount = 32;
        bitmap_info.bmiHeader.biCompression = BI_RGB;
        bitmap_info.bmiHeader.biSizeImage = 1 * 1 * sizeof(DWORD);
        LPDWORD pixel_bits = nullptr;
        HBITMAP hbitmap = ::CreateDIBSection(hdc_, &bitmap_info, DIB_RGB_COLORS, (LPVOID*)&pixel_bits, 0, 0);
        if (!hbitmap) {
            return;
        }

        //ABGR
        DuiColor abgr_color = color;// DuiColorSetABGRInline(DuiColorGetA(color), DuiColorGetB(color), DuiColorGetG(color), DuiColorGetR(color));
        *pixel_bits = abgr_color;

        RECT bmppart = { 0, 0, 1, 1 };
        RECT bmpcorner = { 0 };
        ::DrawImage(hdc_, hbitmap, dest, dest, bmppart, bmpcorner, true, 255);
        ::DeleteObject(hbitmap);
    }
}
void DuiGdiRealCanvas::DrawBitmap(DuiRealBitmap * bitmap, const DuiRect& dest, const DuiRect* src)
{
    GdiRealBitmap * gdi_bitmap = static_cast<GdiRealBitmap*>(bitmap);
    RECT bmppart = { 0, 0, gdi_bitmap->width_, gdi_bitmap->height_ };
    if (src) {
        bmppart = { src->left, src->top, src->right, src->bottom };
    }
    RECT bmpcorner = { 0 };
    RECT dest_area = { dest.left, dest.top, dest.right, dest.bottom };
    ::DrawImage(hdc_, gdi_bitmap->hbitmap_, dest_area, dest_area, bmppart, bmpcorner, true, 255);
}

AutoGdiRealCanvas::AutoGdiRealCanvas(HDC hdc, bool argb)
{
    canvas_ = (new DuiGdiRealCanvas(new DuiCanvas(), hdc, argb))->owner_;
    
}
AutoGdiRealCanvas::~AutoGdiRealCanvas()
{
    delete canvas_;
}
DuiCanvas * AutoGdiRealCanvas::GetCanvas()
{
    return canvas_;
}
