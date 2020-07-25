#ifndef QUI_CORE_WINDOW_H_
#define QUI_CORE_WINDOW_H_

#include "qui/core/qui_prerequisites.h"
#include "qui/core/qui_rect.h"

class QUI_EXPORT QuiWindow {
public:
    QuiWindow();
    virtual ~QuiWindow();

public:
    HWND GetHWND() const;
    operator HWND() const;

    HWND Create(HWND parent, const wchar_t* window_name, DWORD window_style, DWORD window_ex_style, const QuiRect& area);
    HWND Create(HWND parent, const wchar_t* window_name, DWORD window_style, DWORD window_ex_style, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int w = CW_USEDEFAULT, int h = CW_USEDEFAULT);
    HWND CreateQuiWindow(HWND parent, const wchar_t* window_name, DWORD window_style = 0, DWORD dwExStyle = 0);

    HWND Subclass(HWND win_handle);
    void Unsubclass();

    void ShowWindow(bool show = true, bool take_focus = true);
    void ShowModal();
    void Close();
    void CenterWindow();
    void SetIcon(UINT icon_id);
    void ResizeClient(int w = -1, int h = -1);

protected:
    virtual const wchar_t* GetWindowClassName() const = 0;
    virtual const wchar_t* GetSuperClassName() const { return nullptr; }
    virtual DWORD GetClassStyle() const;

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM w_param, LPARAM l_param);
    virtual void OnFinalMessage(HWND hWnd);

private:
    bool RegisterWindowClass();
    bool RegisterSuperclass();

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);
    static LRESULT CALLBACK ControlProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);

private:
    HWND hwnd_;
    WNDPROC old_wndproc_;
    bool subclassed_;
};


#endif   // QUI_CORE_WINDOW_H_
