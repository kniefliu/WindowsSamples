#include "dui_tooltip_window_win.h"

#include <Windows.h>
#include <CommCtrl.h>

DuiRealWindow* DuiRealWindow::MakeRealTooltipWindow(DuiTooltipWindow * owner)
{
    return new TooltipWindowWin(owner);
}

TooltipWindowWin::TooltipWindowWin(DuiTooltipWindow * owner)
    : DuiRealWindowWin(owner)
    , tooltip_width_(0)
    , added_(false)
{

}

void TooltipWindowWin::OnInit()
{
    hwnd_ = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
        GetHostWindowHandle(), NULL, GetInstance(), NULL);
}

int TooltipWindowWin::OnSetTooltipWidth(int width)
{
    if (hwnd_ != NULL && tooltip_width_ >= 0) {
        tooltip_width_ = width;
        (int)::SendMessage(hwnd_, TTM_SETMAXTIPWIDTH, 0, tooltip_width_);
        return tooltip_width_;
    }
    return width;
}
void TooltipWindowWin::OnShowTooltip()
{
    ResetToolInfo();
    if (!added_) {
        ::SendMessage(hwnd_, TTM_ADDTOOL, 0, (LPARAM)&tooltip_);
        added_ = TRUE;
    }
    int width = ::SendMessage(hwnd_, TTM_SETMAXTIPWIDTH, 0, tooltip_width_);
    ::SendMessage(hwnd_, TTM_SETTOOLINFO, 0, (LPARAM)&tooltip_);
    ::SendMessage(hwnd_, TTM_TRACKACTIVATE, TRUE, (LPARAM)&tooltip_);
}
void TooltipWindowWin::OnHideTooltip()
{
    ::SendMessage(hwnd_, TTM_TRACKACTIVATE, FALSE, (LPARAM)&tooltip_);
}
void TooltipWindowWin::OnMoveTooltip(const DuiPoint& loc)
{
    ::SendMessage(hwnd_, TTM_TRACKPOSITION, 0, (LPARAM)(DWORD)MAKELONG(loc.x, loc.y));
}
int TooltipWindowWin::OnGetTooltipTime()
{
    return ::SendMessage(hwnd_, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
}

void TooltipWindowWin::ResetToolInfo()
{
    ::ZeroMemory(&tooltip_, sizeof(TOOLINFO));
    tooltip_.cbSize = sizeof(TOOLINFO);
    tooltip_.cbSize -= sizeof(void*);
    tooltip_.uFlags = TTF_IDISHWND;
    tooltip_.hwnd = GetHostWindowHandle();
    tooltip_.uId = (UINT_PTR)GetHostWindowHandle();
    tooltip_.hinst = GetInstance();
    tooltip_.lpszText = const_cast<LPTSTR>((LPCTSTR)GetTipText());
    tooltip_.rect = GetTipArea();
}

bool TooltipWindowWin::HandleEvent(const DuiWindowEvent * evt, DuiWindowEventResult& result)
{
    return false;
}

bool TooltipWindowWin::OnGetWindowStyle(DWORD& style) const
{
    style = WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP;
    return true;
}
bool TooltipWindowWin::OnGetWindowExStyle(DWORD& ex_style) const
{
    ex_style = WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
    return true;
}

const wchar_t * TooltipWindowWin::GetWindowClassName()
{
    return L"TooltipWindowWin";
}
const wchar_t * TooltipWindowWin::GetSuperClassName()
{
    return TOOLTIPS_CLASS;
}
LRESULT TooltipWindowWin::HandleMessage(UINT msg, WPARAM w_param, LPARAM l_param)
{
    return ::CallWindowProc(old_window_proc_, hwnd_, msg, w_param, l_param);
}
void TooltipWindowWin::OnFinalMessage(HWND hwnd)
{
    DuiRealWindowWin::OnFinalMessage(hwnd);
}

DuiTooltipWindow * TooltipWindowWin::Owner() const
{
    return static_cast<DuiTooltipWindow*>(owner_);
}
DuiTooltipWindow * TooltipWindowWin::Owner()
{
    return static_cast<DuiTooltipWindow*>(owner_);
}
DuiTooltipClient * TooltipWindowWin::GetTootipClient() const
{
    return Owner()->GetClient();
}

DuiWindow* TooltipWindowWin::GetHostWindow() const
{
    if (GetTootipClient()) {
        return GetTootipClient()->GetHostWindow();
    }
    return nullptr;
}
HWND TooltipWindowWin::GetHostWindowHandle() const
{
    if (GetHostWindow()) {
        return (HWND)(GetHostWindow()->GetHandle());
    }
    return ::GetParent(hwnd_);
}
LPCTSTR TooltipWindowWin::GetTipText() const
{
    return L"TooltipWindowWin";
}
RECT TooltipWindowWin::GetTipArea() const
{
    RECT tip_area;
    GetClientRect(GetHostWindowHandle(), &tip_area);
    return tip_area;
}