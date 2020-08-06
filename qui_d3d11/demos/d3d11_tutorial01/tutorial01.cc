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
    LRESULT HandleMessage(UINT msg, WPARAM w_param, LPARAM l_param) override
    {
        LRESULT l_res = 0;
        bool handled = false;
        if (msg == WM_CREATE) {
            child_window_ = new QuiD3D11Window(kRect_Implementor);
            child_window_->Create(*this, L"child", WS_CHILD | WS_VISIBLE, 0);
            handled = true;
        }
        else if (msg == WM_SIZE) {
            RECT rc;
            ::GetClientRect(*this, &rc);
            QuiRect client_area = rc;
            child_window_->ResizeClient(client_area.Width(), client_area.Height());
            handled = true;
        }
        if (handled)
            return l_res;
        if (handled) {
            return l_res;
        }
        return QuiMainWindow::HandleMessage(msg, w_param, l_param);
    } 
    void OnFinalMessage(HWND hWnd) override
    {
        delete this;
    }

    QuiD3D11Window * child_window_;
};

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE /*prev_instance*/, LPSTR /*cmd_line*/, int cmd_show)
{
    QuiApplication::SetInstance(instance);

    FrameWindow* frame_window = new FrameWindow();
    if (!frame_window) {
        return 0;
    }
    
    RECT area = { 0, 0, 800, 600 };
    QuiRect expected_area = area;
    frame_window->Create(0, L"TestAppFrameWindow", WS_OVERLAPPEDWINDOW, 0, expected_area);
    frame_window->CenterWindow();
    frame_window->ShowWindow(true);
    QuiApplication::Run();

    
    return 0;
}
