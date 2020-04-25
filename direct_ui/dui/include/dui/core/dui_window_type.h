#ifndef DUI_CORE_WINDOW_TYPE_H_
#define DUI_CORE_WINDOW_TYPE_H_

#include "dui/core/dui_config.h"
#include <stdint.h>

enum DuiWindowType {
    kFullscreen_Window          = 0x00000001,      /**< fullscreen window */
    kChild_Window               = 0x00000002,      /**< child window */
    kShown_Window               = 0x00000004,      /**< window is visible */
    kHidden_Window              = 0x00000008,      /**< window is not visible */
    kBorderless_Window          = 0x00000010,      /**< no window decoration */
    kResizable_Window           = 0x00000020,      /**< window can be resized */
    kMinimized_Window           = 0x00000040,      /**< window is minimized */
    kMaximized_Window           = 0x00000080,      /**< window is maximized */
    kFullscreenDesktop_Window   = (kFullscreen_Window | 0x00001000),
    kForeign_Window             = 0x00000800,      /**< window not created by DUI */
    kAlwaysOnTop_Window         = 0x00008000,      /**< window should always be above others */
    kSkipTaskbar_Window         = 0x00010000,      /**< window should not be added to the taskbar */
    kUtility_Window             = 0x00020000,      /**< window should be treated as a utility window */
    kTooltip_Window             = 0x00040000,      /**< window should be treated as a tooltip */
    kPopupMenu_Window           = 0x00080000,      /**< window should be treated as a popup menu */
    kLayered_Window             = 0x00100000       /**< window should be treated as a layered */
};

/**
 *  \brief Used to indicate that you don't care what the window position is.
 */
const uint32_t kUndefinedMask_WindowPos = 0x1FFF0000u;
const uint32_t kUndefined_WindowPos = kUndefinedMask_WindowPos;
#define DuiWindowPos_IsUndefined(X) (((X)&0xFFFF0000) == kUndefinedMask_WindowPos)

/**
 *  \brief Used to indicate that the window position should be centered.
 */
const uint32_t kCenteredMask_WindowPos = 0x2FFF0000u;
const uint32_t kCentered_WindowPos = kCenteredMask_WindowPos;
#define DuiWindowPos_IsCentered(X) (((X)&0xFFFF0000) == kCenteredMask_WindowPos)

using DuiWindowFlags = uint32_t;  /**window type*/

using DuiWindowHandle = void*;  /*native window*/

enum DuiHitTestResult {
    kNormal_HitTest,                /**< Region is normal. No special properties. */
    kDraggable_HitTest,             /**< Region can drag entire window. */
    kResizeTopleft_HitTest,
    kResizeTop_HitTest,
    kResizeTopright_HitTest,
    kResizeRight_HitTest,
    kResizeBottomright_HitTest,
    kResizeBottom_HitTest,
    kResizeBottomleft_HitTest,
    kResizeLeft_HitTest
};

#endif  //DUI_CORE_WINDOW_TYPE_H_
