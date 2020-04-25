#include "dui/window/dui_edit_window.h"

#include "core/dui_real_window.h"

DuiEditWindow::DuiEditWindow(bool destroy_by_self)
    : DuiWindow(destroy_by_self)
    , client_(nullptr)
{

}

void DuiEditWindow::Init(DuiEditClient * client)
{
    if (client_) {
        return;
    }
    if (!client)
        return;
    client_ = client;
    Create(nullptr, 100, 200, 100, 100, kChild_Window, client->GetHostWindow());
}
DuiEditClient * DuiEditWindow::GetClient()
{
    return client_;
}

DuiWindowHandle DuiEditWindow::Create(const wchar_t *title, int x, int y, int w, int h, DuiWindowFlags flags, DuiWindow * parent/* = nullptr*/)
{
    return DuiWindow::Create(title, x, y, w, h, flags, parent);
}

void DuiEditWindow::OnDestoryed()
{
    delete this;
}

DuiRealWindow * DuiEditWindow::OnCreate()
{
    return DuiRealWindow::MakeRealEditWindow(this);
}