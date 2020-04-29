#ifndef DUI_CORE_SIZE_H_
#define DUI_CORE_SIZE_H_

#include <stdint.h>

struct DuiSize {
    int32_t width;
    int32_t height;

    static DuiSize Make(int32_t w, int32_t h) { return { w, h }; }
    static DuiSize MakeEmpty() { return { 0, 0 }; }

    void set(int32_t w, int32_t h) { *this = DuiSize{ w, h }; }

    //int32_t width() const { return width; }
    //int32_t height() const { return height; }
};

#endif  //DUI_CORE_SIZE_H_
