#ifndef DUI_CORE_WINDOW_H_
#define DUI_CORE_WINDOW_H_

#include "dui/core/dui_config.h"
#include "dui/core/dui_window_type.h"
#include "dui/core/dui_window_event.h"
#include "dui/core/dui_rect.h"
#include "dui/core/dui_control_manager_client.h"
#include "dui/core/dui_canvas.h"

class DuiRealWindow;
class DuiTooltipWindow;
class DUI_API DuiWindow : public DuiControlManagerClient {
public:
    explicit DuiWindow(bool destroy_by_self = false);
    virtual ~DuiWindow();

public:
    DuiWindowHandle Create(const wchar_t *title, int x, int y, int w, int h, 
        DuiWindowFlags flags, DuiWindow * parent = nullptr);
    DuiWindowHandle CreateFrom(DuiWindowHandle extern_handle);
    void Add(DuiWindow * child);
    DuiWindowHandle GetHandle() const;

    void SetHoverTime(int);

public:
    // from DuiControlManagerClient
    DuiPoint GetLocation() const final;
    DuiPoint GetLocationOnScreen() const final;
    DuiSize GetSize() const final;
    DuiSize GetMaximumSize() const final;
    DuiSize GetMinimumSize() const final;
    void SetLocation(const DuiPoint& pt) final;
    void SetSize(const DuiSize& sz) final;
    void SetMinimumSize(const DuiSize& sz) final;
    void SetMaximumSize(const DuiSize& sz) final;

    DuiPoint GetMousePosition() final;

    void Invalidate(const DuiRect* repaint_area = nullptr);

    int GetNativeWindowCount() const final;
    DuiRect GetNativeWindowRect(DuiWindowHandle child_window) const final;
    bool AddNativeWindow(DuiControlBase * controller, DuiWindowHandle child_window) final;
    bool RemoveNativeWindow(DuiWindowHandle child_window) final;

    DuiAlpha GetOpacity() const final;
    void SetOpacity(DuiAlpha nOpacity) final;
    bool IsLayered() final;
    void SetLayered(bool bLayered) final;
    DuiAlpha GetLayeredOpacity() final;
    void SetLayeredOpacity(DuiAlpha nOpacity) final;

protected:
    virtual bool OnHandleEvent(const DuiWindowEvent * event, DuiWindowEventResult& result);
    virtual void OnDestoryed() {}

private:
    bool HandleEvent(const DuiWindowEvent * event, DuiWindowEventResult& result);

private:
    virtual DuiRealWindow * OnCreate() { return nullptr; }

protected:
    DuiRealWindow* real_window_;

    // expected 
    int x_;
    int y_;
    int width_;
    int height_;
    int min_width_;
    int min_height_;
    int max_width_;
    int max_height_;

    bool destroy_by_self_ : 1;

    friend class DuiRealWindow;
    friend class DuiWindowPriv;
};

#endif  //DUI_CORE_WINDOW_H_
