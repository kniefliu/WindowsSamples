#include "qui/core/qui_bitmap.h"

QuiBitmap::QuiBitmap()
{

}
QuiBitmap::~QuiBitmap()
{

}

int QuiBitmap::GetWidth()
{
    return real_bitmap_->GetWidth();
}
int QuiBitmap::GetHeight()
{
    return real_bitmap_->GetHeight();
}
QuiBitmapColorType QuiBitmap::GetColorType()
{
    return real_bitmap_->GetColorType();
}
void QuiBitmap::BeginDraw()
{
    real_bitmap_->BeginDraw();
}
void QuiBitmap::EndDraw()
{
    real_bitmap_->EndDraw();
}
