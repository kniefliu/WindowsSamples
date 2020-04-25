#ifndef DUI_CORE_WINDOW_PRIV_H_
#define DUI_CORE_WINDOW_PRIV_H_

#include "dui/core/dui_window.h"

class DuiWindowPriv final {
public:
    explicit DuiWindowPriv(DuiWindow * window);
    ~DuiWindowPriv();

    bool HandleEvent(const DuiWindowEvent * event, DuiWindowEventResult& result);
    void OnDestoryed();

private:
    DuiWindow* window_;
};

#endif  //DUI_CORE_WINDOW_PRIV_H_
