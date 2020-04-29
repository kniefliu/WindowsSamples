#include "gdi_render_engine.h"

#include <crtdbg.h>

#include "dui/core/dui_color.h"

void GdiRenderEngine::DrawLine(HDC hdc, const RECT& area, int line_width, DuiColor line_color, int line_style)
{
    _ASSERT(::GetObjectType(hdc) == OBJ_DC || ::GetObjectType(hdc) == OBJ_MEMDC);
    
    LOGPEN log_pen;
    log_pen.lopnColor = RGB(DuiColorGetR(line_color), DuiColorGetG(line_color), DuiColorGetB(line_color));
    log_pen.lopnStyle = line_style;
    log_pen.lopnWidth.x = line_width;
    HPEN hpen = ::CreatePenIndirect(&log_pen);
    HPEN old_hpen = (HPEN)::SelectObject(hdc, hpen);
    POINT temp_loc = { 0 };
    ::MoveToEx(hdc, area.left, area.top, &temp_loc);
    ::LineTo(hdc, area.right, area.bottom);
    ::SelectObject(hdc, old_hpen);
    ::DeleteObject(hpen);
}