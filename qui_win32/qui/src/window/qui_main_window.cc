#include "qui/window/qui_main_window.h"

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
QuiWindowLResult QuiMainWindow::HandleMessage(QuiWindowMessageID msg, QuiWindowWParam w_param, QuiWindowLParam l_param)
{
    if (msg == WM_CLOSE) {
        ::PostQuitMessage(0);
        return 0;
    }
    return QuiWindow::HandleMessage(msg, w_param, l_param);
}
void QuiMainWindow::OnFinalMessage(QuiWindowHandle hWnd)
{
    delete this;
}