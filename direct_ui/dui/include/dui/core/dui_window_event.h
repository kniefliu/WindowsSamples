#ifndef DUI_CORE_EVENT_H_
#define DUI_CORE_EVENT_H_

#include <stdint.h>
#include "dui_point.h"
#include "dui_size.h"
#include "dui_window_type.h"
#include "dui_canvas.h"

const uint32_t kTextSize_TextEditingEvent = 32;

enum DuiWindowEventType {
    kNone_WindowEvent,
    kCreate_WindowEvent,
    kToDestroy_WindowEvent,
    kMoved_WindowEvent,
    kMinimized_WindowEvent,
    kMaximized_WindowEvent,
    kRestored_WindowEvent,
    kResized_WindowEvent,
    kActive_WindowEvent,          /*Activated by some method other than a mouse click*/
    kClickActive_WindowEvent,     /*Activated by a mouse click.*/
    kInactive_WindowEvent,        /*Deactivated*/
    kSetFocus_WindowEvent,
    kKillFocus_WindowEvent,
    kEnabled_WindowEvent,
    kDisabled_WindowEvent,
    kPaint_WindowEvent,
    kPrintClient_WindowEvent,
    kFlush_WindowEvent,
    kClose_WindowEvent,
    kErasebkgnd_WindowEvent,
    kShown_WindowEvent,
    kHidden_WindowEvent,
    kSetCursor_WindowEvent,
    kExposed_WindowEvent,
    kSizeChanged_WindowEvent,
    kEnter_WindowEvent,
    kLeave_WindowEvent,
    kFocusGained_WindowEvent,
    kFocusLost_WindowEvent,
    kTakeFocus_WindowEvent,
    kHitTest_WindowEvent,
    kMouseHover_WindowEvent,
    kMouseMove_WindowEvent,
    kMouseLeave_WindowEvent,
};

struct DuiWindowEvent {
    DuiWindowEventType type;
    uint32_t timestamp;
    /* The position of the mouse pointer in this Window's coordinate space 
     * if the Window is directly under the mouse pointer, otherwise returns null. */
    DuiPoint mouse_position; 
    DuiCanvas * canvas;
    DuiRect paint_area;
    
    // w_param
    union {
        uint64_t padding1;
    };
    
    // l_param
    union {
        uint64_t padding2;
        DuiWindowHandle window_handle;
        DuiPoint location;
        DuiSize size;
    };
};

using DuiWindowEventResult = int32_t;

#endif  //DUI_CORE_EVENT_H_
