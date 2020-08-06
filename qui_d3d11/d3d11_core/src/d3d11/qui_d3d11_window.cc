#include "qui/d3d11/qui_d3d11_window.h"

#include "qui_d3d11_implementor.h"

QuiD3D11Window::QuiD3D11Window(QuiD3D11ImplementorType type)
    : impl_(nullptr)
{
    if (type == kCube_Implementor) {
        impl_ = new CubeImplementor(this);
    }
    else if (type == kRect_Implementor) {
        impl_ = new RectImplementor(this);
    }
    else {
        impl_ = new QuiD3D11Implementor(this);
    }
}
QuiD3D11Window::~QuiD3D11Window()
{

}

QuiD3D11Window::QuiD3D11Window(QuiD3D11Implementor *impl)
    : impl_(impl)
{

}

const wchar_t* QuiD3D11Window::GetWindowClassName() const
{
    return L"D3D11Window";
}
DWORD QuiD3D11Window::GetClassStyle() const
{
    return CS_VREDRAW | CS_HREDRAW;
}

LRESULT QuiD3D11Window::HandleMessage(UINT msg, WPARAM w_param, LPARAM l_param)
{
    if (msg == WM_CREATE) {
        ::SetTimer(*this, kPaintTimerID, kPaintInterval, 0);
    }
    else if (msg == WM_PAINT) {
        impl_->Render();
    }
    else if (msg == WM_ERASEBKGND) {
        return 1;
    }
    else if (msg == WM_TIMER) {
        if (w_param == kPaintTimerID) {
            //impl_->Render();
            ::InvalidateRect(*this, 0, TRUE);
        }
    }
    else if (msg == WM_SIZE) {
        impl_->InitDevice();
    }
    else if (msg == WM_DESTROY) {
        ::KillTimer(*this, kPaintTimerID);
        impl_->CleanupDevice();
    }
    return QuiWindow::HandleMessage(msg, w_param, l_param);
}
void QuiD3D11Window::OnFinalMessage(HWND hWnd)
{

}