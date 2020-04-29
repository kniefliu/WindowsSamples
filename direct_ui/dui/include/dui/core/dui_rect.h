#ifndef DUI_CORE_RECT_H_
#define DUI_CORE_RECT_H_

#include "dui/core/dui_size.h"
#include "dui/core/dui_point.h"
#include <stdint.h>
#include <memory.h>

struct DuiRect {
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;

    static DuiRect MakeEmpty() {
        DuiRect r;
        r.SetEmpty();
        return r;
    }
    static DuiRect MakeWH(int32_t w, int32_t h) {
        DuiRect r;
        r.Set(0, 0, w, h);
        return r;
    }
    static DuiRect MakeSize(const DuiSize& size) {
        DuiRect r;
        r.Set(0, 0, size.width, size.height);
        return r;
    }
    static DuiRect MakeLTRB(int32_t l, int32_t t, int32_t r, int32_t b) {
        DuiRect rect;
        rect.Set(l, t, r, b);
        return rect;
    }
    static DuiRect MakeXYWH(int32_t x, int32_t y, int32_t w, int32_t h) {
        DuiRect r;
        r.Set(x, y, x + w, y + h);
        return r;
    }

    int x() const { return left; }
    int y() const { return top; }
    int width() const { return right - left; }
    int height() const { return bottom - top; }

    void SetEmpty() { memset(this, 0, sizeof(*this)); }
    void Set(int32_t left, int32_t top, int32_t right, int32_t bottom) {
        this->left = left;
        this->top = top;
        this->right = right;
        this->bottom = bottom;
    }
    // alias for set(l, t, r, b)
    void SetLTRB(int32_t left, int32_t top, int32_t right, int32_t bottom) {
        this->Set(left, top, right, bottom);
    }
    void SetXYWH(int32_t x, int32_t y, int32_t width, int32_t height) {
        this->left = x;
        this->top = y;
        this->right = x + width;
        this->bottom = y + height;
    }
    void SetX(int32_t x) {
        int32_t w = right - left;
        this->left = x;
        this->right = x + w;
    }
    void SetY(int32_t y) {
        int32_t h = bottom - top;
        this->top = y;
        this->bottom = y + h;
    }
    void SetWidth(int32_t w) {
        this->right = left + w;
    }
    void SetHeight(int32_t h) {
        this->bottom = top + h;
    }
    bool IsEmpty() const { return left >= right || top >= bottom; }
    bool Intersect(int32_t rhs_left, int32_t rhs_top, int32_t rhs_right, int32_t rhs_bottom) {
        if (rhs_left < rhs_right && rhs_top < rhs_bottom && !this->IsEmpty() &&
            left < rhs_right && rhs_left < right && top < rhs_bottom && rhs_top < bottom) {
            if (left < rhs_left) left = rhs_left;
            if (top < rhs_top) top = rhs_top;
            if (right > rhs_right) right = rhs_right;
            if (bottom > rhs_bottom) bottom = rhs_bottom;
            return true;
        }
        return false;
    }
    bool Intersect(const DuiRect& r) {
        return this->Intersect(r.left, r.top, r.right, r.bottom);
    }
    void Join(int32_t left, int32_t top, int32_t right, int32_t bottom) {
        // do nothing if the params are empty
        if (left >= right || top >= bottom) {
            return;
        }

        // if we are empty, just assign
        if (this->left >= this->right || this->top >= this->bottom) {
            this->Set(left, top, right, bottom);
        }
        else {
            if (left < this->left) this->left = left;
            if (top < this->top) this->top = top;
            if (right > this->right) this->right = right;
            if (bottom > this->bottom) this->bottom = bottom;
        }
    }
    void Join(const DuiRect& r) {
        this->Join(r.left, r.top, r.right, r.bottom);
    }
    bool Contains(int32_t x, int32_t y) const {
        return  (unsigned)(x - left) < (unsigned)(right - left) &&
            (unsigned)(y - top) < (unsigned)(bottom - top);
    }
    bool Contains(const DuiPoint& pt) const {
        return this->Contains(pt.x, pt.y);
    }
};

#endif  //DUI_CORE_RECT_H_
