#include "dui/core/dui_bitmap.h"

#include "core/dui_real_bitmap.h"

DuiBitmap::~DuiBitmap()
{
    delete real_bitmap_;
    real_bitmap_ = nullptr;
}

DuiBitmap * DuiBitmap::MakeBitmap(int width, int height, DuiColor pure_color, DuiPixelType pixel_type/* = kDefault_Pixel*/, DuiBitmapType type/* = kDefault_Bitmap*/)
{
    return DuiRealBitmap::MakeBitmap(width, height, pure_color, pixel_type, type);
}

void DuiBitmap::Clear(DuiColor color)
{
    return real_bitmap_->Clear(color);
}
