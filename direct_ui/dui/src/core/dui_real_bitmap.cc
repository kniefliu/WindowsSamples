#include "dui_real_bitmap.h"

DuiRealBitmap::DuiRealBitmap(DuiBitmap * owner)
    : owner_(owner)
{
    owner_->real_bitmap_ = this;
}
DuiRealBitmap::~DuiRealBitmap()
{

}

DuiBitmap * DuiRealBitmap::MakeBitmap(int width, int height, DuiColor pure_color, DuiPixelType pixel_type/* = kDefault_Pixel*/, DuiBitmapType type/* = kDefault_Bitmap*/)
{
    return MakePlatformBitmap(width, height, pure_color, pixel_type);
}
