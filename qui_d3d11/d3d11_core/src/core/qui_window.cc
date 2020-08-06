#include "qui/core/qui_window.h"

// headers in src
#include "qui_macro.h"

#include <windowsx.h>

HINSTANCE QuiGetInstance()
{
    return (HINSTANCE)QuiApplication::GetInstance();
}

QuiWindow::QuiWindow()
    : hwnd_(nullptr)
    , old_wndproc_(::DefWindowProc)
    , subclassed_(false)
{

}
QuiWindow::~QuiWindow()
{

}

HWND QuiWindow::GetHWND() const
{
    return hwnd_;
}
QuiWindow::operator HWND() const
{
    return hwnd_;
}

HWND QuiWindow::Create(HWND parent, const wchar_t* window_name, DWORD window_style, DWORD window_ex_style, const QuiRect& area)
{
    return Create(parent, window_name, window_style, window_ex_style, area.left, area.top, area.Width(), area.Height());
}
HWND QuiWindow::Create(HWND parent, const wchar_t* window_name, DWORD window_style, DWORD window_ex_style, int x, int y, int w, int h)
{
    const wchar_t * super_class_name = GetSuperClassName();
    if (super_class_name != nullptr && !RegisterSuperclass()) return nullptr;
    const wchar_t * window_class_name = GetWindowClassName();
    if (window_class_name != nullptr && !RegisterWindowClass()) return nullptr;
    hwnd_ = ::CreateWindowEx(window_ex_style, window_class_name, window_name, window_style, x, y, w, h, parent, 0,
        QuiGetInstance(), this);
    return hwnd_;
}
HWND QuiWindow::CreateQuiWindow(HWND parent, const wchar_t* window_name, DWORD window_style, DWORD dwExStyle)
{
    return Create(parent, window_name, window_style, 0, 0, 0, 0);
}

HWND QuiWindow::Subclass(HWND win_handle)
{
    return nullptr;
}
void QuiWindow::Unsubclass()
{
    
}

void QuiWindow::ShowWindow(bool show, bool take_focus)
{
    if (!IsWindow(hwnd_)) {
        return;
    }
    ::ShowWindow(hwnd_, show ? (take_focus ? SW_SHOWNORMAL : SW_SHOWNOACTIVATE) : SW_HIDE);
}
void QuiWindow::ShowModal()
{
    // TODO: 
}
void QuiWindow::Close()
{
    if (!IsWindow(hwnd_)) {
        return;
    }
    // TODO support n_ret
    ::PostMessage(hwnd_, WM_CLOSE, (WPARAM)0, 0L);
}
void QuiWindow::CenterWindow()
{
    RECT client_area = { 0 };
    ::GetClientRect(hwnd_, &client_area);
    RECT work_area = { 0 };
    RECT center_area = { 0 };
    HWND hwnd = hwnd_;
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
    ::SetWindowPos(hwnd_, NULL, left, top, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}
void QuiWindow::SetIcon(UINT icon_id)
{

}
void QuiWindow::ResizeClient(int w, int h)
{
    QuiAssertResult(::IsWindow(hwnd_));
    RECT area = { 0 };
    if (!::GetClientRect(hwnd_, &area)) return;
    if (w != -1) area.right = w;
    if (h != -1) area.bottom = h;
    if (!::AdjustWindowRectEx(&area, GetWindowStyle(hwnd_), (!(GetWindowStyle(hwnd_) & WS_CHILD) && (::GetMenu(hwnd_) != nullptr)), GetWindowExStyle(hwnd_))) return;
    ::SetWindowPos(hwnd_, nullptr, 0, 0, area.right - area.left, area.bottom - area.top, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

DWORD QuiWindow::GetClassStyle() const
{
    return CS_VREDRAW | CS_HREDRAW;
}

LRESULT QuiWindow::HandleMessage(UINT uMsg, WPARAM w_param, LPARAM l_param)
{
    WPARAM wparam = (WPARAM)w_param;
    LPARAM lparam = (LPARAM)l_param;
    LRESULT l_res = ::CallWindowProc(old_wndproc_, hwnd_, uMsg, wparam, lparam);
    return l_res;
}
void QuiWindow::OnFinalMessage(HWND hWnd)
{

}

bool QuiWindow::RegisterWindowClass()
{
    WNDCLASS wnd_cls = { 0 };
    wnd_cls.style = GetClassStyle();
    wnd_cls.cbClsExtra = 0;
    wnd_cls.cbWndExtra = 0;
    wnd_cls.hIcon = NULL;
    wnd_cls.lpfnWndProc = WindowProc;
    wnd_cls.hInstance = QuiGetInstance();
    wnd_cls.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wnd_cls.hbrBackground = NULL;
    wnd_cls.lpszMenuName = NULL;
    wnd_cls.lpszClassName = GetWindowClassName();
    ATOM ret = ::RegisterClass(&wnd_cls);

    return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}
bool QuiWindow::RegisterSuperclass()
{
    WNDCLASSEX wnd_cls = { 0 };
    wnd_cls.cbSize = sizeof(WNDCLASSEX);
    if (!::GetClassInfoEx(NULL, GetSuperClassName(), &wnd_cls)) {
        if (!::GetClassInfoEx(QuiGetInstance(), GetSuperClassName(), &wnd_cls)) {
            return NULL;
        }
    }
    old_wndproc_ = wnd_cls.lpfnWndProc;
    wnd_cls.lpfnWndProc = ControlProc;
    wnd_cls.hInstance = QuiGetInstance();
    wnd_cls.lpszClassName = GetWindowClassName();
    ATOM ret = ::RegisterClassEx(&wnd_cls);

    return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

LRESULT CALLBACK QuiWindow::WindowProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param)
{
    QuiWindow * window = nullptr;
    if (msg == WM_NCCREATE) {
        LPCREATESTRUCT create_info = reinterpret_cast<LPCREATESTRUCT>(l_param);
        window = static_cast<QuiWindow *>(create_info->lpCreateParams);
        window->hwnd_ = hwnd;
        ::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(window));
    }
    else {
        window = reinterpret_cast<QuiWindow *>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (msg == WM_NCDESTROY && window != nullptr) {
            LRESULT l_res = ::CallWindowProc(window->old_wndproc_, hwnd, msg, w_param, l_param);
            ::SetWindowLongPtr(hwnd, GWLP_USERDATA, 0L);
            if (window->subclassed_) {
                window->Unsubclass();
                window->hwnd_ = nullptr;
                window->OnFinalMessage(hwnd);
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
LRESULT CALLBACK QuiWindow::ControlProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param)
{
    QuiWindow * window = nullptr;
    if (msg == WM_NCCREATE) {
        LPCREATESTRUCT create_info = reinterpret_cast<LPCREATESTRUCT>(l_param);
        window = static_cast<QuiWindow *>(create_info->lpCreateParams);
        window->hwnd_ = hwnd;
        ::SetProp(hwnd, L"WndX", (HANDLE)(window));
    }
    else {
        window = reinterpret_cast<QuiWindow *>(::GetProp(hwnd, L"WndX"));
        if (msg == WM_NCDESTROY && window != nullptr) {
            LRESULT l_res = ::CallWindowProc(window->old_wndproc_, hwnd, msg, w_param, l_param);
            ::SetProp(hwnd, L"WndX", NULL);
            if (window->subclassed_) {
                window->Unsubclass();
                window->hwnd_ = nullptr;
                window->OnFinalMessage(hwnd);
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

