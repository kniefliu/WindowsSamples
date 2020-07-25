#ifndef QUI_CORE_MAIN_WINDOW_H_
#define QUI_CORE_MAIN_WINDOW_H_

#include "qui/core/qui_prerequisites.h"
#include "qui/core/qui_rect.h"

class QUI_EXPORT QuiMainWindow : public QuiWindow {
public:
    QuiMainWindow();
    ~QuiMainWindow() override;

protected:
    const wchar_t* GetWindowClassName() const override;
    LRESULT HandleMessage(UINT msg, WPARAM w_param, LPARAM l_param) override;
    void OnFinalMessage(HWND hWnd) override;
};


#endif   // QUI_CORE_MAIN_WINDOW_H_
