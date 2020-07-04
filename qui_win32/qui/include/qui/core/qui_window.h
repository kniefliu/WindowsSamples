#ifndef QUI_CORE_WINDOW_H_
#define QUI_CORE_WINDOW_H_

#include "qui/core/qui_prerequisites.h"
#include "qui/core/qui_rect.h"

// platform related 
typedef void * QuiWindowHandle;
typedef uint32_t QuiWindowStyle;
typedef uint32_t QuiWindowExStyle;
typedef uint32_t QuiWindowClassStyle;
typedef uint32_t QuiWindowMessageID;
typedef void * QuiWindowWParam;
typedef void * QuiWindowLParam;
typedef void * QuiWindowLResult;
typedef uint32_t QuiWindowIconID;
#define QuiNativeWindow_USEDEFAULT       ((int)0x80000000)

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
        int x = QuiNativeWindow_USEDEFAULT, int y = QuiNativeWindow_USEDEFAULT, 
        int w = QuiNativeWindow_USEDEFAULT, int h = QuiNativeWindow_USEDEFAULT);
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
