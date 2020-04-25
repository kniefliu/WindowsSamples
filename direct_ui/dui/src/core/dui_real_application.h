#ifndef DUI_CORE_REAL_APPLICATION_H_
#define DUI_CORE_REAL_APPLICATION_H_

#include "core/dui_real_window.h"


class DuiRealApplication final {
public:
    static void Init();
    static void Run();
    static void Quit();

public:
    static DuiRealWindow* GetWindows();
    static void SetWindows(DuiRealWindow * window);
    static float ComputeDiagonalDPI(int hpix, int vpix, float hinches, float vinches);

public:
    static void ClearDisplays();
    static int GetNumOfDisplays();
    static int AddVideoDisplay(const VideoDisplay * display);

    static int GetWindowDisplayIndex(DuiRealWindow * window);
    static VideoDisplay * GetDisplayForWindow(DuiRealWindow * window);
    static int GetIndexOfDisplay(VideoDisplay * display);
    static bool GetDisplayBounds(int displayIndex, DuiRect * rect);
    static bool GetDisplayBounds(VideoDisplay *, DuiRect * rect);
    static DuiRealWindow* GetFullscreenWindow(VideoDisplay * display);
    static bool AddDisplayMode(VideoDisplay * display, const DisplayMode * mode);
    static bool GetWindowDisplayMode(DuiRealWindow * window, DisplayMode * mode);
    static bool SetDisplayModeForDisplay(VideoDisplay * display, const DisplayMode * mode);
};

#endif//DUI_CORE_REAL_APPLICATION_H_
