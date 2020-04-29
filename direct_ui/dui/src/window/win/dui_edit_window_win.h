#ifndef DUI_WINDOW_EDIT_WINDOW_WIN_H_
#define DUI_WINDOW_EDIT_WINDOW_WIN_H_

#include "dui/window/dui_edit_window.h"

#include "core/win/dui_real_window_win.h"

class EditWindowWin : public DuiRealWindowWin {
public:
    EditWindowWin(DuiEditWindow * owner);

public:
    void OnInit() override;

protected:

    bool OnGetWindowStyle(DWORD& style) const override;
    bool OnGetWindowExStyle(DWORD& ex_style) const override;

    const wchar_t * GetWindowClassName() override;
    const wchar_t * GetSuperClassName() override;
    LRESULT HandleMessage(UINT msg, WPARAM w_param, LPARAM l_param) override;
    void OnFinalMessage(HWND hwnd) override;

    LRESULT OnKillFocus(UINT msg, WPARAM w_param, LPARAM l_param, bool& handled);
    LRESULT OnEditChanged(UINT msg, WPARAM w_param, LPARAM l_param, bool& handled);

protected:
    DuiEditWindow * Owner() const;
    DuiEditWindow * Owner();
    DuiEditClient * GetEditClient() const;

    DuiEditStyle GetEditStyle() const;
    DuiWindow* GetHostWindow();
    HWND GetOwnerNativeWindow();
    void* GetNativeFont();
    uint32_t GetMaxChar() const;
    wchar_t GetPasswordChar();
    bool IsEnabled() const;
    bool IsReadOnly() const;
    bool IsAutoSelAll() const;
    bool IsPasswordMode() const;
private:
    enum {
        kCaret_TimerID = 20,
    };
private:
    HBRUSH bk_brush_;
    bool inited_ : 1;
    bool draw_caret_ : 1;
};

#endif//DUI_WINDOW_EDIT_WINDOW_WIN_H_
