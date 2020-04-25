#ifndef DUI_CORE_REAL_WINDOW_WIN_H_
#define DUI_CORE_REAL_WINDOW_WIN_H_

#include "core/dui_real_window.h"

#include <Windows.h>

#include <vector>

struct NativeWindow {
    DuiControlBase * controller;
    DuiWindowHandle native_handle;

    bool operator==(const NativeWindow& other) {
        return this->native_handle == other.native_handle;
    }
};

class GdiSurface;
class DuiRealWindowWin : public DuiRealWindow {
public:
    static void SetInstance(HINSTANCE inst);
    static HINSTANCE GetInstance();

public:
    DuiRealWindowWin(DuiWindow * owner);
    ~DuiRealWindowWin();

public:
    HWND GetHWND() const;
    operator HWND() const;

    bool RegisterWindowClass();
    bool RegisterSuperclass();

public:
    DuiPoint GetLocationOnScreen() const final;
    void SetSize(const DuiSize& sz) final;
    void SetMinimumSize(const DuiSize& sz) final;
    void SetMaximumSize(const DuiSize& sz) final;

    DuiPoint GetMousePosition() final;

    void Invalidate(const DuiRect* repaint_area = nullptr) final;

    int GetNativeWindowCount() const final;
    DuiRect GetNativeWindowRect(DuiWindowHandle child_window) const final;
    bool AddNativeWindow(DuiControlBase * controller, DuiWindowHandle child_window) final;
    bool RemoveNativeWindow(DuiWindowHandle child_window) final;
    void PaintNativeWindow(DuiCanvas * canvas, const DuiRect& repaint_area);

    DuiAlpha GetOpacity() const final;
    void SetOpacity(DuiAlpha opacity) final;
    bool IsLayered() final;
    void SetLayered(bool layered) final;
    DuiAlpha GetLayeredOpacity() final;
    void SetLayeredOpacity(DuiAlpha opacity) final;

protected:
    void SetWindowPositionInternal(UINT flags);

protected:
    DuiWindowHandle OnCreate(DuiRealWindow * parent) override;
    DuiWindowHandle OnCreateFrom(DuiWindowHandle extern_handle) override;
    DuiWindowHandle OnGetHandle() override;
    void OnSetTitle() override;
    void OnAdd(DuiRealWindow * child) override;
    void OnShow() override;
    void OnHide() override;
    void OnRaise() override;
    void OnMaximize() override;
    void OnMinimize() override;
    void OnRestore() override;
    void OnSetLocation() final;
    void SetFullscreen(VideoDisplay * display, bool fullscreen = true) override;

public:
    LRESULT SendMessage(UINT msg, WPARAM w_param = 0, LPARAM l_param = 0);
    LRESULT PostMessage(UINT msg, WPARAM w_param = 0, LPARAM l_param = 0);

protected:
    HWND Create(HWND hwnd_parent, LPCTSTR title, DWORD style, DWORD ex_style, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int w = CW_USEDEFAULT, int h = CW_USEDEFAULT, HMENU menu = NULL);
    HWND Subclass(HWND hwnd);
    void Unsubclass();

    DWORD GetWindowStyle(DuiWindowFlags flag) const;
    DWORD GetWindowExStyle(DuiWindowFlags flag) const;

    virtual bool OnGetWindowStyle(DWORD& style) const { return false; }
    virtual bool OnGetWindowExStyle(DWORD& ex_style) const { return false; }

    virtual const wchar_t * GetWindowClassName();
    virtual const wchar_t * GetSuperClassName();
    virtual UINT GetClassStyle() const;

    virtual LRESULT HandleMessage(UINT msg, WPARAM w_param, LPARAM l_param);
    virtual void OnFinalMessage(HWND hwnd);

    static LRESULT CALLBACK WindowProc(HWND hwnd,  UINT msg, WPARAM w_param, LPARAM l_param);
    static LRESULT CALLBACK ControlProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);

protected:
    HWND hwnd_;
    WNDPROC old_window_proc_;
    HDC client_area_hdc_;
    GdiSurface * offscreen_surface_;
    bool offscreen_paint_ : 1;
    bool subclassed_ : 1;
    bool mouse_tracking_ : 1;
    bool created_ : 1;
    bool initializing_ : 1;
    bool expected_resize_ : 1;
    bool in_border_change_ : 1;
    bool windowed_mode_was_maximized_ : 1;
    bool in_window_deactivation_ : 1;
    bool layered_changed_ : 1;
    DuiAlpha opacity_ : 8;

    DuiRect layered_update_area_;

    std::vector<NativeWindow> native_windows_;

private:
    enum {
        kLayeredUpdateTimerID = 0x2000
    };

private:
    friend bool SetupWindowData(DuiRealWindowWin * window, HWND hwnd, bool created);
};

#endif//DUI_CORE_REAL_WINDOW_WIN_H_
