#ifndef DUI_WINDOW_TOOLTIP_WINDOW_H_
#define DUI_WINDOW_TOOLTIP_WINDOW_H_

#include "dui/core/dui_window.h"

class DUI_API DuiTooltipClient {
public:
    virtual DuiWindow* GetHostWindow() { return nullptr; }
};

class DUI_API DuiTooltipWindow : public DuiWindow {
public:
    explicit DuiTooltipWindow(bool destroy_by_self = true);

    void Init(DuiTooltipClient * client);
    DuiTooltipClient * GetClient();

    void SetTooltipWindowWidth(int width);
    void ShowTooltip();
    void HideTooltip();
    void MoveTooltip(const DuiPoint& loc);
    int GetTooltipTime();

protected:
    DuiWindowHandle Create(const wchar_t *title, int x, int y, int w, int h,
        DuiWindowFlags flags, DuiWindow * parent = nullptr);
    DuiWindowHandle CreateFrom(DuiWindowHandle extern_handle) { return nullptr; }

protected:
    void OnDestoryed() override;

private:
    DuiRealWindow * OnCreate() override;

private:
    DuiTooltipClient * client_;
    int tooltip_width_;
};

#endif//DUI_WINDOW_TOOLTIP_WINDOW_H_
