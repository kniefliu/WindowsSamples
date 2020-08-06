#include "qui/core/qui_application.h"

#include <Windows.h>

HINSTANCE application_inst = nullptr;

void QuiApplication::SetInstance(HINSTANCE instance)
{
    application_inst = (HINSTANCE)instance;
}
HINSTANCE QuiApplication::GetInstance()
{
    return application_inst;
}
int QuiApplication::Run()
{
    MSG msg = { 0 };
    while (::GetMessage(&msg, NULL, 0, 0)) {
        if (true) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }
    return msg.wParam;
}
