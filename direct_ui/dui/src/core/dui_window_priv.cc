#include "dui_window_priv.h"

DuiWindowPriv::DuiWindowPriv(DuiWindow * window)
    : window_(window)
{

}
DuiWindowPriv::~DuiWindowPriv()
{

}

void DuiWindowPriv::OnDestoryed()
{
    if (!window_) {
        return;
    }
    return window_->OnDestoryed();
}
