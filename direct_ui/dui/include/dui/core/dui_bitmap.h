#ifndef DUI_CORE_BITMAP_H_
#define DUI_CORE_BITMAP_H_

#include "dui/core/dui_config.h"
#include "dui/core/dui_color.h"

enum DuiBitmapType {
    kPlatform_Bitmap,
    kDefault_Bitmap = kPlatform_Bitmap
};

enum DuiPixelType {
    kARGB8888_Pixel,
    kRGB8888_Pixel,
    kDefault_Pixel = kARGB8888_Pixel,
};

class DuiRealBitmap;
class DUI_API DuiBitmap final {
public:
    ~DuiBitmap();

public:
    static DuiBitmap * MakeBitmap(int width, int height, DuiColor pure_color, DuiPixelType pixel_type = kDefault_Pixel, DuiBitmapType type = kDefault_Bitmap);

public:
    void Clear(DuiColor color = DUI_ColorTRANSPARENT);

private:
    DuiRealBitmap * real_bitmap_;

    friend class DuiRealBitmap;
    friend class DuiRealCanvas;
};

#endif//DUI_CORE_BITMAP_H_
