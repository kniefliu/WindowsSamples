#include "dui/window/dui_tooltip_window.h"

DuiTooltipWindow::DuiTooltipWindow(bool destroy_by_self/* = true*/)
    : DuiWindow(destroy_by_self)
    , client_(nullptr)
    , tooltip_width_(0)
{

}

void DuiTooltipWindow::Init(DuiTooltipClient * client)
{
    if (client_) {
        return;
    }
    if (!client)
        return;
    client_ = client;
    real_window_ = DuiRealWindow::MakeRealTooltipWindow(this);
    real_window_->OnInit();
    //Create(nullptr, 100, 200, 100, 100, kBorderless_Window, client->GetHostWindow());
}
DuiTooltipClient * DuiTooltipWindow::GetClient()
{
    return client_;
}

void DuiTooltipWindow::SetTooltipWindowWidth(int width)
{
    if (tooltip_width_ != width) {
        tooltip_width_ = real_window_->SetTooltipWidth(width);
    }
}
void DuiTooltipWindow::ShowTooltip()
{
    real_window_->ShowTooltip();
}
void DuiTooltipWindow::HideTooltip()
{
    real_window_->HideTooltip();
}
void DuiTooltipWindow::MoveTooltip(const DuiPoint& loc)
{
    real_window_->MoveTooltip(loc);
}
int DuiTooltipWindow::GetTooltipTime()
{
    return real_window_->GetTooltipTime();
}

DuiWindowHandle DuiTooltipWindow::Create(const wchar_t *title, int x, int y, int w, int h,
    DuiWindowFlags flags, DuiWindow * parent/* = nullptr*/)
{
    return DuiWindow::Create(title, x, y, w, h, flags, parent);
}

void DuiTooltipWindow::OnDestoryed()
{
    delete this;
}

DuiRealWindow * DuiTooltipWindow::OnCreate()
{
    return DuiRealWindow::MakeRealTooltipWindow(this);
}
