#pragma once

#include "qui/core/qui_rect.h"

#include <Windows.h>

inline QuiRect Win32RECTToQuiRect(const RECT& win_rc)
{
    QuiRect ret = { win_rc.left, win_rc.top, win_rc.right, win_rc.bottom };
    return ret;
}
inline RECT QuiRectToWin32RECT(const QuiRect& qrc)
{
    RECT ret = { qrc.left, qrc.top, qrc.right, qrc.bottom };
    return ret;
}
