#ifndef DUI_CORE_CONTROL_MANAGER_H_
#define DUI_CORE_CONTROL_MANAGER_H_

#include "dui/core/dui_config.h"
#include "dui/core/dui_control_manager_client.h"

#include <string>

class  DuiControlManager {
public:
    DuiControlManager();
    ~DuiControlManager();

public:
    void Init(DuiControlManagerClient * client, const wchar_t* name = nullptr);

private:
    std::wstring name_;
    DuiControlManagerClient* client_;
};

#endif  //DUI_CORE_CONTROL_MANAGER_H_
