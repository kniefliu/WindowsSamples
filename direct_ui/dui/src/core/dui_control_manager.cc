#include "dui/core/dui_control_manager.h"

DuiControlManager::DuiControlManager()
    : client_(nullptr)
{

}
DuiControlManager::~DuiControlManager()
{

}

void DuiControlManager::Init(DuiControlManagerClient * client, const wchar_t* name/* = nullptr*/)
{
    if (!client) {
        return;
    }

    if (name) {
        name_ = name;
    }

    if (client_ != client) {
        client_ = client;
    }
}