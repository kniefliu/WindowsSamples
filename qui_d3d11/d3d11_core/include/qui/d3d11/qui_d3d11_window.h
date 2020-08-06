#pragma once

#include "qui/core/qui_prerequisites.h"

#include "qui/core/qui_window.h"

enum QuiD3D11ImplementorType {
    kClear_Implementor,
    kCube_Implementor,
    kRect_Implementor,
};

class QuiD3D11Implementor;
class QUI_EXPORT QuiD3D11Window : public QuiWindow {
public:
    explicit QuiD3D11Window(QuiD3D11ImplementorType type);
    ~QuiD3D11Window();

protected:
    QuiD3D11Window(QuiD3D11Implementor *impl);

protected:
    const wchar_t* GetWindowClassName() const override;
    DWORD GetClassStyle() const override;

    LRESULT HandleMessage(UINT uMsg, WPARAM w_param, LPARAM l_param) override;
    void OnFinalMessage(HWND hWnd)  override;

protected:
    enum {
        kPaintTimerID = 100,
        kPaintInterval = 15
    };

private:
    QuiD3D11Implementor * impl_;
};
