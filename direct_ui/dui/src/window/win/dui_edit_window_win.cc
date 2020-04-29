#include "dui_edit_window_win.h"

#include "core/win/gdi_render_engine.h"

#include <CommCtrl.h>
#include <windowsx.h>
#include <OleCtl.h>
#include <malloc.h>

DuiRealWindow* DuiRealWindow::MakeRealEditWindow(DuiEditWindow * owner)
{
    return new EditWindowWin(owner);
}

EditWindowWin::EditWindowWin(DuiEditWindow * owner)
    : DuiRealWindowWin(owner)
    , bk_brush_(nullptr)
    , inited_(false)
    , draw_caret_(false)
{

}

void EditWindowWin::OnInit()
{
    HFONT hfont = (HFONT)GetNativeFont();
    if (hfont) {
        SetWindowFont(hwnd_, hfont, TRUE);
    }
    Edit_LimitText(hwnd_, GetMaxChar());
    if (IsPasswordMode()) {
        Edit_SetPasswordChar(hwnd_, GetMaxChar());
    }
    Edit_SetText(hwnd_, L"text");
    Edit_SetModify(hwnd_, FALSE);
    SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));
    Edit_Enable(hwnd_, IsEnabled() == true);
    Edit_SetReadOnly(hwnd_, IsReadOnly() == true);

    ::ShowWindow(hwnd_, SW_SHOWNOACTIVATE);
    ::SetFocus(hwnd_);

    if (IsAutoSelAll()) {
        int text_len = GetWindowTextLength(hwnd_);
        if (text_len == 0) {
            text_len = 1;
        }
        Edit_SetSel(hwnd_, 0, text_len);
    }
    else {
        int text_len = GetWindowTextLength(hwnd_);
        Edit_SetSel(hwnd_, text_len, text_len);
    }

    inited_ = true;
}

bool EditWindowWin::OnGetWindowStyle(DWORD& style) const
{
    style = WS_CHILD | ES_AUTOHSCROLL;
    DuiEditStyle edit_style = GetEditStyle();
    if (edit_style & kLeft_EditStyle) {
        style |= ES_LEFT;
    }
    else if (edit_style & kCenter_EditStyle) {
        style |= ES_CENTER;
    }
    else if (edit_style & kRight_EditStyle) {
        style |= ES_RIGHT;
    }
    if (IsPasswordMode()) {
        style |= ES_PASSWORD;
    }

    return true;
}
bool EditWindowWin::OnGetWindowExStyle(DWORD& ex_style) const
{
    ex_style = 0;
    return true;
}

const wchar_t * EditWindowWin::GetWindowClassName()
{
    return L"EditWindowWin";
}
const wchar_t * EditWindowWin::GetSuperClassName()
{
    return WC_EDITW;
}
LRESULT EditWindowWin::HandleMessage(UINT msg, WPARAM w_param, LPARAM l_param)
{
    LRESULT l_res = 0;
    bool handled = true;
    switch (msg) {
    case WM_CREATE:
        if (GetHostWindow()) {
            GetHostWindow()->Add(owner_);
            //if (GetHostWindow()->IsLayered()) {
            ::SetTimer(hwnd_, kCaret_TimerID, ::GetCaretBlinkTime(), nullptr);
            //}
            handled = false;
        }
        break;
    case WM_KILLFOCUS:
        l_res = OnKillFocus(msg, w_param, l_param, handled);
        break;
    case OCM_COMMAND:
        if (GET_WM_COMMAND_CMD(w_param, l_param) == EN_CHANGE) {
            l_res = OnEditChanged(msg, w_param, l_param, handled);
        }
        else if (GET_WM_COMMAND_CMD(w_param, l_param) == EN_UPDATE) {
            RECT client_area;
            ::GetClientRect(hwnd_, &client_area);
            ::InvalidateRect(hwnd_, &client_area, FALSE);
        }
        break;
    case WM_KEYDOWN:
        if (w_param == VK_RETURN) {

        }
        break;
    case (OCM__BASE + WM_CTLCOLOREDIT):
    case (OCM__BASE + WM_CTLCOLORSTATIC):
    {
        if (GetHostWindow()->IsLayered()) {
            //GetHostWindow()->Add(owner_);
        }
        DuiColor bkcolor = GetEditClient()->GetNativeEditBkColor();
        if (bkcolor == DUI_ColorWHITE) {
            return 0;
        }
        ::SetBkMode((HDC)w_param, TRANSPARENT);
        DuiColor textcolor = GetEditClient()->GetTextColor();
        ::SetTextColor((HDC)w_param, RGB(DuiColorGetR(textcolor), DuiColorGetG(textcolor), DuiColorGetB(textcolor)));
        if (DuiColorGetA(bkcolor) < 0xff) {
            if (bk_brush_) {
                ::DeleteObject(bk_brush_);
            }
            DuiRect area = GetHostWindow()->GetNativeWindowRect(GetHandle());
            // TODO : snapshot from host window
        }
        else {
            if (!bk_brush_) {
                bk_brush_ = ::CreateSolidBrush(RGB(DuiColorGetR(bkcolor), DuiColorGetG(bkcolor), DuiColorGetB(bkcolor)));
            }
        }
        return (LRESULT)bk_brush_;
        break;
    }
    case WM_PAINT:
        if (GetHostWindow() && GetHostWindow()->IsLayered()) {
            GetHostWindow()->Add(owner_);
        }
        return ::CallWindowProc(old_window_proc_, hwnd_, msg, w_param, l_param);
        break;
    case WM_PRINT:
        if (GetHostWindow() && GetHostWindow()->IsLayered()) {
            l_res = DuiRealWindowWin::HandleMessage(msg, w_param, l_param);
            if (GetEditClient()->IsEnabled() && draw_caret_) {
                RECT client_area;
                ::GetClientRect(hwnd_, &client_area);
                POINT caret_location;
                ::GetCaretPos(&caret_location);
                RECT caret_area = { caret_location.x, caret_location.y, caret_location.x, 
                                    caret_location.y + (client_area.bottom - client_area.top) };
                //GdiRenderEngine::DrawLine((HDC)w_param, caret_area, 1, DUI_ColorBLACK);
            }
            return l_res;
        }
        break;
    case WM_TIMER:
        if (w_param == kCaret_TimerID) {
            if (GetHostWindow()->IsLayered()) {
                draw_caret_ = !draw_caret_;
                RECT client_area;
                ::GetClientRect(hwnd_, &client_area);
                ::InvalidateRect(hwnd_, &client_area, FALSE);

                // FIXME: 
                //GetHostWindow()->Invalidate();
            }
            return 0;
        }
        handled = false;
        break;
    }

    return DuiRealWindowWin::HandleMessage(msg, w_param, l_param);
}
void EditWindowWin::OnFinalMessage(HWND hwnd)
{
    GetHostWindow()->Invalidate();
    if (bk_brush_) {
        ::DeleteObject(bk_brush_);
    }
    GetHostWindow()->RemoveNativeWindow(hwnd);
    DuiRealWindowWin::OnFinalMessage(hwnd);
}

LRESULT EditWindowWin::OnKillFocus(UINT msg, WPARAM w_param, LPARAM l_param, bool& handled)
{
    LRESULT l_res = ::DefWindowProc(hwnd_, msg, w_param, l_param);
    if ((HWND)w_param != GetOwnerNativeWindow()) {
        ::SendMessage(GetOwnerNativeWindow(), WM_KILLFOCUS, w_param, l_param);
    }
    //FIXME: this->SendMessage(WM_CLOSE);
    return l_res;
}
LRESULT EditWindowWin::OnEditChanged(UINT msg, WPARAM w_param, LPARAM l_param, bool& handled)
{
    LRESULT l_res = 0;
    if (!inited_) {
        return l_res;
    }
    if (!GetEditClient()) {
        return l_res;
    }
    int text_len = ::GetWindowTextLength(hwnd_) + 1;
    LPTSTR pstr = static_cast<LPTSTR>(_alloca(text_len * sizeof(TCHAR)));
    _ASSERT(pstr);
    if (!pstr) {
        return l_res;
    }
    ::GetWindowText(hwnd_, pstr, text_len);
    GetEditClient()->OnEditChanged(pstr);
    if (GetHostWindow()->IsLayered()) {
        GetHostWindow()->Invalidate();
    }
    return l_res;
}

DuiEditWindow * EditWindowWin::Owner() const
{
    return static_cast<DuiEditWindow *>(owner_);
}
DuiEditWindow * EditWindowWin::Owner()
{
    return static_cast<DuiEditWindow *>(owner_);
}
DuiEditClient * EditWindowWin::GetEditClient() const
{
    return Owner()->GetClient();
}

DuiEditStyle EditWindowWin::GetEditStyle() const
{
    if (GetEditClient()) {
        return GetEditClient()->GetEditStyle();
    }
    return kLeft_EditStyle;
}
DuiWindow* EditWindowWin::GetHostWindow() 
{
    if (GetEditClient()) {
        return GetEditClient()->GetHostWindow();
    }
    return nullptr;
}
HWND EditWindowWin::GetOwnerNativeWindow()
{
    if (GetEditClient()) {
        return (HWND)(GetEditClient()->GetHostWindow()->GetHandle());
    }
    return ::GetParent(hwnd_);
}
void* EditWindowWin::GetNativeFont()
{
    if (GetEditClient()) {
        return GetEditClient()->GetNativeFont();
    }
    return nullptr;
}
uint32_t EditWindowWin::GetMaxChar() const
{
    if (GetEditClient()) {
        return GetEditClient()->GetMaxChar();
    }
    return 200;
}
wchar_t EditWindowWin::GetPasswordChar()
{
    if (GetEditClient()) {
        return GetEditClient()->GetPasswordChar();
    }
    return L'*';
}
bool EditWindowWin::IsEnabled() const
{
    if (GetEditClient()) {
        return GetEditClient()->IsEnabled();
    }
    return true;
}
bool EditWindowWin::IsReadOnly() const
{
    if (GetEditClient()) {
        return GetEditClient()->IsReadOnly();
    }
    return true;
}
bool EditWindowWin::IsAutoSelAll() const
{
    if (GetEditClient()) {
        return GetEditClient()->IsAutoSelAll();
    }
    return true;
}
bool EditWindowWin::IsPasswordMode() const
{
    if (GetEditStyle() & kPassword_EditStyle) {
        return true;
    }
    return false;
}
