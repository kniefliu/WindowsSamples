#include "qui/core/qui_window.h"

// headers in src
#include "qui_window_p.h"
#include "qui_macro.h"

#include <windowsx.h>

HINSTANCE QuiGetInstance()
{
    return (HINSTANCE)QuiApplication::GetInstance();
}

QuiWindow::QuiWindow()
    : priv_(new QuiWindowPrivate(this))
{

}
QuiWindow::~QuiWindow()
{
    delete priv_;
}

QuiWindowHandle QuiWindow::GetHWND() const
{
    return priv_->hwnd_;
}
QuiWindow::operator QuiWindowHandle() const
{
    return priv_->hwnd_;
}

QuiWindowHandle QuiWindow::Create(QuiWindowHandle parent, const wchar_t* window_name, QuiWindowStyle window_style,
    QuiWindowExStyle window_ex_style, const QuiRect& area)
{
    return Create(parent, window_name, window_style, window_ex_style, area.left, area.top, area.Width(), area.Height());
}
QuiWindowHandle QuiWindow::Create(QuiWindowHandle parent, const wchar_t* window_name, QuiWindowStyle window_style,
    QuiWindowExStyle window_ex_style,
    int x, int y, int w, int h)
{
    return priv_->Create((HWND)parent, window_name, window_style, window_ex_style, x, y, w, h);
}
QuiWindowHandle QuiWindow::CreateQuiWindow(QuiWindowHandle parent, const wchar_t* window_name, QuiWindowStyle window_style,
    QuiWindowExStyle dwExStyle)
{
    return Create(parent, window_name, window_style, 0, 0, 0, 0);
}

QuiWindowHandle QuiWindow::Subclass(QuiWindowHandle win_handle)
{
    return priv_->Subclass((HWND)win_handle);
}
void QuiWindow::Unsubclass()
{
    priv_->Unsubclass();
}

void QuiWindow::ShowWindow(bool show, bool take_focus)
{
    if (!IsWindow(priv_->hwnd_)) {
        return;
    }
    ::ShowWindow(priv_->hwnd_, show ? (take_focus ? SW_SHOWNORMAL : SW_SHOWNOACTIVATE) : SW_HIDE);
}
void QuiWindow::ShowModal()
{
    // TODO: 
}
void QuiWindow::Close()
{
    if (!IsWindow(priv_->hwnd_)) {
        return;
    }
    // TODO support n_ret
    ::PostMessage(priv_->hwnd_, WM_CLOSE, (WPARAM)0, 0L);
}
void QuiWindow::CenterWindow()
{
    RECT client_area = { 0 };
    ::GetClientRect(priv_->hwnd_, &client_area);
    RECT work_area = { 0 };
    RECT center_area = { 0 };
    HWND hwnd = priv_->hwnd_;
    HWND center_hwnd = ::GetWindowOwner(hwnd);
    if (center_hwnd != NULL)
        hwnd = center_hwnd;

    MONITORINFO monitor_info = {};
    monitor_info.cbSize = sizeof(monitor_info);
    ::GetMonitorInfo(::MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &monitor_info);
    work_area = monitor_info.rcWork;

    int client_w = client_area.right - client_area.left;
    int client_h = client_area.bottom - client_area.top;

    // Find dialog's upper left based on rcCenter
    int left = (center_area.left + center_area.right) / 2 - client_w / 2;
    int top = (center_area.top + center_area.bottom) / 2 - client_h / 2;

    // The dialog is outside the screen, move it inside
    if (left < work_area.left) left = work_area.left;
    else if (left + client_w > work_area.right) left = work_area.right - client_w;
    if (top < work_area.top) top = work_area.top;
    else if (top + client_h > work_area.bottom) top = work_area.bottom - client_h;
    ::SetWindowPos(priv_->hwnd_, NULL, left, top, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}
void QuiWindow::SetIcon(QuiWindowIconID icon_id)
{

}
void QuiWindow::ResizeClient(int w, int h)
{

}

QuiWindowClassStyle QuiWindow::GetClassStyle() const
{
    return CS_VREDRAW | CS_HREDRAW;
}

QuiWindowLResult QuiWindow::HandleMessage(QuiWindowMessageID uMsg, QuiWindowWParam w_param, QuiWindowLParam l_param)
{
    WPARAM wparam = (WPARAM)w_param;
    LPARAM lparam = (LPARAM)l_param;
    LRESULT l_res = ::CallWindowProc(priv_->old_wndproc_, priv_->hwnd_, uMsg, wparam, lparam);
    QuiWindowLResult res = (QuiWindowLResult)l_res;
    return res;
}
void QuiWindow::OnFinalMessage(QuiWindowHandle hWnd)
{

}

QuiWindow::QuiWindow(QuiWindowPrivate * priv)
    : priv_(priv)
{

}


// private class implement here
QuiWindowPrivate::QuiWindowPrivate(QuiWindow * owner)
    : owner_(owner)
    , hwnd_(nullptr)
    , old_wndproc_(::DefWindowProc)
    , subclassed_(false)
{

}
bool QuiWindowPrivate::RegisterWindowClass()
{
    WNDCLASS wnd_cls = { 0 };
    wnd_cls.style = owner_->GetClassStyle();
    wnd_cls.cbClsExtra = 0;
    wnd_cls.cbWndExtra = 0;
    wnd_cls.hIcon = NULL;
    wnd_cls.lpfnWndProc = QuiWindowPrivate::WindowProc;
    wnd_cls.hInstance = QuiGetInstance();
    wnd_cls.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wnd_cls.hbrBackground = NULL;
    wnd_cls.lpszMenuName = NULL;
    wnd_cls.lpszClassName = owner_->GetWindowClassName();
    ATOM ret = ::RegisterClass(&wnd_cls);

    return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}
bool QuiWindowPrivate::RegisterSuperclass()
{
    WNDCLASSEX wnd_cls = { 0 };
    wnd_cls.cbSize = sizeof(WNDCLASSEX);
    if (!::GetClassInfoEx(NULL, owner_->GetSuperClassName(), &wnd_cls)) {
        if (!::GetClassInfoEx(QuiGetInstance(), owner_->GetSuperClassName(), &wnd_cls)) {
            return NULL;
        }
    }
    old_wndproc_ = wnd_cls.lpfnWndProc;
    wnd_cls.lpfnWndProc = QuiWindowPrivate::ControlProc;
    wnd_cls.hInstance = QuiGetInstance();
    wnd_cls.lpszClassName = owner_->GetWindowClassName();
    ATOM ret = ::RegisterClassEx(&wnd_cls);

    return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

HWND QuiWindowPrivate::Create(HWND parent, const wchar_t* window_name, DWORD window_style,
    DWORD window_ex_style,
    int x, int y, int w, int h)
{
    const wchar_t * super_class_name = owner_->GetSuperClassName();
    if (super_class_name != nullptr && !RegisterSuperclass()) return nullptr;
    const wchar_t * window_class_name = owner_->GetWindowClassName();
    if (window_class_name != nullptr && !RegisterWindowClass()) return nullptr;
    hwnd_ = ::CreateWindowEx(window_ex_style, window_class_name, window_name, window_style, x, y, w, h, parent, 0,
        QuiGetInstance(), this);
    return hwnd_;
}

HWND QuiWindowPrivate::Subclass(HWND hWnd)
{
    return nullptr;
}
void QuiWindowPrivate::Unsubclass()
{

}

LRESULT QuiWindowPrivate::HandleMessage(UINT msg, WPARAM w_param, LPARAM l_param)
{
    QuiWindowWParam wparam = (QuiWindowWParam)w_param;
    QuiWindowWParam lparam = (QuiWindowLParam)l_param;
    QuiWindowLResult l_res = owner_->HandleMessage(msg, wparam, lparam);
    LRESULT res = (LRESULT)l_res;
    return res;
}

LRESULT CALLBACK QuiWindowPrivate::WindowProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param)
{
    QuiWindowPrivate * window = nullptr;
    if (msg == WM_NCCREATE) {
        LPCREATESTRUCT create_info = reinterpret_cast<LPCREATESTRUCT>(l_param);
        window = static_cast<QuiWindowPrivate *>(create_info->lpCreateParams);
        window->hwnd_ = hwnd;
        ::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(window));
    }
    else {
        window = reinterpret_cast<QuiWindowPrivate *>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (msg == WM_NCDESTROY && window != nullptr) {
            LRESULT l_res = ::CallWindowProc(window->old_wndproc_, hwnd, msg, w_param, l_param);
            ::SetWindowLongPtr(hwnd, GWLP_USERDATA, 0L);
            if (window->subclassed_) {
                window->Unsubclass();
                window->hwnd_ = nullptr;
                window->owner_->OnFinalMessage(hwnd);
                return l_res;
            }
        }
    }
    if (window != nullptr) {
        return window->HandleMessage(msg, w_param, l_param);
    }
    else {
        return ::DefWindowProc(hwnd, msg, w_param, l_param);
    }
}
LRESULT CALLBACK QuiWindowPrivate::ControlProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param)
{
    QuiWindowPrivate * window = nullptr;
    if (msg == WM_NCCREATE) {
        LPCREATESTRUCT create_info = reinterpret_cast<LPCREATESTRUCT>(l_param);
        window = static_cast<QuiWindowPrivate *>(create_info->lpCreateParams);
        window->hwnd_ = hwnd;
        ::SetProp(hwnd, L"WndX", (HANDLE)(window));
    }
    else {
        window = reinterpret_cast<QuiWindowPrivate *>(::GetProp(hwnd, L"WndX"));
        if (msg == WM_NCDESTROY && window != nullptr) {
            LRESULT l_res = ::CallWindowProc(window->old_wndproc_, hwnd, msg, w_param, l_param);
            ::SetProp(hwnd, L"WndX", NULL);
            if (window->subclassed_) {
                window->Unsubclass();
                window->hwnd_ = nullptr;
                window->owner_->OnFinalMessage(hwnd);
                return l_res;
            }
        }
    }
    if (window != nullptr) {
        return window->HandleMessage(msg, w_param, l_param);
    }
    else {
        return ::DefWindowProc(hwnd, msg, w_param, l_param);
    }
}

