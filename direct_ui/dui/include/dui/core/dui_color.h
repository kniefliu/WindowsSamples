#ifndef DUI_CORE_COLOR_H_
#define DUI_CORE_COLOR_H_

#include <stdint.h>

using DuiAlpha = uint8_t;
using DuiColor = uint32_t;
using DuiColor8BitComponent = unsigned;

static inline DuiColor DuiColorSetARGBInline(DuiColor8BitComponent a, DuiColor8BitComponent r, DuiColor8BitComponent g, DuiColor8BitComponent b)
{
    return ((a << 24) | (r << 16) | (g << 8) | (b << 0));
}

#define DuiColorSetARGBMacro(a, r, g, b) \
    static_cast<DuiColor>( \
        (static_cast<DuiColor8BitComponent>(a) << 24) | \
        (static_cast<DuiColor8BitComponent>(r) << 16) | \
        (static_cast<DuiColor8BitComponent>(g) << 8) | \
        (static_cast<DuiColor8BitComponent>(b) << 0))

#if defined(NDEBUG)
#define DuiColorSetARGB(a, r, g, b) DuiColorSetARGBMacro(a, r, g, b)
#else
#define DuiColorSetARGB(a, r, g, b) DuiColorSetARGBInline(a, r, g, b)
#endif

#define DuiColorSetRGB(r, g, b)  DuiColorSetARGB(0xFF, r, g, b)

/** return the alpha byte from a DuiColor value */
#define DuiColorGetA(color)      (((color) >> 24) & 0xFF)
/** return the red byte from a DuiColor value */
#define DuiColorGetR(color)      (((color) >> 16) & 0xFF)
/** return the green byte from a DuiColor value */
#define DuiColorGetG(color)      (((color) >>  8) & 0xFF)
/** return the blue byte from a DuiColor value */
#define DuiColorGetB(color)      (((color) >>  0) & 0xFF)

static constexpr inline DuiColor DuiColorSetA(DuiColor c, DuiColor8BitComponent a) {
    return (c & 0x00FFFFFF) | (a << 24);
}

// common colors

/** transparent DuiAlpha value */
#define DUI_AlphaTRANSPARENT static_cast<DuiAlpha>(0x00)
/** opaque DuiAlpha value */
#define DUI_AlphaOPAQUE      static_cast<DuiAlpha>(0xFF)

/** transparent DuiColor value */
#define DUI_ColorTRANSPARENT static_cast<DuiColor>(0x00000000)

/** black DuiColor value */
#define DUI_ColorBLACK       static_cast<DuiColor>(0xFF000000)
/** dark gray DuiColor value */
#define DUI_ColorDKGRAY      static_cast<DuiColor>(0xFF444444)
/** gray DuiColor value */
#define DUI_ColorGRAY        static_cast<DuiColor>(0xFF888888)
/** light gray DuiColor value */
#define DUI_ColorLTGRAY      static_cast<DuiColor>(0xFFCCCCCC)
/** white DuiColor value */
#define DUI_ColorWHITE       static_cast<DuiColor>(0xFFFFFFFF)

/** red DuiColor value */
#define DUI_ColorRED         static_cast<DuiColor>(0xFFFF0000)
/** green DuiColor value */
#define DUI_ColorGREEN       static_cast<DuiColor>(0xFF00FF00)
/** blue DuiColor value */
#define DUI_ColorBLUE        static_cast<DuiColor>(0xFF0000FF)
/** yellow DuiColor value */
#define DUI_ColorYELLOW      static_cast<DuiColor>(0xFFFFFF00)
/** cyan DuiColor value */
#define DUI_ColorCYAN        static_cast<DuiColor>(0xFF00FFFF)
/** magenta DuiColor value */
#define DUI_ColorMAGENTA     static_cast<DuiColor>(0xFFFF00FF)

#endif//DUI_CORE_COLOR_H_
