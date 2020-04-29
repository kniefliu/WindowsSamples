#ifndef DUI_CORE_GDI_RENDER_ENGINE_H_
#define DUI_CORE_GDI_RENDER_ENGINE_H_

#include <windows.h>

class GdiRenderEngine {
public:
    static void DrawLine(HDC hdc, const RECT& area, int line_width, DuiColor line_color, int line_style = PS_SOLID);
};

#endif//DUI_CORE_GDI_RENDER_ENGINE_H_
