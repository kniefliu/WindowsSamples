#ifndef DUI_CORE_CANVAS_H_
#define DUI_CORE_CANVAS_H_

#include "dui/core/dui_config.h"
#include "dui/core/dui_color.h"
#include "dui/core/dui_rect.h"
#include "dui/core/dui_bitmap.h"

enum DuiPenStyle {
    kSolid_PenStyle,            
    kDash_PenStyle,             /* -------  */
    kDot_PenStyle,              /* .......  */
    kDashdot_PenStyle,          /* _._._._  */
    kDashdotdot_PenStyle,       /* _.._.._  */
    kNull_PenStyle,
    kInsideframe_PenStyle,
    kUserstyle_PenStyle,
    kAlternate_PenStyle,
};

class DuiRealCanvas;
class DUI_API DuiCanvas final {
public:
    DuiCanvas();

public:
    void Save();
    void Restore();
    void Clip(const DuiRect& area);
    void DrawLine(const DuiPoint start, DuiPoint end, int line_size, DuiColor line_color, DuiPenStyle pen_style);
    void DrawRect(const DuiRect& rc, int line_size, DuiColor line_color, DuiPenStyle pen_style);
    void DrawColor(const DuiRect& rc, DuiColor color);
    void DrawBitmap(DuiBitmap * bitmap, const DuiRect& dest, const DuiRect* src = nullptr);

private:
    DuiRealCanvas * real_canvas_;
    
private:
    DuiCanvas(const DuiCanvas&) = delete;
    DuiCanvas& operator=(const DuiCanvas&) = delete;

    friend class DuiRealCanvas;
    friend class DuiCanvasPriv;
};

#endif//DUI_CORE_CANVAS_H_
