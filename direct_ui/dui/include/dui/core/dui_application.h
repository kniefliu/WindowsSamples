#ifndef DUI_CORE_APPLICATION_H_
#define DUI_CORE_APPLICATION_H_

#include "dui/core/dui_config.h"

class DUI_API DuiApplication final {
public:
    static void Init();
    static void Run();
    static void Quit();
};

#endif//DUI_CORE_APPLICATION_H_
