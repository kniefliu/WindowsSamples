#include "qui/qui.h"

#include <Windows.h>

class FrameWindow : public QuiMainWindow {
public:
    FrameWindow()
    {
    }
    ~FrameWindow() override
    {
    }

protected:
    const wchar_t* GetWindowClassName() const override
    {
        return L"FrameWindow";
    }
    QuiWindowLResult HandleMessage(QuiWindowMessageID msg, QuiWindowWParam w_param, QuiWindowLParam l_param) override
    {
        return QuiMainWindow::HandleMessage(msg, w_param, l_param);
    } 
    void OnFinalMessage(QuiWindowHandle hWnd) override
    {
        delete this;
    }
    
};

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE /*prev_instance*/, LPSTR /*cmd_line*/, int cmd_show)
{
    QuiApplication::SetInstance(instance);

    FrameWindow* frame_window = new FrameWindow();
    if (!frame_window) {
        return 0;
    }
    
    frame_window->Create(0, L"TestAppFrameWindow", WS_OVERLAPPEDWINDOW, 0, { 0, 0, 800, 600 });
    frame_window->CenterWindow();
    frame_window->ShowWindow(true);
    QuiApplication::Run();

    
    return 0;
}
