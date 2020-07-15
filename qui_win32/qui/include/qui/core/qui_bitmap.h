#ifndef QUI_CORE_BITMAP_H_
#define QUI_CORE_BITMAP_H_

#include "qui/core/qui_prerequisites.h"
#include "qui/core/qui_bitmap_base.h"
#include "qui/core/qui_window_types.h"

class QUI_EXPORT QuiBitmap : public QuiBitmapBase {
public:
    static QuiBitmap * MakeGDIBitmap(int width, int height, QuiBitmapColorType color_type = kRGB8888_BitmapColor, QuiWindowHandle window_handle = nullptr);

public:
    ~QuiBitmap() override;

    int GetWidth() override;
    int GetHeight() override;
    QuiBitmapColorType GetColorType() override;
    void BeginDraw() override;
    void EndDraw() override;

private:
    QuiBitmap();

private:
    QuiBitmapBase * real_bitmap_;

private:
    friend class QuiCanvas;
    friend class QuiRealGDICanvas;
};

#endif   // QUI_CORE_BITMAP_H_
