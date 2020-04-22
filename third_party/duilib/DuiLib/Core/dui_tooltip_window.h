#ifndef DUI_CORE_TOOLTIP_WINDOW_H_
#define DUI_CORE_TOOLTIP_WINDOW_H_

#include "UIBase.h"

enum DuiTooltipWindowType {
    kSystem_Tooltip,
    kSubclassSystem_Tooltip,
    kCustom_Tooltip,
};

class DuiTooltipWindow {
public:
    static DuiTooltipWindow * MakeTooltipWindow(HWND host, DuiTooltipWindowType type = kSystem_Tooltip);

    virtual ~DuiTooltipWindow() { }

public:
    virtual void ResetTooltipInfo(LPCTSTR tip_text, const RECT& tip_rect) = 0;
    virtual int SetMaxTipWidth(int tip_width) = 0;
    virtual void ShowTip() = 0;
    virtual void HideTip() = 0;
    virtual void MoveTip(const POINT& loc) = 0;
    virtual DWORD GetDelayTime() = 0;
    virtual HWND GetTipHandle() = 0;
};

class DuiSystemTooltipWindow : public DuiTooltipWindow {
public:
    DuiSystemTooltipWindow(HWND host);
    ~DuiSystemTooltipWindow() override;

public:
    void ResetTooltipInfo(LPCTSTR tip_text, const RECT& tip_rect) override;
    int SetMaxTipWidth(int tip_width) override;
    void ShowTip() override;
    void HideTip() override;
    void MoveTip(const POINT& loc) override;
    DWORD GetDelayTime() override;
    HWND GetTipHandle() override;

private:
    TOOLINFO tooltip_info_;
    HWND host_;
    HWND tooltip_hwnd_;

    bool inited_;

    friend class DuiTooltipWindow;
};

class DuiSubclassSystemTooltipWindow : public DuiLib::CWindowWnd, public DuiTooltipWindow {
public:
    DuiSubclassSystemTooltipWindow(HWND host);
    ~DuiSubclassSystemTooltipWindow() override;

public:
    void ResetTooltipInfo(LPCTSTR tip_text, const RECT& tip_rect) override;
    int SetMaxTipWidth(int tip_width) override;
    void ShowTip() override;
    void HideTip() override;
    void MoveTip(const POINT& loc) override;
    DWORD GetDelayTime() override;
    HWND GetTipHandle() override;

protected:
    LPCTSTR GetWindowClassName() const override;
    LPCTSTR GetSuperClassName() const override;
    UINT GetClassStyle() const override;

    LRESULT HandleMessage(UINT msg, WPARAM w_param, LPARAM l_param) override;
    void OnFinalMessage(HWND hwnd) override;

private:
    TOOLINFO tooltip_info_;
    HWND host_;

    DuiLib::CPaintManagerUI paint_manager_;
    DuiLib::CLabelUI * tip_label_;
    DWORD tip_bkcolor_;
    DWORD tip_textcolor_;

    bool inited_ : 1;
    bool painting_ : 1;
};

class DuiCustomTooltipWindow : public DuiLib::CWindowWnd, public DuiTooltipWindow {
public:
    DuiCustomTooltipWindow(HWND host);
    ~DuiCustomTooltipWindow() override;

public:
    void ResetTooltipInfo(LPCTSTR tip_text, const RECT& tip_rect) override;
    int SetMaxTipWidth(int tip_width) override;
    void ShowTip() override;
    void HideTip() override;
    void MoveTip(const POINT& loc) override;
    DWORD GetDelayTime() override;
    HWND GetTipHandle() override;

protected:
    LPCTSTR GetWindowClassName() const override;
    UINT GetClassStyle() const override;

    LRESULT HandleMessage(UINT msg, WPARAM w_param, LPARAM l_param) override;
    void OnFinalMessage(HWND hwnd) override;

private:
    enum {
        kShowTooltip_TimerID = 101,
        kHideTooltip_TimerID = 102,
        kMoveTooltip_TimerID = 103,
        kAutoPopTooltip_TimerID = 104,
    };

private:
    HWND host_;
    DuiLib::CPaintManagerUI paint_manager_;
    DuiLib::CLabelUI * tip_label_;
    RECT tip_rect_;
    POINT location_;
    int tip_width_;
    bool inited_ : 1;
    bool showing_ : 1;
    bool hiding_ : 1;
    bool moving_ : 1;
    HWND tooltip_hwnd_;
    DWORD tip_bkcolor_;
    DWORD tip_textcolor_;
    int auto_pop_delay_time_;
    int initial_delay_time_;

    friend class DuiTooltipWindow;
};

#endif//DUI_CORE_TOOLTIP_WINDOW_H_
