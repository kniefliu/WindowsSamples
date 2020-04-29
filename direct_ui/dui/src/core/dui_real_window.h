#ifndef DUI_CORE_REAL_WINDOW_H_
#define DUI_CORE_REAL_WINDOW_H_

#include "dui/core/dui_window.h"

#include "dui/core/dui_rect.h"

#if defined(DUI_BUILD_FOR_WIN32)
#include <Windows.h>
#endif

#include <string>

#define Fullscreen_Visible(W) \
    (((W)->flags_ & kFullscreen_Window) && \
     ((W)->flags_ & kShown_Window) && \
     !((W)->flags_ & kMinimized_Window))


struct DisplayMode {
    uint32_t format;            /**< pixel format */
    int w;                      /**< width, in screen coordinates */
    int h;                      /**< height, in screen coordinates */
    int refresh_rate;           /**< refresh rate (or zero for unspecified) */
    
    // driver data
#if defined(DUI_BUILD_FOR_WIN32)
    DEVMODE DeviceMode;
    float ScaleX;
    float ScaleY;
    float DiagDPI;
    float HorzDPI;
    float VertDPI;
#else
#endif

    DisplayMode() {
        memset(this, 0, sizeof(*this));
    }
};

const int kMaxLength_VideoDisplayName = 128;
const int kMaxCount_DisplayModes = 30;
const int kMaxCount_Displays = 10;
struct VideoDisplay {
    wchar_t name[kMaxLength_VideoDisplayName];
    int max_display_modes;
    int num_display_modes;
    DisplayMode *display_modes;// [kMaxCount_DisplayModes];
    DisplayMode desktop_mode;
    DisplayMode current_mode;

    DuiRealWindow *fullscreen_window;

    // driver data
#if defined(DUI_BUILD_FOR_WIN32)
    TCHAR DeviceName[32];
#else
#endif
};

class DuiEditWindow;
class DuiEditClient;
class DuiTooltipWindow;
class DuiRealWindow {
public:
    static DuiRealWindow* MakeRealWindow(DuiWindow * owner);
    static DuiRealWindow* MakeRealEditWindow(DuiEditWindow * owner);
    static DuiRealWindow* MakeRealTooltipWindow(DuiTooltipWindow * owner);

public:
    DuiRealWindow(DuiWindow * owner);
    virtual ~DuiRealWindow();

    DuiWindowHandle GetHandle();

    void SetHoverTime(int time) { hover_time_ = time; }

public:
    virtual DuiPoint GetLocation() const;
    virtual DuiPoint GetLocationOnScreen() const = 0;
    virtual DuiSize GetSize() const;
    virtual DuiSize GetMaximumSize() const;
    virtual DuiSize GetMinimumSize() const;
    void SetLocation(const DuiPoint& pt);
    virtual void SetSize(const DuiSize& sz) = 0;
    virtual void SetMinimumSize(const DuiSize& sz) = 0;
    virtual void SetMaximumSize(const DuiSize& sz) = 0;

    virtual DuiPoint GetMousePosition() = 0;

    virtual void Invalidate(const DuiRect* repaint_area = nullptr) = 0;

    virtual int GetNativeWindowCount() const { return 0; }
    virtual DuiRect GetNativeWindowRect(DuiWindowHandle child_window) const { return DuiRect::MakeEmpty(); }
    virtual bool AddNativeWindow(DuiControlBase * controller, DuiWindowHandle child_window) { return false; }
    virtual bool RemoveNativeWindow(DuiWindowHandle child_window) { return false; }

    virtual DuiAlpha GetOpacity() const { return 255; }
    virtual void SetOpacity(DuiAlpha nOpacity) { }
    virtual bool IsLayered() { return false; }
    virtual void SetLayered(bool bLayered) { }
    virtual DuiAlpha GetLayeredOpacity() { return 255; }
    virtual void SetLayeredOpacity(DuiAlpha nOpacity) { }

public:
    DuiWindowHandle Create(const wchar_t *title, int x, int y, int w, int h, DuiWindowFlags flags, DuiWindow * parent = nullptr);
    DuiWindowHandle CreateFrom(DuiWindowHandle extern_handle);
    void SetTitle(const wchar_t *title);
    void Add(DuiWindow * child);

public:
    void FinishCreation(DuiWindowFlags flags);
    void Show();
    void Hide();
    void Raise();
    void Maximize();
    void Minimize();
    void Restore();
    bool SetFullscreen(DuiWindowFlags flags);

    virtual void SetFullscreen(VideoDisplay * display, bool fullscreen) = 0;

public:
    bool UpdateFullscreenMode(bool fullscreen);

public:
    virtual void OnInit() { }

protected:
    virtual bool HandleEvent(const DuiWindowEvent * evt, DuiWindowEventResult& result);
    virtual DuiWindowHandle OnCreate(DuiRealWindow * parent) = 0;
    virtual DuiWindowHandle OnCreateFrom(DuiWindowHandle extern_handle) = 0;
    virtual DuiWindowHandle OnGetHandle() = 0;
    virtual void OnSetTitle() = 0;
    virtual void OnAdd(DuiRealWindow * child) = 0;
    virtual void OnShow() = 0;
    virtual void OnHide() = 0;
    virtual void OnRaise() = 0;
    virtual void OnMaximize() = 0;
    virtual void OnMinimize() = 0;
    virtual void OnRestore() = 0;
    virtual void OnSetLocation() = 0;

private:
    void OnShown();
    void OnHidden();
    void OnResized();
    void OnMinimized();
    void OnRestored();

    // Tootip related
public:
    int SetTooltipWidth(int width);
    void ShowTooltip();
    void HideTooltip();
    void MoveTooltip(const DuiPoint& loc);
    int GetTooltipTime();
protected:
    virtual int OnSetTooltipWidth(int width) { return width; }
    virtual void OnShowTooltip() {}
    virtual void OnHideTooltip() {}
    virtual void OnMoveTooltip(const DuiPoint& loc) {}
    virtual int OnGetTooltipTime() { return hover_time_; }

protected:
    DuiWindow * owner_;

    int x_;
    int y_;
    int width_;
    int height_;
    int min_width_;
    int min_height_;
    int max_width_;
    int max_height_;
    DuiWindowFlags flags_;
    DuiWindowFlags last_fullscreen_flags_;

    /* Stored position and size for windowed mode */
    DuiRect windowed_;

    DisplayMode fullscreen_mode_;

    bool hiding_ : 1;
    bool destroying_ : 1;

    std::wstring title_;

    int hover_time_;

protected:
    DuiRealWindow * prev_;
    DuiRealWindow * next_;

    friend class DuiRealApplication;
};

#endif  //DUI_CORE_REAL_WINDOW_H_
