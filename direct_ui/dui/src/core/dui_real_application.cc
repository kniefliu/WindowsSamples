#include "dui_real_application.h"

static DuiRealWindow * windows = 0;

DuiRealWindow* DuiRealApplication::GetWindows()
{
    return windows;
}
void DuiRealApplication::SetWindows(DuiRealWindow * window)
{
    windows = window;
}

float DuiRealApplication::ComputeDiagonalDPI(int hpix, int vpix, float hinches, float vinches)
{
    float den2 = hinches * hinches + vinches * vinches;
    if (den2 <= 0.0f) {
        return 0.0f;
    }

    return (float)(std::sqrt((double)hpix * (double)hpix + (double)vpix * (double)vpix) /
        std::sqrt((double)den2));
}
