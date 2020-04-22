#include "StdAfx.h"

#include "dui_tooltip_window.h"

DuiTooltipWindow * DuiTooltipWindow::MakeTooltipWindow(HWND host, DuiTooltipWindowType type/* = kSystem_Tooltip*/)
{
    DuiTooltipWindow * window = nullptr;
    if (type == kSystem_Tooltip) {
        DuiSystemTooltipWindow * window_base = new DuiSystemTooltipWindow(host);
        window_base->tooltip_hwnd_ = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            host, NULL, DuiLib::CPaintManagerUI::GetInstance(), NULL);
        window = window_base;
    }
    else if (type == kSubclassSystem_Tooltip) {
        DuiSubclassSystemTooltipWindow * window_ex = new DuiSubclassSystemTooltipWindow(host);
        window_ex->Create(host, L"", WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, 0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT);
        window = window_ex;
    }
    else if (type == kCustom_Tooltip) {
        DuiCustomTooltipWindow * window_ex = new DuiCustomTooltipWindow(host);
        window_ex->Create(host, L"", WS_POPUP, WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_NOPARENTNOTIFY);
        window_ex->tooltip_hwnd_ = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            host, NULL, DuiLib::CPaintManagerUI::GetInstance(), NULL);
        window = window_ex;
    }
    return window;
}

DuiSystemTooltipWindow::DuiSystemTooltipWindow(HWND host)
    : host_(host)
    , tooltip_hwnd_(nullptr)
    , inited_(false)
{

}
DuiSystemTooltipWindow::~DuiSystemTooltipWindow()
{
    if (tooltip_hwnd_) {
        ::DestroyWindow(tooltip_hwnd_);
        tooltip_hwnd_ = nullptr;
    }
}

void DuiSystemTooltipWindow::ResetTooltipInfo(LPCTSTR tip_text, const RECT& tip_rect)
{
    ::ZeroMemory(&tooltip_info_, sizeof(TOOLINFO));
    tooltip_info_.cbSize = sizeof(TOOLINFO);
    tooltip_info_.uFlags = TTF_IDISHWND;
    tooltip_info_.hwnd = host_;
    tooltip_info_.uId = (UINT_PTR)host_;
    tooltip_info_.hinst = DuiLib::CPaintManagerUI::GetInstance();
    tooltip_info_.lpszText = const_cast<LPTSTR>((LPCTSTR)tip_text);
    tooltip_info_.rect = tip_rect;
}
int DuiSystemTooltipWindow::SetMaxTipWidth(int tip_width)
{
    if (!inited_) {
        ::SendMessage(tooltip_hwnd_, TTM_ADDTOOL, 0, (LPARAM)&tooltip_info_);
        inited_ = true;
    }
    return (int)::SendMessage(tooltip_hwnd_, TTM_SETMAXTIPWIDTH, 0, tip_width);
}
void DuiSystemTooltipWindow::ShowTip()
{
    if (!inited_) {
        ::SendMessage(tooltip_hwnd_, TTM_ADDTOOL, 0, (LPARAM)&tooltip_info_);
        inited_ = true;
    }
    ::SendMessage(tooltip_hwnd_, TTM_SETTOOLINFO, 0, (LPARAM)&tooltip_info_);
    ::SendMessage(tooltip_hwnd_, TTM_TRACKACTIVATE, TRUE, (LPARAM)&tooltip_info_);
}
void DuiSystemTooltipWindow::HideTip()
{
    ::SendMessage(tooltip_hwnd_, TTM_TRACKACTIVATE, FALSE, (LPARAM)&tooltip_info_);
}
void DuiSystemTooltipWindow::MoveTip(const POINT& loc)
{
    ::SendMessage(tooltip_hwnd_, TTM_TRACKPOSITION, 0, (LPARAM)(DWORD)MAKELONG(loc.x, loc.y));
}
DWORD DuiSystemTooltipWindow::GetDelayTime()
{
    return (DWORD)::SendMessage(tooltip_hwnd_, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
}
HWND DuiSystemTooltipWindow::GetTipHandle()
{
    return tooltip_hwnd_;
}


DuiSubclassSystemTooltipWindow::DuiSubclassSystemTooltipWindow(HWND host)
    : host_(host)
    , tip_label_(nullptr)
    , inited_(false)
    , painting_(false)
{

}
DuiSubclassSystemTooltipWindow::~DuiSubclassSystemTooltipWindow()
{
    if (m_hWnd && ::IsWindow(m_hWnd)) {
        ::DestroyWindow(m_hWnd);
        m_hWnd = nullptr;
    }
}

void DuiSubclassSystemTooltipWindow::ResetTooltipInfo(LPCTSTR tip_text, const RECT& tip_rect)
{
    ::ZeroMemory(&tooltip_info_, sizeof(TOOLINFO));
    tooltip_info_.cbSize = sizeof(TOOLINFO);
    tooltip_info_.uFlags = TTF_IDISHWND;
    tooltip_info_.hwnd = host_;
    tooltip_info_.uId = (UINT_PTR)host_;
    tooltip_info_.hinst = DuiLib::CPaintManagerUI::GetInstance();
    tooltip_info_.lpszText = const_cast<LPTSTR>((LPCTSTR)tip_text);
    tooltip_info_.rect = tip_rect;
    if (tip_label_) {
        tip_label_->SetText(L"Hello");
    }
}
int DuiSubclassSystemTooltipWindow::SetMaxTipWidth(int tip_width)
{
    if (!inited_) {
        tip_bkcolor_ = ::SendMessage(m_hWnd, TTM_GETTIPBKCOLOR, 0, 0);
        tip_textcolor_ = ::SendMessage(m_hWnd, TTM_GETTIPTEXTCOLOR, 0, 0);
        ::SendMessage(m_hWnd, TTM_ADDTOOL, 0, (LPARAM)&tooltip_info_);
        inited_ = true;
    }
    int width = (int)::SendMessage(m_hWnd, TTM_SETMAXTIPWIDTH, 0, tip_width);
    this->ResizeClient(tip_width + 10, 100);
    return width;
}
void DuiSubclassSystemTooltipWindow::ShowTip()
{
    if (!inited_) {
        tip_bkcolor_ = ::SendMessage(m_hWnd, TTM_GETTIPBKCOLOR, 0, 0);
        tip_textcolor_ = ::SendMessage(m_hWnd, TTM_GETTIPTEXTCOLOR, 0, 0);
        ::SendMessage(m_hWnd, TTM_ADDTOOL, 0, (LPARAM)&tooltip_info_);
        inited_ = true;
    }
    if (tip_label_) {
        tip_label_->SetBkColor(tip_bkcolor_);
        tip_label_->SetTextColor(tip_textcolor_);
    }
    ::SendMessage(m_hWnd, TTM_SETTOOLINFO, 0, (LPARAM)&tooltip_info_);
    ::SendMessage(m_hWnd, TTM_TRACKACTIVATE, TRUE, (LPARAM)&tooltip_info_);
    this->ResizeClient(300 + 10, 100);
}
void DuiSubclassSystemTooltipWindow::HideTip()
{
    ::SendMessage(m_hWnd, TTM_TRACKACTIVATE, FALSE, (LPARAM)&tooltip_info_);
}
void DuiSubclassSystemTooltipWindow::MoveTip(const POINT& loc)
{
    ::SendMessage(m_hWnd, TTM_TRACKPOSITION, 0, (LPARAM)(DWORD)MAKELONG(loc.x, loc.y));
}
DWORD DuiSubclassSystemTooltipWindow::GetDelayTime()
{
    return (DWORD)::SendMessage(m_hWnd, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
}
HWND DuiSubclassSystemTooltipWindow::GetTipHandle()
{
    return m_hWnd;
}

LPCTSTR DuiSubclassSystemTooltipWindow::GetWindowClassName() const
{
    return L"DuiSubclassSystemTooltipWindow";
}
LPCTSTR DuiSubclassSystemTooltipWindow::GetSuperClassName() const
{
    return TOOLTIPS_CLASS;
}
UINT DuiSubclassSystemTooltipWindow::GetClassStyle() const
{
    return CS_VREDRAW | CS_HREDRAW;
}

LRESULT DuiSubclassSystemTooltipWindow::HandleMessage(UINT msg, WPARAM w_param, LPARAM l_param)
{
    LRESULT l_res = 0;
    bool handled = false;
    switch (msg) {
    case WM_CREATE:
    {
#if USE_CUSTOM_TOOLTIP
        paint_manager_.SetTooltipEnabled(false);
#endif
        DuiLib::CContainerUI * con = new DuiLib::CContainerUI();
        DWORD tip_bkcolor = tip_bkcolor_;// RGB(GetBValue(tip_bkcolor_), GetGValue(tip_bkcolor_), GetRValue(tip_bkcolor_));
        DWORD tip_textcolor = tip_textcolor_;// RGB(GetBValue(tip_textcolor_), GetGValue(tip_textcolor_), GetRValue(tip_textcolor_));
        con->SetBkColor(tip_bkcolor);
        con->SetBorderRound({ 5, 5 });
        con->SetInset({ 5, 5, 5, 5 });
        tip_label_ = new DuiLib::CLabelUI();
        tip_label_->SetBkColor(tip_bkcolor);
        tip_label_->SetTextColor(tip_textcolor);
        con->Add(tip_label_);
        paint_manager_.SetNoActivate(true);
        paint_manager_.Init(m_hWnd);
        paint_manager_.SetLayered(false);
        paint_manager_.AttachDialog(con);
        break;
    }
    case WM_MOUSEHOVER:
    case WM_MOUSEMOVE:
    case WM_MOUSELEAVE:
        return 0;
        break;
    case TTM_ADJUSTRECT:
    {
        RECT * area = (RECT*)l_param;
        if (w_param == TRUE) {
            *area = { 0, 0, 300, 100 };
        }
        else {
            *area = { 0, 0, 300, 100 };
        }
        return 0;
        break;
    }
    }
    if (!handled) {
        if (msg != WM_PAINT) {
            handled = paint_manager_.MessageHandler(msg, w_param, l_param, l_res);
        }
        else {
            handled = paint_manager_.MessageHandler(msg, w_param, l_param, l_res);
        }
    }
    if (handled) {
        return l_res;
    }

    return CWindowWnd::HandleMessage(msg, w_param, l_param);
}
void DuiSubclassSystemTooltipWindow::OnFinalMessage(HWND hwnd)
{

}


DuiCustomTooltipWindow::DuiCustomTooltipWindow(HWND host)
    : host_(host)
    , tip_label_(nullptr)
    , inited_(false)
    , showing_(false)
    , hiding_(false)
    , moving_(false)
    , initial_delay_time_(500)
{

}
DuiCustomTooltipWindow::~DuiCustomTooltipWindow()
{
    if (m_hWnd && ::IsWindow(m_hWnd)) {
        ::DestroyWindow(m_hWnd);
        m_hWnd = nullptr;
    }
}

void DuiCustomTooltipWindow::ResetTooltipInfo(LPCTSTR tip_text, const RECT& tip_rect)
{
    if (tip_label_) {
        tip_label_->SetText(tip_text);
    }
    tip_rect_ = tip_rect;
}
int DuiCustomTooltipWindow::SetMaxTipWidth(int tip_width)
{
    tip_width_ = tip_width;
    //::SetWindowPos(m_hWnd, 0, 0, 0, tip_width, 30, NO_SHO);
    return (int)tip_width_;
}
void DuiCustomTooltipWindow::ShowTip()
{
    if (!inited_) {
        tip_bkcolor_ = ::SendMessage(tooltip_hwnd_, TTM_GETTIPBKCOLOR, 0, 0);
        tip_textcolor_ = ::SendMessage(tooltip_hwnd_, TTM_GETTIPTEXTCOLOR, 0, 0);
        auto_pop_delay_time_ = ::SendMessage(tooltip_hwnd_, TTM_GETDELAYTIME, TTDT_AUTOPOP, 0);
        initial_delay_time_ = ::SendMessage(tooltip_hwnd_, TTM_GETDELAYTIME, TTDT_INITIAL, 0);
        inited_ = true;
        ::DestroyWindow(tooltip_hwnd_);
        tooltip_hwnd_ = nullptr;
        //initial_delay_time_ = 2000;
        if (tip_label_) {
            tip_label_->SetBkColor(0xFFeeeeee);
            tip_label_->SetTextColor(0xFF000000);
        }
    }
    OutputDebugStringA("ShowTip\n");
    ::SetTimer(m_hWnd, kShowTooltip_TimerID, initial_delay_time_, 0);
    showing_ = true;
}
void DuiCustomTooltipWindow::HideTip()
{
    OutputDebugStringA("HideTip\n");
    //::SetTimer(m_hWnd, kHideTooltip_TimerID, initial_delay_time_, 0);
    ::KillTimer(m_hWnd, kAutoPopTooltip_TimerID);
    ::ShowWindow(m_hWnd, SW_HIDE);
    hiding_ = true;
}
void DuiCustomTooltipWindow::MoveTip(const POINT& loc)
{
    OutputDebugStringA("MoveTip\n");
    location_ = loc;
    ::ClientToScreen(host_, &location_);
    location_.x += 10;
    location_.y += 10;
    //::SetTimer(m_hWnd, kMoveTooltip_TimerID, 200, 0);
    ::SetWindowPos(m_hWnd, 0, location_.x, location_.y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
    moving_ = true;
}
DWORD DuiCustomTooltipWindow::GetDelayTime()
{
    return initial_delay_time_;
}
HWND DuiCustomTooltipWindow::GetTipHandle()
{
    return m_hWnd;
}

LPCTSTR DuiCustomTooltipWindow::GetWindowClassName() const
{
    return L"DuiCustomTooltipWindow";
}
UINT DuiCustomTooltipWindow::GetClassStyle() const
{
    return CS_VREDRAW | CS_HREDRAW;
}

LRESULT DuiCustomTooltipWindow::HandleMessage(UINT msg, WPARAM w_param, LPARAM l_param)
{
    LRESULT l_res = 0;
    bool handled = false;
    switch (msg) {
    case WM_CREATE:
    {
#if USE_CUSTOM_TOOLTIP
        paint_manager_.SetTooltipEnabled(false);
#endif
        //DuiLib::CContainerUI * con = new DuiLib::CContainerUI();
        DWORD tip_bkcolor = tip_bkcolor_;// RGB(GetBValue(tip_bkcolor_), GetGValue(tip_bkcolor_), GetRValue(tip_bkcolor_));
        DWORD tip_textcolor = tip_textcolor_;// RGB(GetBValue(tip_textcolor_), GetGValue(tip_textcolor_), GetRValue(tip_textcolor_));
        //con->SetBkColor(tip_bkcolor);
        //con->SetBorderRound({ 5, 5 });
        //con->SetInset({ 5, 5, 5, 5 });
        tip_label_ = new DuiLib::CLabelUI();
        tip_label_->SetBkColor(tip_bkcolor);
        tip_label_->SetTextColor(tip_textcolor);
        tip_label_->SetBorderColor(0xFFFF0000);
        tip_label_->SetBorderSize(2);
        tip_label_->SetBorderRound({ 2,2 });
        //tip_label_->SetTextStyle(DT_CENTER | DT_VCENTER);
        //->Add(tip_label_);
        paint_manager_.SetNoActivate(true);
        paint_manager_.Init(m_hWnd);
        paint_manager_.SetLayered(false);
        paint_manager_.AttachDialog(tip_label_);
        break;
    }
    case WM_TIMER:
    {
        if (kAutoPopTooltip_TimerID == w_param) {
            POINT cursor_loc;
            ::GetCursorPos(&cursor_loc);
            ::ScreenToClient(host_, &cursor_loc);
            if (tip_rect_.left < cursor_loc.x && cursor_loc.x < tip_rect_.right &&
                tip_rect_.top < cursor_loc.y && cursor_loc.y < tip_rect_.bottom) {

            }
            else {
                ::KillTimer(m_hWnd, kAutoPopTooltip_TimerID);
                ::ShowWindow(m_hWnd, SW_HIDE);
            }            
            return l_res;
        }

        if (w_param == kShowTooltip_TimerID || w_param == kHideTooltip_TimerID || w_param == kMoveTooltip_TimerID) {
            if (w_param == kShowTooltip_TimerID) {
                showing_ = false;
                ::KillTimer(m_hWnd, kShowTooltip_TimerID);

                if (true/*!hiding_*/) {
                    // show
                    POINT pt;
                    ::GetCursorPos(&pt);
                    pt.x += 10;
                    pt.y += 10;
                    SIZE real_size = tip_label_->EstimateSize({ tip_width_, 100 });
                    ::SetWindowPos(m_hWnd, HWND_TOPMOST, pt.x, pt.y, real_size.cx + 4, real_size.cy + 4, SWP_NOACTIVATE);
                    //::SetWindowPos(m_hWnd, 0, pt.x, pt.y, tip_width_, 30, SWP_NOACTIVATE);
                    ::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
                }

                ::KillTimer(m_hWnd, kAutoPopTooltip_TimerID);
                ::SetTimer(m_hWnd, kAutoPopTooltip_TimerID, auto_pop_delay_time_, 0);
            }
            else if (w_param == kHideTooltip_TimerID) {
                //hiding_ = false;
                //::KillTimer(m_hWnd, kHideTooltip_TimerID);

                //if (!showing_) {
                //    ::ShowWindow(m_hWnd, SW_HIDE);
                //}
            }
            else {
                //moving_ = false;
                //::KillTimer(m_hWnd, kMoveTooltip_TimerID);
                //// move
                //::SetWindowPos(m_hWnd, 0, location_.x, location_.y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
            }
            return l_res;
        }
        break;
    }
    case WM_SETCURSOR:
        l_res = TRUE;
        return l_res;
        break;
    case WM_ACTIVATE:
        return 0;
        break;
    case WM_SETFOCUS:
    case WM_KILLFOCUS:
        return 0;
        break;
    case WM_MOUSELEAVE:
    case WM_MOUSEMOVE:
    case WM_MOUSEHOVER:
        return 0;
        break;
    case WM_NCACTIVATE:
        if (!::IsIconic(m_hWnd)) {
            l_res = (w_param == 0) ? TRUE : FALSE;
            handled = true;
        }
        break;
    case WM_NCCALCSIZE:
    case WM_NCPAINT:
        l_res = 0;
        handled = true;
        break;
    case WM_MOUSEACTIVATE:
    {
        if (true) {
            l_res = MA_NOACTIVATE;
            handled = true;
        }
        break;
    }
    case WM_PAINT:
        OutputDebugStringA("WM_PAINT\n");
        break;
    }

    if (!handled) {
        handled = paint_manager_.MessageHandler(msg, w_param, l_param, l_res);
    }

    if (handled) {
        return l_res;
    }

    return CWindowWnd::HandleMessage(msg, w_param, l_param);
}
void DuiCustomTooltipWindow::OnFinalMessage(HWND hwnd)
{

}
