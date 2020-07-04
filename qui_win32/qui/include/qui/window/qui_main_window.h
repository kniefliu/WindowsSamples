#ifndef QUI_WINDOW_MAIN_WINDOW_H_
#define QUI_WINDOW_MAIN_WINDOW_H_

#include "qui/core/qui_prerequisites.h"
#include "qui/core/qui_window.h"

class QUI_EXPORT QuiMainWindow : public QuiWindow {
public:
    QuiMainWindow();
    ~QuiMainWindow() override;

protected:
    const wchar_t* GetWindowClassName() const override;
    QuiWindowLResult HandleMessage(QuiWindowMessageID msg, QuiWindowWParam w_param, QuiWindowLParam l_param) override;
    void OnFinalMessage(QuiWindowHandle hWnd) override;
};

#endif   // QUI_WINDOW_MAIN_WINDOW_H_
