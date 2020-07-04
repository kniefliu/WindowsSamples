#pragma once

#include <Windows.h>

class QuiWindow;
class QuiWindowPrivate {
public:
    QuiWindowPrivate(QuiWindow * owner);

public:
    bool RegisterWindowClass();
    bool RegisterSuperclass();

    HWND Create(HWND parent, const wchar_t* window_name, DWORD window_style,
        DWORD window_ex_style,
        int x = CW_USEDEFAULT, int y = CW_USEDEFAULT,
        int w = CW_USEDEFAULT, int h = CW_USEDEFAULT);

    HWND Subclass(HWND hWnd);
    void Unsubclass();

    LRESULT HandleMessage(UINT msg, WPARAM w_param, LPARAM l_param);

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);
    static LRESULT CALLBACK ControlProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);

public:
    QuiWindow * owner_;
    HWND hwnd_;
    WNDPROC old_wndproc_;
    bool subclassed_ : 1;
};