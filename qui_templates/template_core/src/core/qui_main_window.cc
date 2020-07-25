#include "qui/core/qui_main_window.h"

#include <Windows.h>

QuiMainWindow::QuiMainWindow()
{

}
QuiMainWindow::~QuiMainWindow()
{

}

const wchar_t* QuiMainWindow::GetWindowClassName() const
{
    return L"MainWindow";
}
LRESULT QuiMainWindow::HandleMessage(UINT msg, WPARAM w_param, LPARAM l_param)
{
    if (msg == WM_CLOSE) {
        ::PostQuitMessage(0);
        return 0;
    }
    return QuiWindow::HandleMessage(msg, w_param, l_param);
}
void QuiMainWindow::OnFinalMessage(HWND hWnd)
{
    delete this;
}