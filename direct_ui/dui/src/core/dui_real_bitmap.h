#ifndef DUI_CORE_REAL_BITMAP_H_
#define DUI_CORE_REAL_BITMAP_H_

#include "dui/core/dui_bitmap.h"

class DuiRealBitmap {
public:
    DuiRealBitmap(DuiBitmap * owner);
    virtual ~DuiRealBitmap();

    DuiColor GetPixel(int x, int y);
    void SetPixel(int x, int y);

    virtual void Clear(DuiColor color) = 0;

public:
    static DuiBitmap * MakeBitmap(int width, int height, DuiColor pure_color, DuiPixelType pixel_type = kDefault_Pixel, DuiBitmapType type = kDefault_Bitmap);

private:
    static DuiBitmap * MakePlatformBitmap(int width, int height, DuiColor pure_color, DuiPixelType pixel_type = kDefault_Pixel);


protected:
    DuiBitmap * owner_;
};

#endif//DUI_CORE_REAL_BITMAP_H_
