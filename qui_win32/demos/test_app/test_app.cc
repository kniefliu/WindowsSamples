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
        QuiWindowLResult l_res = 0;
        bool handled = false;
        if (msg == WM_CREATE) {
            paint_manager_.Init(GetHWND());
            QuiControl * control = new QuiControl();
            control->SetBkColor(QUI_ColorBLUE);
            paint_manager_.AttachDialog(control);
        }
        if (handled)
            return l_res;
        handled = paint_manager_.MessageHandler(msg, w_param, l_param, l_res);
        if (handled) {
            return l_res;
        }
        return QuiMainWindow::HandleMessage(msg, w_param, l_param);
    } 
    void OnFinalMessage(QuiWindowHandle hWnd) override
    {
        delete this;
    }
    
private:
    QuiPaintManager paint_manager_;
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
