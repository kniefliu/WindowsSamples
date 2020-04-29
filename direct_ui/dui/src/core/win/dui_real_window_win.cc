#include "dui_real_window_win.h"

#include "dui/core/dui_point.h"
#include "dui/core/dui_application.h"

#include "core/dui_window_priv.h"
#include "core/win/dui_gdi_real_canvas_win.h"

#include <crtdbg.h>
#include <OleCtl.h>
#include <CommCtrl.h>

#include <sstream>

#pragma comment(lib, "Comctl32.lib")

class FakeControl : public DuiControlBase {
public:
};
static FakeControl fake_control_;


DuiRealWindow* DuiRealWindow::MakeRealWindow(DuiWindow * owner)
{
    return new DuiRealWindowWin(owner);
}

HINSTANCE dui_instance = NULL;
DWORD dui_window_class_sytle = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
const wchar_t kDuiWindowClassName[] = L"DuiWindowClassName";
const wchar_t kDuiWindowPropName[] = L"DuiRealWindowWinX";
const DWORD kBasic_WindowStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
const DWORD kFullscreen_WindowStyle = WS_POPUP;
const DWORD kChild_WindowStyle = WS_CHILD;
const DWORD kBorderless_WindowStyle = WS_POPUP;
const DWORD kNormal_WindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
const DWORD kResizable_WindowStyle = WS_THICKFRAME | WS_MAXIMIZEBOX;
const DWORD kMask_WindowStyle = kFullscreen_WindowStyle | kBorderless_WindowStyle | kNormal_WindowStyle | kResizable_WindowStyle;

void DuiRealWindowWin::SetInstance(HINSTANCE inst)
{
    dui_instance = inst;
}
HINSTANCE DuiRealWindowWin::GetInstance()
{
    if (dui_instance) {
        return dui_instance;
    }
    
    dui_instance = ::GetModuleHandle(NULL);
    return dui_instance;
}

using UpdateLayeredWindowFunction = BOOL(__stdcall *)(HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD);
UpdateLayeredWindowFunction update_layered_window = nullptr;

DuiRealWindowWin::DuiRealWindowWin(DuiWindow * owner)
    : DuiRealWindow(owner)
    , hwnd_(NULL)
    , old_window_proc_(::DefWindowProc)
    , client_area_hdc_(0)
    , offscreen_surface_ (nullptr)
    , offscreen_paint_(true)
    , subclassed_(false)
    , mouse_tracking_(false)
    , expected_resize_(false)
    , layered_changed_(false)
    , opacity_(255)
{

}
DuiRealWindowWin::~DuiRealWindowWin()
{
    if (IsWindow(hwnd_)) {
        ::SetWindowLongPtr(hwnd_, GWLP_USERDATA, 0L);
    }
    if (offscreen_surface_) {
        delete offscreen_surface_;
        offscreen_surface_ = nullptr;
    }
}

HWND DuiRealWindowWin::GetHWND() const
{
    return hwnd_;
}
DuiRealWindowWin::operator HWND() const
{
    return hwnd_;
}

bool DuiRealWindowWin::RegisterWindowClass()
{
    WNDCLASS wc = { 0 };
    wc.style = GetClassStyle();
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hIcon = NULL;
    wc.lpfnWndProc = DuiRealWindowWin::WindowProc;
    wc.hInstance = DuiRealWindowWin::GetInstance();
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = GetWindowClassName();
    ATOM ret = ::RegisterClass(&wc);
    _ASSERT(ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS);
    return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}
bool DuiRealWindowWin::RegisterSuperclass()
{
    // Get the class information from an existing
    // window so we can subclass it later on...
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    if (!::GetClassInfoEx(NULL, GetSuperClassName(), &wc)) {
        if (!::GetClassInfoEx(DuiRealWindowWin::GetInstance(), GetSuperClassName(), &wc)) {
            _ASSERT(!"Unable to locate window class");
            return NULL;
        }
    }
    old_window_proc_ = wc.lpfnWndProc;
    wc.lpfnWndProc = DuiRealWindowWin::ControlProc;
    wc.hInstance = DuiRealWindowWin::GetInstance();
    wc.lpszClassName = GetWindowClassName();
    ATOM ret = ::RegisterClassEx(&wc);
    _ASSERT(ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS);
    return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

DuiPoint DuiRealWindowWin::GetLocationOnScreen() const
{
    POINT pt = { x_, y_ };
    ::ClientToScreen(hwnd_, &pt);
    return{ pt.x, pt.y };
}
void DuiRealWindowWin::SetSize(const DuiSize& sz)
{
    width_ = sz.width;
    height_ = sz.height;
    SetWindowPositionInternal(SWP_NOCOPYBITS | SWP_NOMOVE | SWP_NOACTIVATE);
}
void DuiRealWindowWin::SetMinimumSize(const DuiSize& sz)
{
    min_width_ = sz.width;
    min_height_ = sz.height;
    SetSize({ width_, height_ });
}
void DuiRealWindowWin::SetMaximumSize(const DuiSize& sz)
{
    max_width_ = sz.width;
    max_height_ = sz.height;
    SetSize({ width_, height_ });
}

DuiPoint DuiRealWindowWin::GetMousePosition()
{
    POINT pt;
    ::GetCursorPos(&pt);
    ::ScreenToClient(hwnd_, &pt);
    return{ pt.x, pt.y };
}

void DuiRealWindowWin::Invalidate(const DuiRect* repaint_area/* = nullptr*/)
{
    if (!repaint_area) {
        if (!IsLayered()) {
            ::InvalidateRect(hwnd_, NULL, FALSE);
        }
        else {
            DuiRect client_area = { 0, 0, width_, height_ };
            layered_update_area_.Join(client_area);
        }
    }
    else {
        DuiRect new_area = *repaint_area;
        if (new_area.left < 0) new_area.left = 0;
        if (new_area.top < 0) new_area.top = 0;
        if (new_area.right < new_area.left) new_area.right = new_area.left;
        if (new_area.bottom < new_area.top) new_area.bottom = new_area.top;
        if (!IsLayered()) {
            RECT inv_area = { new_area.left, new_area.top, new_area.right, new_area.bottom };
            ::InvalidateRect(hwnd_, &inv_area, FALSE);
        }
        else {
            layered_update_area_.Join(new_area);
        }
    }
}

int DuiRealWindowWin::GetNativeWindowCount() const
{
    return (int)(native_windows_.size());
}
DuiRect DuiRealWindowWin::GetNativeWindowRect(DuiWindowHandle child_window) const
{
    RECT child_area;
    ::GetWindowRect((HWND)child_window, &child_area);
    ::ScreenToClient(hwnd_, (LPPOINT)(&child_area));
    ::ScreenToClient(hwnd_, (LPPOINT)(&child_area) + 1);
    return{ child_area.left, child_area.top, child_area.right, child_area.bottom };
}
bool DuiRealWindowWin::AddNativeWindow(DuiControlBase * controller, DuiWindowHandle child_window)
{
    if (!controller || !child_window) {
        return false;
    }

    DuiRect child_area = GetNativeWindowRect(child_window);
    Invalidate(&child_area);

    NativeWindow native_window_wrapper = { controller, child_window };
    if (std::find(native_windows_.begin(), native_windows_.end(), native_window_wrapper) != native_windows_.end()) {
        return false;
    }
    native_windows_.push_back(native_window_wrapper);
    return true;
}
bool DuiRealWindowWin::RemoveNativeWindow(DuiWindowHandle child_window)
{
    for (auto iter = native_windows_.begin(); iter != native_windows_.end(); ++iter) {
        if ((iter->native_handle) == child_window) {
            native_windows_.erase(iter);
            return true;
        }
    }
    return false;
}
void DuiRealWindowWin::PaintNativeWindow(DuiCanvas * canvas, const DuiRect& repaint_area)
{
    if (IsLayered()) {
        DuiCanvasPriv canvas_priv(canvas);
        DuiGdiRealCanvas* real_canvas = dynamic_cast<DuiGdiRealCanvas*>(canvas_priv.GetRealCanvas());
        if (!real_canvas) {
            return;
        }
        HDC offscreen_hdc = real_canvas->GetHDC();
        // process native window
        for (auto iter = native_windows_.begin(); iter != native_windows_.end(); ++iter) {
            HWND child_hwnd = static_cast<HWND>(iter->native_handle);
            if (!::IsWindow(child_hwnd)) {
                iter = native_windows_.erase(iter);
                continue;
            }
            if (!::IsWindowVisible(child_hwnd)) {
                continue;
            }
            DuiRect child_area = GetNativeWindowRect(child_hwnd);
            DuiRect temp_area = { 0 };
            temp_area = child_area;
            if (!temp_area.Intersect(repaint_area)) {
                continue;
            }
            GdiSurface child_surface(offscreen_hdc, child_area.width(), child_area.height(), kARGB32_GdiSurface);
            HDC child_hdc = child_surface.SelectDC();
            ::SendMessage(child_hwnd, WM_PRINT, (WPARAM)child_hdc, (LPARAM)(PRF_CHECKVISIBLE | PRF_CHILDREN | PRF_CLIENT | PRF_OWNED));
            COLORREF* pixel;
            COLORREF* pixels = child_surface.GetPixels();
            for (int y = 0; y < child_area.height(); y++) {
                for (int x = 0; x < child_area.width(); x++) {
                    pixel = pixels + y * child_area.width() + x;
                    if (*pixel != 0x00000000) {
                        *pixel |= 0xff000000;
                    }
                }
            }
            ::BitBlt(offscreen_hdc, child_area.x(), child_area.y(), child_area.width()
                , child_area.height(), child_hdc, 0, 0, SRCCOPY);
            child_surface.UnselectDC();
        }
    }
}

DuiAlpha DuiRealWindowWin::GetOpacity() const
{
    return opacity_;
}
void DuiRealWindowWin::SetOpacity(DuiAlpha opacity)
{
    opacity_ = opacity;
    if (!hwnd_) {
        using SetLayeredWindowAttributesFunction = BOOL(__stdcall *)(HWND, COLORREF, BYTE, DWORD);
        static SetLayeredWindowAttributesFunction set_layered_window_attributes = nullptr;

        HMODULE user_module = ::GetModuleHandleW(L"User32.dll");
        if (user_module) {
            set_layered_window_attributes =
                (SetLayeredWindowAttributesFunction)::GetProcAddress(user_module, "SetLayeredWindowAttributes");
            if (!set_layered_window_attributes) {
                return;
            }
        }

        DWORD exstyle = ::GetWindowLong(hwnd_, GWL_EXSTYLE);
        DWORD new_exstyle = exstyle;
        if (opacity >= 0 && opacity < 256) {
            new_exstyle |= WS_EX_LAYERED;
        }
        else {
            new_exstyle &= ~WS_EX_LAYERED;
        }
        if (exstyle != new_exstyle) {
            ::SetWindowLong(hwnd_, GWL_EXSTYLE, new_exstyle);
        }
        set_layered_window_attributes(hwnd_, 0, opacity, LWA_ALPHA);

        flags_ &= ~kLayered_Window;

        Invalidate();
    }
}
bool DuiRealWindowWin::IsLayered() {
    return (flags_ & kLayered_Window) == kLayered_Window;
}
void DuiRealWindowWin::SetLayered(bool layered)
{
    if (hwnd_ && layered != IsLayered()) {
        UINT uStyle = ::GetWindowLong(hwnd_, GWL_STYLE);
        if ((uStyle & WS_CHILD) != 0) {
            return;
        }

        if (!update_layered_window) {
            HMODULE user_module = ::GetModuleHandleW(L"User32.dll");
            if (user_module) {
                update_layered_window =
                    (UpdateLayeredWindowFunction)::GetProcAddress(user_module, "UpdateLayeredWindow");               
            }
        }
        if (!update_layered_window) {
            return;
        }

        DWORD exstyle = ::GetWindowLong(hwnd_, GWL_EXSTYLE);
        DWORD new_exstyle = exstyle;
        if (layered) {
            new_exstyle |= WS_EX_LAYERED;
            ::SetTimer(hwnd_, kLayeredUpdateTimerID, 10L, NULL);
        }
        else {
            new_exstyle &= ~WS_EX_LAYERED;
            ::KillTimer(hwnd_, kLayeredUpdateTimerID);
        }
        if (exstyle != new_exstyle) {
            ::SetWindowLong(hwnd_, GWL_EXSTYLE, new_exstyle);
        }
        if (layered) {
            flags_ |= kLayered_Window;
        }
        else {
            flags_ &= ~kLayered_Window;
        }

        // FIXME: force repaint
        //if (m_pRoot != NULL) m_pRoot->NeedUpdate();

        Invalidate();
    }
}
DuiAlpha DuiRealWindowWin::GetLayeredOpacity()
{
    return opacity_;
}
void DuiRealWindowWin::SetLayeredOpacity(DuiAlpha opacity)
{
    opacity_ = opacity;
    layered_changed_ = true;
    Invalidate();
}

void DuiRealWindowWin::SetWindowPositionInternal(UINT sys_flags)
{
    HWND hwnd = hwnd_;
    RECT rect;
    DWORD style;
    HWND top = 0;
    BOOL menu;
    int x, y;
    int w, h;

    style = GetWindowLong(hwnd, GWL_STYLE);
    rect.left = 0;
    rect.top = 0;
    rect.right = width_;
    rect.bottom = height_;
    menu = (style & WS_CHILDWINDOW) ? FALSE : (GetMenu(hwnd) != NULL);
    if (!(sys_flags & kLayered_Window)) {
        AdjustWindowRectEx(&rect, style, menu, 0);
    }
    w = (rect.right - rect.left);
    h = (rect.bottom - rect.top);
    x = x_ + rect.left;
    y = y_ + rect.top;

    expected_resize_ = true;
    ::SetWindowPos(hwnd, top, x, y, w, h, sys_flags);
    expected_resize_ = false;
}

static bool SetupWindowData(DuiRealWindowWin * window, HWND hwnd, bool created)
{
    window->hwnd_ = hwnd;
    window->client_area_hdc_ = ::GetDC(hwnd);
    window->created_ = created;
    window->initializing_ = true;

    /* Fill in the DUI window with the window data */
    {
        RECT rect;
        if (GetClientRect(hwnd, &rect)) {
            int w = rect.right;
            int h = rect.bottom;
            if ((window->width_ && window->width_ != w) || (window->height_ && window->height_ != h)) {
                /* We tried to create a window larger than the desktop and Windows didn't allow it.  Override! */
                RECT rect;
                DWORD style;
                BOOL menu;
                int x, y;
                int w, h;

                /* Figure out what the window area will be */
                style = GetWindowLong(hwnd, GWL_STYLE);
                rect.left = 0;
                rect.top = 0;
                rect.right = window->width_;
                rect.bottom = window->height_;
                menu = (style & WS_CHILDWINDOW) ? FALSE : (GetMenu(hwnd) != NULL);
                DWORD ex_style = GetWindowLong(hwnd, GWL_EXSTYLE);
                if (!(ex_style & WS_EX_LAYERED)) {
                    AdjustWindowRectEx(&rect, style, menu, 0);
                }
                w = (rect.right - rect.left);
                h = (rect.bottom - rect.top);
                x = window->x_ + rect.left;
                y = window->y_ + rect.top;

                ::SetWindowPos(hwnd, HWND_NOTOPMOST, x, y, w, h, SWP_NOCOPYBITS | SWP_NOZORDER | SWP_NOACTIVATE);
            }
            else {
                window->width_ = w;
                window->height_ = h;
            }
        }
    }
    {
        POINT point;
        point.x = 0;
        point.y = 0;
        if (::ClientToScreen(hwnd, &point)) {
            window->x_ = point.x;
            window->y_ = point.y;
        }
    }
    {
        DWORD style = GetWindowLong(hwnd, GWL_STYLE);
        if (style & WS_VISIBLE) {
            window->flags_ |= kShown_Window;
        }
        else {
            window->flags_ &= ~kShown_Window;
        }
        if (style & (WS_BORDER | WS_THICKFRAME)) {
            window->flags_ &= ~kBorderless_Window;
        }
        else {
            window->flags_ |= kBorderless_Window;
        }
        if (style & WS_THICKFRAME) {
            window->flags_ |= kResizable_Window;
        }
        else {
            window->flags_ &= ~kResizable_Window;
        }
#ifdef WS_MAXIMIZE
        if (style & WS_MAXIMIZE) {
            window->flags_ |= kMaximized_Window;
        }
        else
#endif
        {
            window->flags_ &= ~kMaximized_Window;
        }
#ifdef WS_MINIMIZE
        if (style & WS_MINIMIZE) {
            window->flags_ |= kMinimized_Window;
        }
        else
#endif
        {
            window->flags_ &= ~kMinimized_Window;
        }
    }
    /*if (GetFocus() == hwnd) {
        window->flags |= SDL_WINDOW_INPUT_FOCUS;
        SDL_SetKeyboardFocus(data->window);

        if (window->flags & SDL_WINDOW_INPUT_GRABBED) {
            RECT rect;
            GetClientRect(hwnd, &rect);
            ClientToScreen(hwnd, (LPPOINT)& rect);
            ClientToScreen(hwnd, (LPPOINT)& rect + 1);
            ClipCursor(&rect);
        }
    }*/

    /* Enable multi-touch */
    //if (videodata->RegisterTouchWindow) {
    //    videodata->RegisterTouchWindow(hwnd, (TWF_FINETOUCH | TWF_WANTPALM));
    //}

    /* Enable dropping files */
    DragAcceptFiles(hwnd, TRUE);

    window->initializing_ = false;

    /* All done! */
    return 0;
}
DuiWindowHandle DuiRealWindowWin::OnCreate(DuiRealWindow * parent)
{
    UINT window_style = 0;
    window_style |= this->GetWindowStyle(flags_);

    RECT rect;
    /* Figure out what the window area will be */
    rect.left = x_;
    rect.top = y_;
    rect.right = x_ + width_;
    rect.bottom = y_ + height_;
    if (!(flags_ & kLayered_Window)) {
        AdjustWindowRectEx(&rect, window_style, FALSE, 0);
    }
    int x = rect.left;
    int y = rect.top;
    int w = (rect.right - rect.left);
    int h = (rect.bottom - rect.top);

    UINT window_ex_style = GetWindowExStyle(flags_);
    flags_ = (flags_ & ~kLayered_Window);
    DuiRealWindowWin * real_parent = static_cast<DuiRealWindowWin*>(parent);
    HWND hwnd_parent = 0;
    if (real_parent) {
        hwnd_parent = real_parent->hwnd_;
    }
    HMENU menu = nullptr;
    HWND win_handle = this->Create(hwnd_parent, L"", window_style, window_ex_style,
        x, y, w, h, menu);
    SetupWindowData(this, hwnd_, true);
    return (DuiWindowHandle)(win_handle);
}
DuiWindowHandle DuiRealWindowWin::OnCreateFrom(DuiWindowHandle extern_handle)
{
    HWND hwnd = (HWND)extern_handle;
    int title_len = ::GetWindowTextLength(hwnd);
    wchar_t * title = new wchar_t[title_len + 1];
    if (title) {
        title_len = ::GetWindowText(hwnd, title, title_len + 1);
    }
    else {
        title_len = 0;
    }
    if (title_len > 0) {
        title_ = title;
    }
    if (title) {
        delete[] title;
    }
    SetupWindowData(this, hwnd, false);

    return (DuiWindowHandle)hwnd_;
}
DuiWindowHandle DuiRealWindowWin::OnGetHandle()
{
    return (DuiWindowHandle)hwnd_;
}
void DuiRealWindowWin::OnSetTitle()
{
    LPCWSTR title = title_.c_str();
    ::SetWindowTextW(hwnd_, title);
}
void DuiRealWindowWin::OnAdd(DuiRealWindow * child)
{
    DuiRealWindowWin* win_child = static_cast<DuiRealWindowWin*>(child);
    ::SetParent(win_child->hwnd_, this->hwnd_);
    AddNativeWindow(&fake_control_, child->GetHandle());
}
void DuiRealWindowWin::OnShow()
{
    ::ShowWindow(hwnd_, SW_SHOW);
}
void DuiRealWindowWin::OnHide()
{
    ::ShowWindow(hwnd_, SW_HIDE);
}
void DuiRealWindowWin::OnRaise()
{
    ::SetForegroundWindow(hwnd_);
}
void DuiRealWindowWin::OnMaximize()
{
    expected_resize_ = true;
    ::ShowWindow(hwnd_, SW_MAXIMIZE);
    expected_resize_ = false;
}
void DuiRealWindowWin::OnMinimize()
{
    ::ShowWindow(hwnd_, SW_MINIMIZE);
}
void DuiRealWindowWin::OnRestore()
{
    expected_resize_ = true;
    ::ShowWindow(hwnd_, SW_RESTORE);
    expected_resize_ = false;
}
void DuiRealWindowWin::OnSetLocation()
{
    SetWindowPositionInternal(SWP_NOCOPYBITS | SWP_NOSIZE | SWP_NOACTIVATE);
}
void DuiRealWindowWin::SetFullscreen(VideoDisplay * display, bool fullscreen)
{
    HWND hwnd = hwnd_;
    RECT rect;
    DuiRect bounds;
    DWORD style;
    HWND top;
    BOOL menu;
    int x, y;
    int w, h;

    bool allow_topmost = false;
    if (allow_topmost && (flags_ & (kFullscreen_Window)) == (kFullscreen_Window)) {
        top = HWND_TOPMOST;
    }
    else {
        top = HWND_NOTOPMOST;
    }

    style = GetWindowLong(hwnd, GWL_STYLE);
    style &= kMask_WindowStyle;
    style |= GetWindowStyle(flags_);

    DuiRealApplication::GetDisplayBounds(display, &bounds);

    if (fullscreen) {
        x = bounds.x();
        y = bounds.y();
        w = bounds.width();
        h = bounds.height();

        /* Unset the maximized flag.  This fixes
        https://bugzilla.libsdl.org/show_bug.cgi?id=3215
        */
        if (style & WS_MAXIMIZE) {
            windowed_mode_was_maximized_ = true;
            style &= ~WS_MAXIMIZE;
        }
    }
    else {
        /* Restore window-maximization state, as applicable.
        Special care is taken to *not* do this if and when we're
        alt-tab'ing away (to some other window; as indicated by
        in_window_deactivation), otherwise
        https://bugzilla.libsdl.org/show_bug.cgi?id=3215 can reproduce!
        */
        if (windowed_mode_was_maximized_ && !in_window_deactivation_) {
            style |= WS_MAXIMIZE;
            windowed_mode_was_maximized_ = false;
        }
        rect.left = 0;
        rect.top = 0;
        rect.right = windowed_.width();
        rect.bottom = windowed_.height();
        menu = (style & WS_CHILDWINDOW) ? FALSE : (GetMenu(hwnd) != NULL);
        if (!(flags_ & kLayered_Window)) {
            AdjustWindowRectEx(&rect, style, menu, 0);;
        }
        w = (rect.right - rect.left);
        h = (rect.bottom - rect.top);
        x = windowed_.x() + rect.left;
        y = windowed_.y() + rect.top;
    }
    SetWindowLong(hwnd, GWL_STYLE, style);
    expected_resize_ = true;
    ::SetWindowPos(hwnd, top, x, y, w, h, SWP_NOCOPYBITS | SWP_NOACTIVATE);
    expected_resize_ = false;
}

LRESULT DuiRealWindowWin::SendMessage(UINT msg, WPARAM w_param, LPARAM l_param)
{
    _ASSERT(::IsWindow(hwnd_));
    return ::SendMessage(hwnd_, msg, w_param, l_param);
}
LRESULT DuiRealWindowWin::PostMessage(UINT msg, WPARAM w_param, LPARAM l_param)
{
    _ASSERT(::IsWindow(hwnd_));
    return ::PostMessage(hwnd_, msg, w_param, l_param);
}

HWND DuiRealWindowWin::Create(HWND hwnd_parent, LPCTSTR title, DWORD style, DWORD ex_style, int x, int y, int w, int h, HMENU menu)
{
    if (GetSuperClassName() && !RegisterSuperclass()) {
        return NULL;
    }
    if (!GetSuperClassName() && !RegisterWindowClass()) {
        return NULL;
    }
    HWND hwnd = ::CreateWindowEx(ex_style, GetWindowClassName(), title, style, x, y, w, h,
        hwnd_parent, menu, dui_instance, this);
    DWORD err =::GetLastError();
    // setup window data here
    hwnd_ = hwnd;
    return hwnd;
}

HWND DuiRealWindowWin::Subclass(HWND hwnd)
{
    // FIXME: 
    _ASSERT(NULL);
    return hwnd;
}
void DuiRealWindowWin::Unsubclass()
{
    // FIXME: 
}

DWORD DuiRealWindowWin::GetWindowStyle(DuiWindowFlags flags) const
{
    DWORD style = 0;
    if (OnGetWindowStyle(style)) {
        return style;
    }
    style = 0;
    if (style == 0) {
        if (flags & kFullscreen_Window) {
            style |= kFullscreen_WindowStyle;
        }
        else if (flags & kChild_Window) {
            style |= kChild_WindowStyle;
        }
        else {
            if (flags & kBorderless_Window) {
                style |= kBorderless_WindowStyle;
            }
            else {
                style |= kNormal_WindowStyle;
            }
            if (flags & kResizable_Window) {
                style |= kResizable_WindowStyle;
            }
        }
    }
    style |= kBasic_WindowStyle;
    return style;
}
DWORD DuiRealWindowWin::GetWindowExStyle(DuiWindowFlags flag) const
{
    DWORD ex_style = 0;
    if (OnGetWindowExStyle(ex_style)) {
        return ex_style;
    }
    ex_style = 0;
    if (flag & kLayered_Window) {
        ex_style |= WS_EX_LAYERED;
    }
    return ex_style;
}

const wchar_t * DuiRealWindowWin::GetWindowClassName()
{
    return kDuiWindowClassName;
}
const wchar_t * DuiRealWindowWin::GetSuperClassName()
{
    return nullptr;
}
UINT DuiRealWindowWin::GetClassStyle() const
{
    return dui_window_class_sytle;
}

LRESULT DuiRealWindowWin::HandleMessage(UINT msg, WPARAM w_param, LPARAM l_param)
{
    if (!::IsWindow(hwnd_)) {
        return 0;
    }

    LRESULT result = -1;

    if (msg != WM_TIMER && msg != WM_PAINT) {
        std::stringstream ss;
        ss << "message id: " << msg << "\n";
        //OutputDebugStringA(ss.str().c_str());
        result = -1;
    }
    switch (msg) {
    case WM_CREATE:
    {
        /* Sent when an application requests that a window be created by calling the CreateWindowEx or CreateWindow function. (The message is sent before the function returns.) 
         * The window procedure of the new window receives this message after the window is created, 
         * but before the window becomes visible.
         */
        client_area_hdc_ = ::GetDC(hwnd_);

        /* If an application processes this message, it should return zero to continue creation of the window. 
         * If the application returns ¨C1, the window is destroyed and the CreateWindowEx or CreateWindow function returns a NULL handle.
         */
        result = 0;

        DuiWindowEventResult evt_ret;
        DuiWindowEvent evt;
        evt.type = kCreate_WindowEvent;
        bool handled = HandleEvent(&evt, evt_ret);
        if (handled) {
            return result;
        }
        break;
    }
    case WM_DESTROY:
    {
        /* Sent when a window is being destroyed. 
         * It is sent to the window procedure of the window being destroyed after 
         * the window is removed from the screen.
         * This message is sent first to the window being destroyed and 
         * then to the child windows (if any) as they are destroyed. 
         * During the processing of the message, it can be assumed that all child windows still exist.
         */
        if (client_area_hdc_) {
            ::ReleaseDC(hwnd_, client_area_hdc_);
            client_area_hdc_ = nullptr;
        }

        /* If an application processes this message, it should return zero. */
        result = 0;

        DuiWindowEventResult evt_ret;
        DuiWindowEvent evt;
        evt.type = kToDestroy_WindowEvent;
        bool handled = HandleEvent(&evt, evt_ret);
        if (handled) {
            return result;
        }
        break;
    }
    case WM_MOVE:
    {
        /* Sent after a window has been moved. */
        /* The parameters are given in screen coordinates for overlapped and pop - up windows 
         * and in parent - client coordinates for child windows.
         */
        DuiPoint pt;
        pt.set(LOWORD(l_param), HIWORD(l_param));

        /*If an application processes this message, it should return zero.*/
        result = 0;

        DuiWindowEventResult evt_ret;
        DuiWindowEvent evt;
        evt.type = kMoved_WindowEvent;
        evt.timestamp = ::GetTickCount();
        evt.location = pt;

        bool handled = HandleEvent(&evt, evt_ret);
        if (handled) {
            return result;
        }
        break;
    }
    case WM_SIZE:
    {
        /*Sent to a window after its size has changed.*/
        DuiWindowEvent evt;
        evt.timestamp = ::GetTickCount();
        bool post = true;
        switch (w_param) {
        case SIZE_MINIMIZED:
            evt.type = kMinimized_WindowEvent;
            break;
        case SIZE_MAXIMIZED:
            evt.type = kMaximized_WindowEvent;
            break;
        /*The window has been resized, but neither the SIZE_MINIMIZED nor SIZE_MAXIMIZED value applies.*/
        case SIZE_RESTORED:
            evt.type = kRestored_WindowEvent;
            break;
        default:
            post = false;
            break;
        }

        if (post) {
            /*If an application processes this message, it should return zero.*/
            result = 0;

            DuiWindowEventResult evt_ret;
            bool handled = HandleEvent(&evt, evt_ret);
            if (handled) {
                return result;
            }
        }
        break;
    }
    case WM_ACTIVATE:
    {
        /* Sent to both the window being activated and the window being deactivated.
         */
        BOOL minimized = (BOOL)(HIWORD(w_param));
        if (!minimized && (LOWORD(w_param) != WA_INACTIVE)) {
            if (LOWORD(w_param) == WA_CLICKACTIVE) {
                if (GetAsyncKeyState(VK_LBUTTON)) {

                }
                if (GetAsyncKeyState(VK_RBUTTON)) {

                }
                if (GetAsyncKeyState(VK_MBUTTON)) {

                }
                
            }
        }
        else {

        }

        DuiWindowEvent evt;
        if ((LOWORD(w_param) == WA_ACTIVE)) {
            evt.type = kActive_WindowEvent;
        }
        else if ((LOWORD(w_param) == WA_CLICKACTIVE)) {
            evt.type = kClickActive_WindowEvent;
        }
        else if ((LOWORD(w_param) == WA_INACTIVE)) {
            evt.type = kInactive_WindowEvent;
        }
        evt.window_handle = (DuiWindowHandle)l_param;

        /*If an application processes this message, it should return zero.*/
        result = 0;

        DuiWindowEventResult evt_ret;
        bool handled = HandleEvent(&evt, evt_ret);
        if (handled) {
            return result;
        }
        break;
    }
    case WM_SETFOCUS:
    {
        /* Sent to a window after it has gained the keyboard focus. */
        DuiWindowEvent evt;
        evt.type = kSetFocus_WindowEvent;
        evt.window_handle = (DuiWindowHandle)w_param;

        /*An application should return zero if it processes this message.*/
        result = 0;
        
        DuiWindowEventResult evt_ret;
        bool handled = HandleEvent(&evt, evt_ret);
        if (handled) {
            return result;
        }
        break;
    }
    case WM_KILLFOCUS:
    {
        /* Sent to a window immediately before it loses the keyboard focus. */
        DuiWindowEvent evt;
        evt.type = kKillFocus_WindowEvent;
        evt.window_handle = (DuiWindowHandle)w_param;

        /*If an application is displaying a caret, the caret should be destroyed at this point.*/
        /*An application should return zero if it processes this message.*/
        result = 0;

        DuiWindowEventResult evt_ret;
        bool handled = HandleEvent(&evt, evt_ret);
        if (handled) {
            return result;
        }
        break;
    }
    case WM_ENABLE:
    {
        /* Sent when an application changes the enabled state of a window. 
         * It is sent to the window whose enabled state is changing. 
         * This message is sent before the EnableWindow function returns, 
         * but after the enabled state (WS_DISABLED style bit) of the window has changed.
         */
        DuiWindowEvent evt;
        BOOL enabled = (BOOL)w_param;
        if (enabled) {
            evt.type = kEnabled_WindowEvent;
        }
        else {
            evt.type = kDisabled_WindowEvent;
        }

        /*If an application processes this message, it should return zero.*/
        result = 0;

        DuiWindowEventResult evt_ret;
        bool handled = HandleEvent(&evt, evt_ret);
        if (handled) {
            return result;
        }
        break;
    }
    case WM_PAINT:
    {
        /* The WM_PAINT message is sent when the system or another application makes a request 
         * to paint a portion of an application's window. 
         * The message is sent when the UpdateWindow or RedrawWindow function is called, 
         * or by the DispatchMessage function when the application obtains a WM_PAINT message 
         * by using the GetMessage or PeekMessage function.
         */
        /*An application returns zero if it processes this message.*/
        result = 0; 
        DuiRect repaint_area = { 0, 0, width_, height_ };

        if (IsLayered()) {
            offscreen_paint_ = true;
            repaint_area = layered_update_area_;
            if (layered_update_area_.IsEmpty()) {
                PAINTSTRUCT ps = { 0 };
                ::BeginPaint(hwnd_, &ps);
                ::EndPaint(hwnd_, &ps);
                return result;
            }
            if (repaint_area.right > width_) repaint_area.right = width_;
            if (repaint_area.bottom > height_) repaint_area.bottom = height_;
            layered_update_area_.SetEmpty();
        }
        else {
            RECT sys_paint_area;
            if (!::GetUpdateRect(hwnd_, &sys_paint_area, FALSE)) {
                return result;
            }
            repaint_area = { sys_paint_area.left, sys_paint_area.top, sys_paint_area.right, sys_paint_area.bottom };
        }

        // offscreen create
        if (offscreen_paint_) {
            bool offscreen_recreated = false;
            if (!offscreen_surface_ || offscreen_surface_->GetWidth() != width_ || offscreen_surface_->GetHeight() != height_) {
                offscreen_recreated = true;
            }
            if (!offscreen_recreated) {
                if (offscreen_surface_) {
                    bool window_opaque = !IsLayered();
                    bool surface_opaque = (offscreen_surface_->GetType() != kARGB32_GdiSurface);
                    if (window_opaque != surface_opaque) {
                        offscreen_recreated = true;
                    }
                }
            }
            if (offscreen_recreated) {
                if (offscreen_surface_) {
                    delete offscreen_surface_;
                    offscreen_surface_ = nullptr;
                }
                GdiSurfaceType surface_type = kARGB32_GdiSurface;// kRGB32_GdiSurface;
                if (IsLayered()) {
                    surface_type = kARGB32_GdiSurface;
                }
                offscreen_surface_ = new GdiSurface(client_area_hdc_, width_, height_, surface_type);
            }
        }

        PAINTSTRUCT ps = { 0 };
        HDC hdc = ::BeginPaint(hwnd_, &ps);
        if (offscreen_paint_) {
            HDC offscreen_hdc = offscreen_surface_->SelectDC();
            int save = ::SaveDC(offscreen_hdc);

            if (IsLayered()) {
                offscreen_surface_->Clear(repaint_area);
            }
#if 1
            // offscreen paint event
            DuiWindowEvent offscreen_evt;
            offscreen_evt.type = kPaint_WindowEvent;
            offscreen_evt.paint_area = repaint_area;
            bool use_argb = true; // IsLayered();
            AutoGdiRealCanvas offscreen_canvas(offscreen_hdc, use_argb);
            offscreen_evt.canvas = offscreen_canvas.GetCanvas();
            DuiWindowEventResult evt_ret;
            HandleEvent(&offscreen_evt, evt_ret);
#else
            RECT rc = { 10, 10, 12, 12 };
            ::SetBkColor(offscreen_hdc, 0x00ffffff);
            ::ExtTextOut(offscreen_hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);

            DuiRect area = { 10, 10, 12, 12 };
            offscreen_surface_->Clear(area, DUI_ColorWHITE);
#endif

            if (IsLayered()) {
                PaintNativeWindow(offscreen_canvas.GetCanvas(), repaint_area);
            }

            ::RestoreDC(offscreen_hdc, save);

            // flush            
            if (IsLayered()) {
                RECT window_area = { 0 };
                ::GetWindowRect(hwnd_, &window_area);
                BLENDFUNCTION bf = { AC_SRC_OVER, 0, opacity_, AC_SRC_ALPHA };
                POINT dest_pos = { window_area.left, window_area.top };
                SIZE window_size = { width_, height_ };
                POINT src_pos = { 0, 0 };
                if (!update_layered_window(hwnd_, hdc, &dest_pos, &window_size, offscreen_hdc, &src_pos, 0, &bf, ULW_ALPHA)) {
                    DWORD err =::GetLastError();
                    err = 0;
                }
            }
            else {
                DuiWindowEvent evt;
                evt.timestamp = ::GetTickCount();
                evt.type = kFlush_WindowEvent;
                ::BitBlt(hdc, repaint_area.left, repaint_area.top, repaint_area.width(),
                    repaint_area.height(), offscreen_hdc, repaint_area.left, repaint_area.top, SRCCOPY);
                AutoGdiRealCanvas auto_canvas(hdc, false);
                DuiWindowEventResult evt_ret;
                evt.canvas = auto_canvas.GetCanvas();
                HandleEvent(&evt, evt_ret);
            }
            offscreen_surface_->UnselectDC();
        }
        else {
            DuiWindowEvent evt;
            evt.timestamp = ::GetTickCount();
            evt.type = kFlush_WindowEvent;
            AutoGdiRealCanvas auto_canvas(hdc, false);
            DuiWindowEventResult evt_ret;
            evt.canvas = auto_canvas.GetCanvas();
            bool handled = HandleEvent(&evt, evt_ret);
        }
        ::EndPaint(hwnd_, &ps);

        return result;
        break;
    }
    case WM_PRINTCLIENT:
    {
        /**
         * The WM_PRINTCLIENT message is sent to a window to request that it draw its client 
         * area in the specified device context, most commonly in a printer device context.
         */
        /**
         * A window can process this message in much the same manner as WM_PAINT, except that 
         * BeginPaint and EndPaint need not be called (a device context is provided), and 
         * the window should draw its entire client area rather than just the invalid region.
         */
        RECT client_area;
        ::GetClientRect(hwnd_, &client_area);
        HDC hdc = (HDC)w_param;
        int save = ::SaveDC(hdc);
        AutoGdiRealCanvas auto_canvas(hdc, true);
        // offscreen paint event
        DuiWindowEvent offscreen_evt;
        offscreen_evt.type = kPrintClient_WindowEvent;
        offscreen_evt.paint_area.Set(0, 0, client_area.right - client_area.left, client_area.bottom - client_area.top);
        offscreen_evt.canvas = auto_canvas.GetCanvas();
        DuiWindowEventResult evt_ret;
        HandleEvent(&offscreen_evt, evt_ret);
        //m_pRoot->Paint(hDC, rcClient, NULL);
        // Check for traversing children. The crux is that WM_PRINT will assume
        // that the DC is positioned at frame coordinates and will paint the child
        // control at the wrong position. We'll simulate the entire thing instead.
        if ((l_param & PRF_CHILDREN) != 0) {
            HWND child = ::GetWindow(hwnd_, GW_CHILD);
            while (child != NULL) {
                RECT child_area = { 0 };
                ::GetWindowRect(child, &child_area);
                ::MapWindowPoints(HWND_DESKTOP, hwnd_, reinterpret_cast<LPPOINT>(&child_area), 2);
                ::SetWindowOrgEx(hdc, -child_area.left, -child_area.top, NULL);
                // NOTE: We use WM_PRINT here rather than the expected WM_PRINTCLIENT
                //       since the latter will not print the nonclient correctly for
                //       EDIT controls.
                ::SendMessage(child, WM_PRINT, w_param, l_param | PRF_NONCLIENT);
                child = ::GetWindow(child, GW_HWNDNEXT);
            }
        }
        ::RestoreDC(hdc, save);
        break;
    }
    case WM_CLOSE:
    {
        /* Sent as a signal that a window or an application should terminate. */
        DuiWindowEvent evt;
        evt.type = kClose_WindowEvent;
        DuiWindowEventResult evt_ret;
        bool handled = HandleEvent(&evt, evt_ret);

        /*If an application processes this message, it should return zero.*/
        result = 0;
        if (!handled) {
            if (!prev_ && !next_) {
                DuiApplication::Quit();
            }
        } 
        else {
            return result;
        }
        break;
    }
    //case WM_QUIT:
    //{
    //    // The WM_QUIT message is not associated with a window and therefore will never be received through a window's window procedure. It is retrieved only by the GetMessage or PeekMessage functions.
    //    result = 0;
    //    break;
    //}
    case WM_ERASEBKGND:
    {
        /**
         * Sent when the window background must be erased (for example, when a window is resized). 
         * The message is sent to prepare an invalidated portion of a window for painting.
         */
        DuiWindowEvent evt;
        RECT client_area;
        ::GetClientRect(hwnd_, &client_area);
        /* We'll do our own drawing, prevent flicker */
        evt.type = kErasebkgnd_WindowEvent;
        DuiWindowEventResult evt_ret;
        HDC hdc = (HDC)w_param;
        AutoGdiRealCanvas auto_canvas(hdc, false);
        evt.canvas = auto_canvas.GetCanvas();
        bool handled = HandleEvent(&evt, evt_ret);
        /* An application should return nonzero if it erases the background; otherwise, it should return zero. */
        result = 1;
        if (handled) {
            return result;
        }
        break;
    }
    //case WM_SYSCOLORCHANGE:
    //{
    //    break;
    //}
    case WM_SHOWWINDOW:
    {
        /* Sent to a window when the window is about to be hidden or shown. */
        DuiWindowEvent evt;
        if (w_param) {
            evt.type = kShown_WindowEvent;
        }
        else {
            evt.type = kHidden_WindowEvent;
        }
        DuiWindowEventResult evt_ret;
        bool handled = HandleEvent(&evt, evt_ret);
        /* If an application processes this message, it should return zero. */
        break;
    }
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
        break;
    case WM_MOUSEHOVER:
    {
        result = 0;
        mouse_tracking_ = false;
        POINT pt = { LOWORD(l_param), HIWORD(l_param) };
        DuiWindowEvent evt;
        evt.type = kMouseHover_WindowEvent;
        evt.location = { pt.x, pt.y };
        DuiWindowEventResult evt_ret;
        bool handled = HandleEvent(&evt, evt_ret);
        if (handled) {
            return result;
        }
        break;
    }
    case WM_MOUSELEAVE:
    {
        POINT pt = { 0 };
        if (mouse_tracking_) {
            ::GetCursorPos(&pt);
        }
        DuiWindowEvent evt;
        evt.type = kMouseLeave_WindowEvent;
        evt.location = { pt.x, pt.y };
        DuiWindowEventResult evt_ret;
        bool handled = HandleEvent(&evt, evt_ret);
        mouse_tracking_ = false;
        if (handled) {
            return result;
        }
        break;
    }
    case WM_MOUSEMOVE:
    {
        if (!mouse_tracking_) {
            TRACKMOUSEEVENT tme = { 0 };
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_HOVER | TME_LEAVE;
            tme.hwndTrack = hwnd_;
            tme.dwHoverTime = hover_time_;
            _TrackMouseEvent(&tme);
            mouse_tracking_ = true;
        }
        POINT pt = { LOWORD(l_param), HIWORD(l_param) };
        DuiWindowEvent evt;
        evt.type = kMouseMove_WindowEvent;
        evt.location = { pt.x, pt.y };
        DuiWindowEventResult evt_ret;
        bool handled = HandleEvent(&evt, evt_ret);
        result = 0;
        if (handled) {
            return result;
        }
        break;
    }
    case WM_SETCURSOR:
    {
        /* Sent to a window if the mouse causes the cursor to move within a window and mouse input is not captured. */
        DuiWindowEvent evt;
        uint16_t hittest;
        evt.type = kSetCursor_WindowEvent;
        hittest = LOWORD(l_param);
        if (hittest == HTCLIENT) {
            result = TRUE;
        }
        else {
            result = FALSE;
        }
        DuiWindowEventResult evt_ret;
        bool handled = HandleEvent(&evt, evt_ret);
        if (handled) {
            return result;
        }
        break;
    }
    case WM_GETMINMAXINFO:
    {
        /* Sent to a window when the size or position of the window is about to change. 
         * An application can use this message to override the window's default maximized size and position, 
         * or its default minimum or maximum tracking size.
         */
        MINMAXINFO *info;
        int x, y;
        int w, h;
        int min_w, min_h;
        int max_w, max_h;
        int style;
        BOOL menu;
        BOOL constrain_max_size;

        /* If this is an expected size change, allow it */
        if (expected_resize_) {
            break;
        }

        /* Get the current position of our window */
        RECT size;
        GetWindowRect(hwnd_, &size);
        x = size.left;
        y = size.top;

        {
            /* Calculate current size of our window */
            DuiSize sz = GetSize(); w = sz.width; h = sz.height;
            sz = GetMinimumSize(); min_w = sz.width; min_h = sz.height;
            sz = GetMaximumSize(); max_w = sz.width; max_h = sz.height;
        }

        /* Store in min_w and min_h difference between current size and minimal
        size so we don't need to call AdjustWindowRectEx twice */
        min_w -= w;
        min_h -= h;
        if (max_w && max_h) {
            max_w -= w;
            max_h -= h;
            constrain_max_size = TRUE;
        }
        else {
            constrain_max_size = FALSE;
        }

        size.top = 0;
        size.left = 0;
        size.bottom = h;
        size.right = w;

        style = GetWindowLong(hwnd_, GWL_STYLE);
        /* DJM - according to the docs for GetMenu(), the
        return value is undefined if hwnd is a child window.
        Apparently it's too difficult for MS to check
        inside their function, so I have to do it here.
        */
        menu = (style & WS_CHILDWINDOW) ? FALSE : (GetMenu(hwnd_) != NULL);
        if (!(flags_ & kLayered_Window)) {
            AdjustWindowRectEx(&size, style, menu, 0);
        }
        w = size.right - size.left;
        h = size.bottom - size.top;

        /* Fix our size to the current size */
        info = (MINMAXINFO *)l_param;
        if (flags_ & kResizable_Window) {
            info->ptMinTrackSize.x = w + min_w;
            info->ptMinTrackSize.y = h + min_h;
            if (constrain_max_size) {
                info->ptMaxTrackSize.x = w + max_w;
                info->ptMaxTrackSize.y = h + max_h;
            }
        }
        else {
            info->ptMaxSize.x = w;
            info->ptMaxSize.y = h;
            info->ptMaxPosition.x = x;
            info->ptMaxPosition.y = y;
            info->ptMinTrackSize.x = w;
            info->ptMinTrackSize.y = h;
            info->ptMaxTrackSize.x = w;
            info->ptMaxTrackSize.y = h;
        }

        result = 0;
        return result;
        break;
    }
    case WM_WINDOWPOSCHANGING:
    {
        /* Sent to a window whose size, position, or place in the Z order is about to change as a 
         * result of a call to the SetWindowPos function or another window-management function.
         */
        LPWINDOWPOS window_pos = (LPWINDOWPOS)(l_param);
        //if (!(window_pos->flags & SWP_NOSIZE)) {
        //    width_ = window_pos->cx;
        //    height_ = window_pos->cy;
        //}

        /*If an application processes this message, it should return zero.*/
        // result = 0;

        if (expected_resize_) {
            result = 0;
        }
        break;
    }
    case WM_WINDOWPOSCHANGED:
    {
        /* Sent to a window whose size, position, or place in the Z order has changed as a 
         * result of a call to the SetWindowPos function or another window-management function. 
         */

        LPWINDOWPOS window_pos = (LPWINDOWPOS)(l_param);

        if (!(window_pos->flags & SWP_NOMOVE)) {
            DuiWindowEventResult evt_ret;
            DuiWindowEvent evt;
            evt.type = kMoved_WindowEvent;
            evt.location.x = window_pos->x;
            evt.location.y = window_pos->y;
            bool handled = HandleEvent(&evt, evt_ret);
        }
        if (!(window_pos->flags & SWP_NOSIZE)) {
            DuiWindowEventResult evt_ret;
            DuiWindowEvent evt;
            evt.type = kResized_WindowEvent;
            evt.size.width = window_pos->cx;
            evt.size.height = window_pos->cy;
            bool handled = HandleEvent(&evt, evt_ret);
        }

        /* Forces a WM_PAINT event */
        //::InvalidateRect(hwnd_, NULL, FALSE);

        /* If an application processes this message, it should return zero. */
        result = 0;

        //return result;
        break;
    }
    case WM_NCHITTEST:
    {
        /**
         * Sent to a window in order to determine what part of the window corresponds to a particular screen coordinate. 
         * This can happen, for example, when the cursor moves, when a mouse button is pressed or released, 
         * or in response to a call to a function such as WindowFromPoint. If the mouse is not captured, 
         * the message is sent to the window beneath the cursor. Otherwise, the message is sent to 
         * the window that has captured the mouse.
         */
        if (IsLayered()) {
            POINT winpoint = { (int)LOWORD(l_param), (int)HIWORD(l_param) };
            if (::ScreenToClient(hwnd_, &winpoint)) {
                const DuiPoint point = { (int)winpoint.x, (int)winpoint.y };

                // mCaptionHeight is the default size of the NC area at
                // the top of the window. If the window has a caption,
                // the size is calculated as the sum of:
                //      SM_CYFRAME        - The thickness of the sizing border
                //                          around a resizable window
                //      SM_CXPADDEDBORDER - The amount of border padding
                //                          for captioned windows
                //      SM_CYCAPTION      - The height of the caption area
                //
                // If the window does not have a caption, mCaptionHeight will be equal to
                // `GetSystemMetrics(SM_CYFRAME)`
                int frame_height = GetSystemMetrics(SM_CYFRAME);
                int frame_width = GetSystemMetrics(SM_CXFRAME);
                int padding_border = ::GetSystemMetrics(SM_CXPADDEDBORDER);
                int caption_height = ::GetSystemMetrics(SM_CYCAPTION);

                DuiWindowEventResult evt_ret;
                DuiWindowEvent evt;
                evt.type = kHitTest_WindowEvent;
                evt_ret = kNormal_HitTest;
                bool handled = HandleEvent(&evt, evt_ret);
                if (handled) {
                    DuiHitTestResult ret = (DuiHitTestResult)evt_ret;
                    switch (ret) {
                    case kDraggable_HitTest:
                        return HTCAPTION;
                    case kResizeTopleft_HitTest:
                        return HTTOPLEFT;
                    case kResizeTop_HitTest:
                        return HTTOP;
                    case kResizeTopright_HitTest:
                        return HTTOPRIGHT;
                    case kResizeRight_HitTest:
                        return HTRIGHT;
                    case kResizeBottomright_HitTest:
                        return HTBOTTOMRIGHT;
                    case kResizeBottom_HitTest:
                        return HTBOTTOM;
                    case kResizeBottomleft_HitTest:
                        return HTBOTTOMLEFT;
                    case kResizeLeft_HitTest:
                        return HTLEFT;
                    case kNormal_HitTest:
                        return HTCLIENT;
                    }
                }
                else {
                    DuiRect caption_area = { frame_width, frame_height, width_ - frame_width, frame_height + caption_height };
                    DuiRect client_area = { frame_width, frame_height + caption_height, width_ - frame_width, height_ - frame_height };
                    DuiRect left_area = { 0, frame_height, frame_width, height_ - frame_height };
                    DuiRect right_area = { width_ - frame_width, frame_height, width_, height_ - frame_height };
                    DuiRect top_area = { frame_width, 0, width_ - frame_width, frame_height };
                    DuiRect bottom_area = { frame_width, height_ - frame_height, width_ - frame_width, height_ };
                    DuiRect lt_area = { 0, 0, frame_width, frame_height };
                    DuiRect rt_area = { width_ - frame_width, 0, width_, frame_height };
                    DuiRect lb_area = { 0, height_ - frame_height, frame_width, height_ };
                    DuiRect rb_area = { width_ - frame_width, height_ - frame_height, width_, height_ };
                    if (caption_area.Contains(point)) {
                        return HTCAPTION;
                    }
                    else if (left_area.Contains(point)) {
                        return HTLEFT;
                    }
                    else if (right_area.Contains(point)) {
                        return HTRIGHT;
                    }
                    else if (top_area.Contains(point)) {
                        return HTTOP;
                    }
                    else if (bottom_area.Contains(point)) {
                        return HTBOTTOM;
                    }
                    else if (lt_area.Contains(point)) {
                        return HTTOPLEFT;
                    }
                    else if (rt_area.Contains(point)) {
                        return HTTOPRIGHT;
                    }
                    else if (lb_area.Contains(point)) {
                        return HTBOTTOMLEFT;
                    }
                    else if (rb_area.Contains(point)) {
                        return HTBOTTOMRIGHT;
                    }
                    else if (client_area.Contains(point)) {
                        return HTCLIENT;
                    }
                    else {
                        return HTCLIENT;
                    }
                }
            }
        }
        break;
    }
    case WM_NCCALCSIZE:
    {
        /* Sent when the size and position of a window's client area must be calculated.  */
        if (IsLayered()) {
            result = 0;
            /* If the wParam parameter is FALSE, the application should return zero. */
            /* If wParam is TRUE, the application should return zero or a combination of the following values. */
            /* If wParam is TRUE and an application returns zero, the old client area is preserved and is aligned with the upper-left corner of the new client area. */
            return result;
        }
        break;
    }
    case WM_NCPAINT:
    {
        /* The WM_NCPAINT message is sent to a window when its frame must be painted. */
        if (IsLayered()) {
            /* An application returns zero if it processes this message. */
            result = 0;
            return result;
        }
        break;
    }
    case WM_NCACTIVATE:
    {
        /* Sent to a window when its nonclient area needs to be changed to indicate an active or inactive state. */
        if (IsLayered()) {
            if (!::IsIconic(hwnd_)) {
                /* When the wParam parameter is FALSE, an application should return TRUE to indicate 
                 * that the system should proceed with the default processing, or it should return 
                 * FALSE to prevent the change. When wParam is TRUE, the return value is ignored. */
                result = (w_param == 0) ? TRUE : FALSE;
                return result;
            }
        }
        break;
    }
    case WM_TIMER:
    {
        /**
         * Posted to the installing thread's message queue when a timer expires. 
         * The message is posted by the GetMessage or PeekMessage function.
         */
        /**
         * The WM_TIMER message is a low-priority message. 
         * The GetMessage and PeekMessage functions post this message only when no other higher-priority messages are in the thread's message queue.
         */
        if (LOWORD(w_param) == kLayeredUpdateTimerID) {
            if (IsLayered() && !layered_update_area_.IsEmpty()) {
                if (!::IsIconic(hwnd_)) {
                    ::PostMessage(hwnd_, WM_PAINT, 0, 0L);
                }
                break;
            }
        }
        break;
    }
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORSTATIC:
    {
        // Refer To: http://msdn.microsoft.com/en-us/library/bb761691(v=vs.85).aspx
        // Read-only or disabled edit controls do not send the WM_CTLCOLOREDIT message; instead, they send the WM_CTLCOLORSTATIC message.
        if (l_param == 0) break;
        HWND child_hwnd = (HWND)l_param;
        result = ::SendMessage(child_hwnd, OCM__BASE + msg, w_param, l_param);
        return result;
    }
    break;
    }

    return ::CallWindowProc(old_window_proc_, hwnd_, msg, w_param, l_param);
}
void DuiRealWindowWin::OnFinalMessage(HWND hwnd)
{
    DuiWindowPriv window_priv(owner_);
    window_priv.OnDestoryed();
}

LRESULT CALLBACK DuiRealWindowWin::WindowProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param)
{
    DuiRealWindowWin * this_ptr = nullptr;
    if (msg == WM_NCCREATE) {
        LPCREATESTRUCT create_struct_ = reinterpret_cast<LPCREATESTRUCT>(l_param);
        this_ptr = static_cast<DuiRealWindowWin*>(create_struct_->lpCreateParams);
        this_ptr->hwnd_ = hwnd;
        ::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(this_ptr));
    }
    else {
        bool ret = ::IsWindow(hwnd) == TRUE;
        this_ptr = reinterpret_cast<DuiRealWindowWin*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (msg == WM_NCDESTROY && this_ptr) {
            LRESULT result = ::CallWindowProc(this_ptr->old_window_proc_, hwnd, msg, w_param, l_param);
            ::SetWindowLongPtr(this_ptr->hwnd_, GWLP_USERDATA, 0L);
            if (this_ptr->subclassed_) {
                this_ptr->Unsubclass();
            }
            this_ptr->hwnd_ = nullptr;
            this_ptr->OnFinalMessage(hwnd);
            return result;
        }
    }
    if (this_ptr) {
        return this_ptr->HandleMessage(msg, w_param, l_param);
    }
    else {
        return ::DefWindowProc(hwnd, msg, w_param, l_param);
    }
}
LRESULT CALLBACK DuiRealWindowWin::ControlProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param)
{
    DuiRealWindowWin* this_ptr = nullptr;
    if (msg == WM_NCCREATE) {
        LPCREATESTRUCT create_struct_ = reinterpret_cast<LPCREATESTRUCT>(l_param);
        this_ptr = static_cast<DuiRealWindowWin*>(create_struct_->lpCreateParams);
        ::SetProp(hwnd, kDuiWindowPropName, (HANDLE)this_ptr);
        this_ptr->hwnd_ = hwnd;
    }
    else {
        this_ptr = reinterpret_cast<DuiRealWindowWin*>(::GetProp(hwnd, kDuiWindowPropName));
        if (msg == WM_NCDESTROY && this_ptr) {
            LRESULT result = ::CallWindowProc(this_ptr->old_window_proc_, hwnd, msg, w_param, l_param);
            if (this_ptr->subclassed_) {
                this_ptr->Unsubclass();
            }
            ::SetProp(hwnd, kDuiWindowPropName, (HANDLE)nullptr);
            this_ptr->hwnd_ = nullptr;
            this_ptr->OnFinalMessage(hwnd);
            return result;
        }
    }
    if (this_ptr) {
        return this_ptr->HandleMessage(msg, w_param, l_param);
    }
    else {
        return ::DefWindowProc(hwnd, msg, w_param, l_param);
    }
}
