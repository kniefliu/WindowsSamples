#include "dui/core/dui_window.h"

#include "core/dui_real_window.h"
#include "dui/window/dui_tooltip_window.h"


DuiWindow::DuiWindow(bool destroy_by_self)
    : real_window_(nullptr)
    , destroy_by_self_(destroy_by_self)
{

}
DuiWindow::~DuiWindow()
{
    if (real_window_) {
        delete real_window_;
    }
}

DuiWindowHandle DuiWindow::Create(const wchar_t *title, int x, int y, int w, int h, DuiWindowFlags flags, DuiWindow * parent)
{
    DuiWindowHandle ret_handle = nullptr;
    if (real_window_ && real_window_->GetHandle()) {
        return real_window_->GetHandle();
    }
    if (!real_window_) {
        real_window_ = this->OnCreate();
        if (!real_window_) {
            real_window_ = DuiRealWindow::MakeRealWindow(this);
        }
        ret_handle = real_window_->Create(title, x, y, w, h, flags, parent);
        real_window_->OnInit();
    }
    return ret_handle;
}
DuiWindowHandle DuiWindow::CreateFrom(DuiWindowHandle extern_handle)
{
    if (real_window_ && real_window_->GetHandle()) {
        return real_window_->GetHandle();
    }
    if (!real_window_) {
        real_window_ = DuiRealWindow::MakeRealWindow(this);
        return real_window_->CreateFrom(extern_handle);
    }
    return nullptr;
}
void DuiWindow::Add(DuiWindow * child)
{
    real_window_->Add(child);
}
DuiWindowHandle DuiWindow::GetHandle() const
{
    return real_window_->GetHandle();
}

void DuiWindow::SetHoverTime(int time)
{
    return real_window_->SetHoverTime(time);
}

DuiPoint DuiWindow::GetLocation() const
{
    return real_window_->GetLocation();
}
DuiPoint DuiWindow::GetLocationOnScreen() const
{
    return real_window_->GetLocationOnScreen();
}
DuiSize DuiWindow::GetSize() const
{
    return real_window_->GetSize();
}
DuiSize DuiWindow::GetMaximumSize() const
{
    return real_window_->GetMaximumSize();
}
DuiSize DuiWindow::GetMinimumSize() const
{
    return real_window_->GetMinimumSize();
}
void DuiWindow::SetLocation(const DuiPoint& pt)
{
    real_window_->SetLocation(pt);
}
void DuiWindow::SetSize(const DuiSize& sz)
{
    real_window_->SetSize(sz);
}
void DuiWindow::SetMinimumSize(const DuiSize& sz)
{
    real_window_->SetMinimumSize(sz);
}
void DuiWindow::SetMaximumSize(const DuiSize& sz)
{
    real_window_->SetMaximumSize(sz);
}

DuiPoint DuiWindow::GetMousePosition()
{
    return real_window_->GetMousePosition();
}

void DuiWindow::Invalidate(const DuiRect* repaint_area/* = nullptr*/)
{
    real_window_->Invalidate(repaint_area);
}

int DuiWindow::GetNativeWindowCount() const
{
    return real_window_->GetNativeWindowCount();
}
DuiRect DuiWindow::GetNativeWindowRect(DuiWindowHandle child_window) const
{
    return real_window_->GetNativeWindowRect(child_window);
}
bool DuiWindow::AddNativeWindow(DuiControlBase * controller, DuiWindowHandle child_window)
{
    return real_window_->AddNativeWindow(controller, child_window);
}
bool DuiWindow::RemoveNativeWindow(DuiWindowHandle child_window)
{
    return real_window_->RemoveNativeWindow(child_window);
}

DuiAlpha DuiWindow::GetOpacity() const
{
    return real_window_->GetOpacity();
}
void DuiWindow::SetOpacity(DuiAlpha nOpacity)
{
    real_window_->SetOpacity(nOpacity);
}
bool DuiWindow::IsLayered()
{
    return real_window_->IsLayered();
}
void DuiWindow::SetLayered(bool bLayered)
{
    real_window_->SetLayered(bLayered);
}
DuiAlpha DuiWindow::GetLayeredOpacity()
{
    return real_window_->GetLayeredOpacity();
}
void DuiWindow::SetLayeredOpacity(DuiAlpha nOpacity)
{
    real_window_->SetLayeredOpacity(nOpacity);
}

bool DuiWindow::OnHandleEvent(const DuiWindowEvent * event, DuiWindowEventResult& result)
{
    return false;
}

bool DuiWindow::HandleEvent(const DuiWindowEvent * event, DuiWindowEventResult& result)
{
    return OnHandleEvent(event, result);
}
