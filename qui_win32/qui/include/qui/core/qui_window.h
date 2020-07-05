#ifndef QUI_CORE_WINDOW_H_
#define QUI_CORE_WINDOW_H_

#include "qui/core/qui_prerequisites.h"
#include "qui/core/qui_rect.h"
#include "qui/core/qui_window_types.h"


class QuiWindowPrivate;
class QUI_EXPORT QuiWindow {
public:
    QuiWindow();
    virtual ~QuiWindow();

public:
    QuiWindowHandle GetHWND() const;
    operator QuiWindowHandle() const;

    QuiWindowHandle Create(QuiWindowHandle parent, const wchar_t* window_name, QuiWindowStyle window_style,
        QuiWindowExStyle window_ex_style, const QuiRect& area);
    QuiWindowHandle Create(QuiWindowHandle parent, const wchar_t* window_name, QuiWindowStyle window_style,
        QuiWindowExStyle window_ex_style,
        int x = QuiCreateWindow_USEDEFAULT, int y = QuiCreateWindow_USEDEFAULT,
        int w = QuiCreateWindow_USEDEFAULT, int h = QuiCreateWindow_USEDEFAULT);
    QuiWindowHandle CreateQuiWindow(QuiWindowHandle parent, const wchar_t* window_name, QuiWindowStyle window_style = 0,
        QuiWindowExStyle dwExStyle = 0);

    QuiWindowHandle Subclass(QuiWindowHandle win_handle);
    void Unsubclass();

    void ShowWindow(bool show = true, bool take_focus = true);
    void ShowModal();
    void Close();
    void CenterWindow();
    void SetIcon(QuiWindowIconID icon_id);
    void ResizeClient(int w = -1, int h = -1);

protected:
    virtual const wchar_t* GetWindowClassName() const = 0;
    virtual const wchar_t* GetSuperClassName() const { return nullptr; }
    virtual QuiWindowClassStyle GetClassStyle() const;

    virtual QuiWindowLResult HandleMessage(QuiWindowMessageID uMsg, QuiWindowWParam w_param, QuiWindowLParam l_param);
    virtual void OnFinalMessage(QuiWindowHandle hWnd);

protected:
    explicit QuiWindow(QuiWindowPrivate * priv);
    QuiWindowPrivate * GetPrivate() { return priv_; }
    const QuiWindowPrivate * GetPrivate() const { return priv_; }

private:
    QuiWindowPrivate * priv_;
    friend class QuiWindowPrivate;
};


#endif   // QUI_CORE_WINDOW_H_
