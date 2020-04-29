#include "core/dui_real_application.h"

#include "core/win/dui_real_window_win.h"

#include <Windows.h>
#include <CommCtrl.h>
#include <shellscalingapi.h>

#include <vector>
#include <sstream>


const int kTextSize_TexteditingEvent = 32;
const int kMaxCandlist = 10;
const int kMaxCandlength = 256;

#if WINVER < 0x0601
/* Touch input definitions */
#define TWF_FINETOUCH   1
#define TWF_WANTPALM    2

#define TOUCHEVENTF_MOVE 0x0001
#define TOUCHEVENTF_DOWN 0x0002
#define TOUCHEVENTF_UP   0x0004

DECLARE_HANDLE(HTOUCHINPUT);

typedef struct _TOUCHINPUT {
    LONG      x;
    LONG      y;
    HANDLE    hSource;
    DWORD     dwID;
    DWORD     dwFlags;
    DWORD     dwMask;
    DWORD     dwTime;
    ULONG_PTR dwExtraInfo;
    DWORD     cxContact;
    DWORD     cyContact;
} TOUCHINPUT, *PTOUCHINPUT;

#endif /* WINVER < 0x0601 */

#if WINVER < 0x0603

typedef enum MONITOR_DPI_TYPE {
    MDT_EFFECTIVE_DPI = 0,
    MDT_ANGULAR_DPI = 1,
    MDT_RAW_DPI = 2,
    MDT_DEFAULT = MDT_EFFECTIVE_DPI
} MONITOR_DPI_TYPE;

#endif /* WINVER < 0x0603 */

//typedef BOOL(*PFNSHFullScreen)(HWND, DWORD);
//typedef void(*PFCoordTransform)(SDL_Window*, POINT*);

typedef struct
{
    void **lpVtbl;
    int refcount;
    void *data;
} TSFSink;

/* Definition from Win98DDK version of IMM.H */
typedef struct tagINPUTCONTEXT2 {
    HWND hWnd;
    BOOL fOpen;
    POINT ptStatusWndPos;
    POINT ptSoftKbdPos;
    DWORD fdwConversion;
    DWORD fdwSentence;
    union {
        LOGFONTA A;
        LOGFONTW W;
    } lfFont;
    COMPOSITIONFORM cfCompForm;
    CANDIDATEFORM cfCandForm[4];
    HIMCC hCompStr;
    HIMCC hCandInfo;
    HIMCC hGuideLine;
    HIMCC hPrivate;
    DWORD dwNumMsgBuf;
    HIMCC hMsgBuf;
    DWORD fdwInit;
    DWORD dwReserve[3];
} INPUTCONTEXT2, *PINPUTCONTEXT2, NEAR *NPINPUTCONTEXT2, FAR *LPINPUTCONTEXT2;

/* Private display data */
struct VideoData {
    int render;

    DWORD clipboard_count;

    /* Touch input functions */
    void* userDLL;
    BOOL(WINAPI *CloseTouchInputHandle)(HTOUCHINPUT);
    BOOL(WINAPI *GetTouchInputInfo)(HTOUCHINPUT, UINT, PTOUCHINPUT, int);
    BOOL(WINAPI *RegisterTouchWindow)(HWND, ULONG);

    void* shcoreDLL;
    HRESULT(WINAPI *GetDpiForMonitor)(HMONITOR         hmonitor,
        MONITOR_DPI_TYPE dpiType,
        UINT             *dpiX,
        UINT             *dpiY);

    bool ime_com_initialized;
    struct ITfThreadMgr *ime_threadmgr;
    bool ime_initialized;
    bool ime_enabled;
    bool ime_available;
    HWND ime_hwnd_main;
    HWND ime_hwnd_current;
    HIMC ime_himc;

    WCHAR ime_composition[kTextSize_TexteditingEvent];
    WCHAR ime_readingstring[16];
    int ime_cursor;

    bool ime_candlist;
    WCHAR ime_candidates[kMaxCandlist][kMaxCandlength];
    DWORD ime_candcount;
    DWORD ime_candref;
    DWORD ime_candsel;
    UINT ime_candpgsize;
    int ime_candlistindexbase;
    bool ime_candvertical;

    bool ime_dirty;
    DuiRect ime_rect;
    DuiRect ime_candlistrect;
    int ime_winwidth;
    int ime_winheight;

    HKL ime_hkl;
    void* ime_himm32;
    UINT(WINAPI *GetReadingString)(HIMC himc, UINT uReadingBufLen, LPWSTR lpwReadingBuf, PINT pnErrorIndex, BOOL *pfIsVertical, PUINT puMaxReadingLen);
    BOOL(WINAPI *ShowReadingWindow)(HIMC himc, BOOL bShow);
    LPINPUTCONTEXT2(WINAPI *ImmLockIMC)(HIMC himc);
    BOOL(WINAPI *ImmUnlockIMC)(HIMC himc);
    LPVOID(WINAPI *ImmLockIMCC)(HIMCC himcc);
    BOOL(WINAPI *ImmUnlockIMCC)(HIMCC himcc);

    bool ime_uiless;
    struct ITfThreadMgrEx *ime_threadmgrex;
    DWORD ime_uielemsinkcookie;
    DWORD ime_alpnsinkcookie;
    DWORD ime_openmodesinkcookie;
    DWORD ime_convmodesinkcookie;
    TSFSink *ime_uielemsink;
    TSFSink *ime_ippasink;
};

VideoData video_data = { 0 };


struct WIN_GetMonitorDPIData {
    VideoData *vid_data;
    DisplayMode *mode;
};

static BOOL CALLBACK
WIN_GetMonitorDPI(HMONITOR hMonitor,
    HDC      hdcMonitor,
    LPRECT   lprcMonitor,
    LPARAM   dwData)
{
    WIN_GetMonitorDPIData *data = (WIN_GetMonitorDPIData*)dwData;
    UINT hdpi, vdpi;

    if (data->vid_data->GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &hdpi, &vdpi) == S_OK &&
        hdpi > 0 &&
        vdpi > 0) {
        float hsize, vsize;

        data->mode->HorzDPI = (float)hdpi;
        data->mode->VertDPI = (float)vdpi;

        // Figure out the monitor size and compute the diagonal DPI.
        hsize = data->mode->w / data->mode->HorzDPI;
        vsize = data->mode->h / data->mode->VertDPI;

        data->mode->DiagDPI = DuiRealApplication::ComputeDiagonalDPI(data->mode->w,
            data->mode->h,
            hsize,
            vsize);

        // We can only handle one DPI per display mode so end the enumeration.
        return FALSE;
    }

    // We didn't get DPI information so keep going.
    return TRUE;
}

static void WIN_UpdateDisplayMode(LPCTSTR deviceName, DWORD index, DisplayMode * mode)
{
    VideoData *vid_data = (VideoData *)&video_data;
    HDC hdc;

    mode->DeviceMode.dmFields =
        (DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY |
            DM_DISPLAYFLAGS);

    if (index == ENUM_CURRENT_SETTINGS
        && (hdc = CreateDC(deviceName, NULL, NULL, NULL)) != NULL) {
        char bmi_data[sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)];
        LPBITMAPINFO bmi;
        HBITMAP hbm;
        int logical_width = GetDeviceCaps(hdc, HORZRES);
        int logical_height = GetDeviceCaps(hdc, VERTRES);

        mode->ScaleX = (float)logical_width / mode->DeviceMode.dmPelsWidth;
        mode->ScaleY = (float)logical_height / mode->DeviceMode.dmPelsHeight;
        mode->w = logical_width;
        mode->h = logical_height;

        // WIN_GetMonitorDPI needs mode->w and mode->h
        // so only call after those are set.
        if (vid_data->GetDpiForMonitor) {
            WIN_GetMonitorDPIData dpi_data;
            RECT monitor_rect;

            dpi_data.vid_data = vid_data;
            dpi_data.mode = mode;
            monitor_rect.left = mode->DeviceMode.dmPosition.x;
            monitor_rect.top = mode->DeviceMode.dmPosition.y;
            monitor_rect.right = monitor_rect.left + 1;
            monitor_rect.bottom = monitor_rect.top + 1;
            EnumDisplayMonitors(NULL, &monitor_rect, WIN_GetMonitorDPI, (LPARAM)&dpi_data);
        }
        else {
            // We don't have the Windows 8.1 routine so just
            // get system DPI.
            mode->HorzDPI = (float)GetDeviceCaps(hdc, LOGPIXELSX);
            mode->VertDPI = (float)GetDeviceCaps(hdc, LOGPIXELSY);
            if (mode->HorzDPI == mode->VertDPI) {
                mode->DiagDPI = mode->HorzDPI;
            }
            else {
                mode->DiagDPI = DuiRealApplication::ComputeDiagonalDPI(mode->w,
                    mode->h,
                    (float)GetDeviceCaps(hdc, HORZSIZE) / 25.4f,
                    (float)GetDeviceCaps(hdc, VERTSIZE) / 25.4f);
            }
        }

        ::ZeroMemory(bmi_data, sizeof(bmi_data));
        bmi = (LPBITMAPINFO)bmi_data;
        bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

        hbm = CreateCompatibleBitmap(hdc, 1, 1);
        GetDIBits(hdc, hbm, 0, 1, NULL, bmi, DIB_RGB_COLORS);
        GetDIBits(hdc, hbm, 0, 1, NULL, bmi, DIB_RGB_COLORS);
        DeleteObject(hbm);
        DeleteDC(hdc);
        mode->format = 0;
        //if (bmi->bmiHeader.biCompression == BI_BITFIELDS) {
        //    switch (*(uint32_t *)bmi->bmiColors) {
        //    case 0x00FF0000:
        //        mode->format = SDL_PIXELFORMAT_RGB888;
        //        break;
        //    case 0x000000FF:
        //        mode->format = SDL_PIXELFORMAT_BGR888;
        //        break;
        //    case 0xF800:
        //        mode->format = SDL_PIXELFORMAT_RGB565;
        //        break;
        //    case 0x7C00:
        //        mode->format = SDL_PIXELFORMAT_RGB555;
        //        break;
        //    }
        //}
        //else if (bmi->bmiHeader.biBitCount == 8) {
        //    mode->format = SDL_PIXELFORMAT_INDEX8;
        //}
        //else if (bmi->bmiHeader.biBitCount == 4) {
        //    mode->format = SDL_PIXELFORMAT_INDEX4LSB;
        //}
    }
    //else if (mode->format == SDL_PIXELFORMAT_UNKNOWN) {
    //    /* FIXME: Can we tell what this will be? */
    //    if ((data->DeviceMode.dmFields & DM_BITSPERPEL) == DM_BITSPERPEL) {
    //        switch (data->DeviceMode.dmBitsPerPel) {
    //        case 32:
    //            mode->format = SDL_PIXELFORMAT_RGB888;
    //            break;
    //        case 24:
    //            mode->format = SDL_PIXELFORMAT_RGB24;
    //            break;
    //        case 16:
    //            mode->format = SDL_PIXELFORMAT_RGB565;
    //            break;
    //        case 15:
    //            mode->format = SDL_PIXELFORMAT_RGB555;
    //            break;
    //        case 8:
    //            mode->format = SDL_PIXELFORMAT_INDEX8;
    //            break;
    //        case 4:
    //            mode->format = SDL_PIXELFORMAT_INDEX4LSB;
    //            break;
    //        }
    //    }
    //}
}
static bool WIN_GetDisplayMode(LPCTSTR deviceName, DWORD index, DisplayMode * mode)
{
    DEVMODE devmode;

    devmode.dmSize = sizeof(devmode);
    devmode.dmDriverExtra = 0;
    if (!EnumDisplaySettings(deviceName, index, &devmode)) {
        return false;
    }

    mode->DeviceMode = devmode;

    /* Default basic information */
    mode->ScaleX = 1.0f;
    mode->ScaleY = 1.0f;
    mode->DiagDPI = 0.0f;
    mode->HorzDPI = 0.0f;
    mode->VertDPI = 0.0f;

    mode->format = 0;
    mode->w = mode->DeviceMode.dmPelsWidth;
    mode->h = mode->DeviceMode.dmPelsHeight;
    mode->refresh_rate = mode->DeviceMode.dmDisplayFrequency;

    /* Fill in the mode information */
    WIN_UpdateDisplayMode(deviceName, index, mode);
    return true;
}
static bool WIN_AddDisplay(LPTSTR DeviceName)
{
    VideoDisplay display;
    DisplayMode mode;
    DISPLAY_DEVICE device;

    if (!WIN_GetDisplayMode(DeviceName, ENUM_CURRENT_SETTINGS, &mode)) {
        return false;
    }

    ::ZeroMemory(&display, sizeof(VideoDisplay));

    memcpy(display.DeviceName, DeviceName,
        sizeof(display.DeviceName));

    device.cb = sizeof(device);
    if (EnumDisplayDevices(DeviceName, 0, &device, 0)) {
        memcpy(display.name, device.DeviceString, sizeof(device.DeviceString));
    }
    display.desktop_mode = mode;
    display.current_mode = mode;
    DuiRealApplication::AddVideoDisplay(&display);
    return true;
}

bool WIN_SetDisplayMode(VideoDisplay * display, DisplayMode * mode)
{
    LONG status;

    if (true/*memcmp(mode, &display->desktop_mode, sizeof(DisplayMode)) == 0*/) {
        status = ChangeDisplaySettingsEx(display->DeviceName, NULL, NULL, CDS_FULLSCREEN, NULL);
    }
    else {
        status = ChangeDisplaySettingsEx(display->DeviceName, &mode->DeviceMode, NULL, CDS_FULLSCREEN, NULL);
    }
    if (status != DISP_CHANGE_SUCCESSFUL) {
        const char *reason = "Unknown reason";
        switch (status) {
        case DISP_CHANGE_BADFLAGS:
            reason = "DISP_CHANGE_BADFLAGS";
            break;
        case DISP_CHANGE_BADMODE:
            reason = "DISP_CHANGE_BADMODE";
            break;
        case DISP_CHANGE_BADPARAM:
            reason = "DISP_CHANGE_BADPARAM";
            break;
        case DISP_CHANGE_FAILED:
            reason = "DISP_CHANGE_FAILED";
            break;
        }
        return false;
    }
    EnumDisplaySettings(display->DeviceName, ENUM_CURRENT_SETTINGS, &mode->DeviceMode);
    WIN_UpdateDisplayMode(display->DeviceName, ENUM_CURRENT_SETTINGS, mode);
    return true;
}
bool WIN_InitModes()
{
    int pass;
    DWORD i, j, count;
    DISPLAY_DEVICE device;

    device.cb = sizeof(device);

    /* Get the primary display in the first pass */
    for (pass = 0; pass < 2; ++pass) {
        for (i = 0; ; ++i) {
            TCHAR DeviceName[32];

            if (!EnumDisplayDevices(NULL, i, &device, 0)) {
                break;
            }
            if (!(device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)) {
                continue;
            }
            if (pass == 0) {
                if (!(device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)) {
                    continue;
                }
            }
            else {
                if (device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) {
                    continue;
                }
            }
            memcpy(DeviceName, device.DeviceName, sizeof(DeviceName));

            count = 0;
            for (j = 0; ; ++j) {
                if (!EnumDisplayDevices(DeviceName, j, &device, 0)) {
                    break;
                }
                if (!(device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)) {
                    continue;
                }
                if (pass == 0) {
                    if (!(device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)) {
                        continue;
                    }
                }
                else {
                    if (device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) {
                        continue;
                    }
                }
                count += WIN_AddDisplay(device.DeviceName);
            }
            if (count == 0) {
                WIN_AddDisplay(DeviceName);
            }
        }
    }
    if (DuiRealApplication::GetNumOfDisplays() == 0) {
        return false;
    }
    return true;
}

void DuiRealApplication::Init()
{
    HRESULT result = ::CoInitialize(NULL);
    if (FAILED(result)) {
        return;
    }
    ::InitCommonControls();
    DWORD err = GetLastError();
    WIN_InitModes();
}
static int MessageLoop()
{
    MSG msg = { 0 };
    while (::GetMessage(&msg, NULL, 0, 0)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
    return (int)(msg.wParam);
}
void DuiRealApplication::Run()
{
    MessageLoop();
}
void DuiRealApplication::Quit()
{
    ::PostQuitMessage(0);
    ClearDisplays();
    ::CoUninitialize();
}

static std::vector<VideoDisplay*> displays;
void DuiRealApplication::ClearDisplays()
{
    for (auto iter : displays) {
        if (iter) {
            free(iter->display_modes);
            delete iter;
        }
    }
    displays.clear();
}
int DuiRealApplication::GetNumOfDisplays()
{
    return (int)(displays.size());
}
int DuiRealApplication::AddVideoDisplay(const VideoDisplay * display)
{
    int index = -1;

    index = displays.size();
    VideoDisplay * new_display = new VideoDisplay(*display);
    displays.push_back(new_display);
    if (!display->name) {
        std::wstringstream wss;
        wss << index;
        wcscpy(displays[index]->name, wss.str().c_str());
    }
    
    return index;
}
int DuiRealApplication::GetWindowDisplayIndex(DuiRealWindow * window)
{
    int displayIndex;
    int i, dist;
    int closest = -1;
    int closest_dist = 0x7FFFFFFF;
    DuiPoint center;
    DuiPoint delta;
    DuiRect rect;

    if (DuiWindowPos_IsUndefined(window->x_) ||
        DuiWindowPos_IsCentered(window->x_)) {
        displayIndex = (window->x_ & 0xFFFF);
        if (displayIndex >= GetNumOfDisplays()) {
            displayIndex = 0;
        }
        return displayIndex;
    }
    if (DuiWindowPos_IsUndefined(window->y_) ||
        DuiWindowPos_IsCentered(window->y_)) {
        displayIndex = (window->y_ & 0xFFFF);
        if (displayIndex >= GetNumOfDisplays()) {
            displayIndex = 0;
        }
        return displayIndex;
    }

    /* Find the display containing the window */
    for (i = 0; i < GetNumOfDisplays(); ++i) {
        VideoDisplay *display = displays[i];

        if (display->fullscreen_window == window) {
            return i;
        }
    }
    center.x = window->x_ + window->width_ / 2;
    center.y = window->y_ + window->height_ / 2;
    for (i = 0; i < GetNumOfDisplays(); ++i) {
        GetDisplayBounds(i, &rect);
        if (rect.Contains(center)) {
            return i;
        }

        delta.x = center.x - (rect.x() + rect.width() / 2);
        delta.y = center.y - (rect.y() + rect.height() / 2);
        dist = (delta.x*delta.x + delta.y*delta.y);
        if (dist < closest_dist) {
            closest = i;
            closest_dist = dist;
        }
    }
    if (closest < 0) {
        return -1;
    }
    return closest;
}
VideoDisplay * DuiRealApplication::GetDisplayForWindow(DuiRealWindow * window)
{
    int displayIndex = GetWindowDisplayIndex(window);
    if (displayIndex >= 0) {
        return displays[displayIndex];
    }
    else {
        return nullptr;
    }
    return nullptr;
}
int DuiRealApplication::GetIndexOfDisplay(VideoDisplay * display)
{
    int index = 0;
    for (auto iter : displays) {
        if (iter == display) {
            return index;
        }
        index++;
    }
    return -1;
}
bool DuiRealApplication::GetDisplayBounds(int displayIndex, DuiRect * rect)
{
    if (rect) {
        int x = 0;
        int y = 0;
        int w = 0;
        int h = 0;
        VideoDisplay *display = displays[displayIndex];

        x = (int)std::ceil(display->current_mode.DeviceMode.dmPosition.x * display->current_mode.ScaleX);
        y = (int)std::ceil(display->current_mode.DeviceMode.dmPosition.y * display->current_mode.ScaleY);
        w = (int)std::ceil(display->current_mode.DeviceMode.dmPelsWidth *  display->current_mode.ScaleX);
        h = (int)std::ceil(display->current_mode.DeviceMode.dmPelsHeight * display->current_mode.ScaleY);

        /* Assume that the displays are left to right */
        if (displayIndex == 0) {
            x = 0;
            y = 0;
        }
        else {
            GetDisplayBounds(displayIndex - 1, rect);
            x += w;
        }
        w = display->current_mode.w;
        h = display->current_mode.h;
        
        rect->SetXYWH(x, y, w, h);
    }
    return true;  /* !!! FIXME: should this be an error if (rect==NULL) ? */
}
bool DuiRealApplication::GetDisplayBounds(VideoDisplay *display, DuiRect * rect)
{
    return GetDisplayBounds(GetIndexOfDisplay(display), rect);
}
DuiRealWindow* DuiRealApplication::GetFullscreenWindow(VideoDisplay * display)
{
    return display->fullscreen_window;
}

static int cmpmodes(const void *A, const void *B)
{
    const DisplayMode *a = (const DisplayMode *)A;
    const DisplayMode *b = (const DisplayMode *)B;
    if (a == b) {
        return 0;
    }
    else if (a->w != b->w) {
        return b->w - a->w;
    }
    else if (a->h != b->h) {
        return b->h - a->h;
    }
    //else if (SDL_BITSPERPIXEL(a->format) != SDL_BITSPERPIXEL(b->format)) {
    //    return SDL_BITSPERPIXEL(b->format) - SDL_BITSPERPIXEL(a->format);
    //}
    //else if (SDL_PIXELLAYOUT(a->format) != SDL_PIXELLAYOUT(b->format)) {
    //    return SDL_PIXELLAYOUT(b->format) - SDL_PIXELLAYOUT(a->format);
    //}
    else if (a->refresh_rate != b->refresh_rate) {
        return b->refresh_rate - a->refresh_rate;
    }
    return 0;
}
bool DuiRealApplication::AddDisplayMode(VideoDisplay * display, const DisplayMode * mode)
{
    DisplayMode *modes;
    int i, nmodes;

    /* Make sure we don't already have the mode in the list */
    modes = display->display_modes;
    nmodes = display->num_display_modes;
    for (i = 0; i < nmodes; ++i) {
        if (cmpmodes(mode, &modes[i]) == 0) {
            return false;
        }
    }

    /* Go ahead and add the new mode */
    if (nmodes == display->max_display_modes) {
        modes = (DisplayMode *)realloc(modes, (display->max_display_modes + 32) * sizeof(*modes));
        if (!modes) {
            return false;
        }
        display->display_modes = modes;
        display->max_display_modes += 32;
    }
    modes[nmodes] = *mode;
    display->num_display_modes++;

    /* Re-sort video modes */
    std::qsort(display->display_modes, display->num_display_modes,
        sizeof(DisplayMode), cmpmodes);

    return true;
}
void WIN_GetDisplayModes(VideoDisplay * display)
{
    DWORD i;
    DisplayMode mode;

    for (i = 0;; ++i) {
        if (!WIN_GetDisplayMode(display->DeviceName, i, &mode)) {
            break;
        }
        //if (SDL_ISPIXELFORMAT_INDEXED(mode.format)) {
        //    /* We don't support palettized modes now */
        //    continue;
        //}
        if (true/*mode.format != SDL_PIXELFORMAT_UNKNOWN*/) {
            if (!DuiRealApplication::AddDisplayMode(display, &mode)) {

            }
        }
    }
}
int GetNumDisplayModesForDisplay(VideoDisplay * display)
{
    if (!display->num_display_modes) {
        WIN_GetDisplayModes(display);
        std::qsort(display->display_modes, display->num_display_modes,
            sizeof(DisplayMode), cmpmodes);
    }
    return display->num_display_modes;
}
DisplayMode * GetClosestDisplayModeForDisplay(VideoDisplay * display, const DisplayMode * mode, DisplayMode * closest)
{
    uint32_t target_format;
    int target_refresh_rate;
    int i;
    DisplayMode *current, *match;

    if (!mode || !closest) {
        return nullptr;
    }

    /* Default to the desktop format */
    if (mode->format) {
        target_format = mode->format;
    }
    else {
        target_format = display->desktop_mode.format;
    }

    /* Default to the desktop refresh rate */
    if (mode->refresh_rate) {
        target_refresh_rate = mode->refresh_rate;
    }
    else {
        target_refresh_rate = display->desktop_mode.refresh_rate;
    }

    match = nullptr;
    for (i = 0; i < GetNumDisplayModesForDisplay(display); ++i) {
        current = &display->display_modes[i];

        if (current->w && (current->w < mode->w)) {
            /* Out of sorted modes large enough here */
            break;
        }
        if (current->h && (current->h < mode->h)) {
            if (current->w && (current->w == mode->w)) {
                /* Out of sorted modes large enough here */
                break;
            }
            /* Wider, but not tall enough, due to a different
            aspect ratio. This mode must be skipped, but closer
            modes may still follow. */
            continue;
        }
        if (!match || current->w < match->w || current->h < match->h) {
            match = current;
            continue;
        }
        //if (current->format != match->format) {
        //    /* Sorted highest depth to lowest */
        //    if (current->format == target_format ||
        //        (SDL_BITSPERPIXEL(current->format) >=
        //            SDL_BITSPERPIXEL(target_format)
        //            && SDL_PIXELTYPE(current->format) ==
        //            SDL_PIXELTYPE(target_format))) {
        //        match = current;
        //    }
        //    continue;
        //}
        if (current->refresh_rate != match->refresh_rate) {
            /* Sorted highest refresh to lowest */
            if (current->refresh_rate >= target_refresh_rate) {
                match = current;
            }
        }
    }
    if (match) {
        if (match->format) {
            closest->format = match->format;
        }
        else {
            closest->format = mode->format;
        }
        if (match->w && match->h) {
            closest->w = match->w;
            closest->h = match->h;
        }
        else {
            closest->w = mode->w;
            closest->h = mode->h;
        }
        if (match->refresh_rate) {
            closest->refresh_rate = match->refresh_rate;
        }
        else {
            closest->refresh_rate = mode->refresh_rate;
        }

        /*
        * Pick some reasonable defaults if the app and driver don't
        * care
        */
        //if (!closest->format) {
        //    closest->format = SDL_PIXELFORMAT_RGB888;
        //}
        if (!closest->w) {
            closest->w = 640;
        }
        if (!closest->h) {
            closest->h = 480;
        }
        return closest;
    }
    return NULL;
}
bool DuiRealApplication::GetWindowDisplayMode(DuiRealWindow * window, DisplayMode * mode)
{
    DisplayMode fullscreen_mode;
    VideoDisplay *display;

    if (!mode) {
        return false;
    }

    fullscreen_mode = window->fullscreen_mode_;
    if (!fullscreen_mode.w) {
        fullscreen_mode.w = window->windowed_.width();
    }
    if (!fullscreen_mode.h) {
        fullscreen_mode.h = window->windowed_.height();
    }

    display = GetDisplayForWindow(window);

    /* if in desktop size mode, just return the size of the desktop */
    if ((window->flags_ & kFullscreenDesktop_Window) == kFullscreenDesktop_Window) {
        fullscreen_mode = display->desktop_mode;
    }
    else if (!GetClosestDisplayModeForDisplay(GetDisplayForWindow(window),
        &fullscreen_mode,
        &fullscreen_mode)) {
        return false;
    }

    if (mode) {
        *mode = fullscreen_mode;
    }
    return true;
}

bool DuiRealApplication::SetDisplayModeForDisplay(VideoDisplay * display, const DisplayMode * mode)
{
    DisplayMode display_mode;
    DisplayMode current_mode;

    if (mode) {
        display_mode = *mode;

        /* Default to the current mode */
        if (!display_mode.format) {
            display_mode.format = display->current_mode.format;
        }
        if (!display_mode.w) {
            display_mode.w = display->current_mode.w;
        }
        if (!display_mode.h) {
            display_mode.h = display->current_mode.h;
        }
        if (!display_mode.refresh_rate) {
            display_mode.refresh_rate = display->current_mode.refresh_rate;
        }

        /* Get a good video mode, the closest one possible */
        if (!GetClosestDisplayModeForDisplay(display, &display_mode, &display_mode)) {
            return false;
        }
    }
    else {
        display_mode = display->desktop_mode;
    }

    /* See if there's anything left to do */
    current_mode = display->current_mode;
    if (memcmp(&display_mode, &current_mode, sizeof(display_mode)) == 0) {
        return true;
    }

    /* Actually change the display mode */
    if (!WIN_SetDisplayMode(display, &display_mode)) {
        return false;
    }
    display->current_mode = display_mode;
    return true;
}
