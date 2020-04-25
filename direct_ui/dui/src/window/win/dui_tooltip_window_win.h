#ifndef DUI_WINDOW_TOOLTIP_WINDOW_WIN_H_
#define DUI_WINDOW_TOOLTIP_WINDOW_WIN_H_

#include "core/win/dui_real_window_win.h"

#include "dui/window/dui_tooltip_window.h"

#include <CommCtrl.h>

class TooltipWindowWin : public DuiRealWindowWin {
public:
    TooltipWindowWin(DuiTooltipWindow * owner);

public:
    void OnInit() override;

    int OnSetTooltipWidth(int iWidth) override;
    void OnShowTooltip();
    void OnHideTooltip();
    void OnMoveTooltip(const DuiPoint& loc);
    int OnGetTooltipTime();

protected:
    void ResetToolInfo();

protected:
    bool HandleEvent(const DuiWindowEvent * evt, DuiWindowEventResult& result) override;

    bool OnGetWindowStyle(DWORD& style) const override;
    bool OnGetWindowExStyle(DWORD& ex_style) const override;

    const wchar_t * GetWindowClassName() override;
    const wchar_t * GetSuperClassName() override;
    LRESULT HandleMessage(UINT msg, WPARAM w_param, LPARAM l_param) override;
    void OnFinalMessage(HWND hwnd) override;

protected:
    DuiTooltipWindow * Owner() const;
    DuiTooltipWindow * Owner();

    DuiTooltipClient * GetTootipClient() const;

    DuiWindow* GetHostWindow() const;
    HWND GetHostWindowHandle() const;
    LPCTSTR GetTipText() const;
    RECT GetTipArea() const;

private:
    TOOLINFO tooltip_;
    int tooltip_width_;
    bool added_;
};

#endif//DUI_WINDOW_TOOLTIP_WINDOW_WIN_H_
