#ifndef DUI_CORE_POINT_H_
#define DUI_CORE_POINT_H_

#include <stdint.h>

struct DuiPoint {
    int32_t x;
    int32_t y;

    static DuiPoint Make(int32_t v_x, int32_t v_y) {
        DuiPoint pt;
        pt.set_x(v_x);
        pt.set_y(v_y);
    }

    //int32_t x() const { return x; }
    //int32_t y() const { return y; }
    void set_x(int32_t v) { x = v; }
    void set_y(int32_t v) { y = v; }

    void set(int32_t v_x, int32_t v_y) {
        x = v_x;
        y = v_y;
    }
};

#endif  //DUI_CORE_POINT_H_
