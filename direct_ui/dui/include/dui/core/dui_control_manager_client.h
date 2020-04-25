#ifndef DUI_CORE_CONTROL_MANAGER_CLIENT_H_
#define DUI_CORE_CONTROL_MANAGER_CLIENT_H_

#include "dui/core/dui_config.h"
#include "dui/core/dui_window_type.h"
#include "dui/core/dui_rect.h"

class DUI_API DuiControlBase {
public:

};

class DUI_API DuiControlManagerClient {
public:
    virtual DuiPoint GetLocation() const = 0;
    virtual DuiPoint GetLocationOnScreen() const = 0;
    virtual DuiSize GetSize() const = 0;
    virtual DuiSize GetMaximumSize() const = 0;
    virtual DuiSize GetMinimumSize() const = 0;
    virtual void SetLocation(const DuiPoint& pt) = 0;
    virtual void SetSize(const DuiSize& sz) = 0;
    virtual void SetMinimumSize(const DuiSize& sz) = 0;
    virtual void SetMaximumSize(const DuiSize& sz) = 0;

    virtual DuiPoint GetMousePosition() = 0;

public:
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
};

#endif //DUI_CORE_CONTROL_MANAGER_CLIENT_H_
