#ifndef QUI_CORE_COLOR_H_
#define QUI_CORE_COLOR_H_

#include "qui/core/qui_prerequisites.h"

typedef uint8_t QuiAlpha;

typedef uint32_t QuiColor;

static constexpr inline QuiColor QuiColorSetARGB(QUIU8CPU a, QUIU8CPU r, QUIU8CPU g, QUIU8CPU b) {
    return QuiASSERT(a <= 255 && r <= 255 && g <= 255 && b <= 255),
        (a << 24) | (r << 16) | (g << 8) | (b << 0);
}

/** Return a QuiColor value from 8 bit component values, with an implied value
of 0xFF for alpha (fully opaque)
*/
#define QuiColorSetRGB(r, g, b)  QuiColorSetARGB(0xFF, r, g, b)

/** return the alpha byte from a QuiColor value */
#define QuiColorGetA(color)      (((color) >> 24) & 0xFF)
/** return the red byte from a QuiColor value */
#define QuiColorGetR(color)      (((color) >> 16) & 0xFF)
/** return the green byte from a QuiColor value */
#define QuiColorGetG(color)      (((color) >>  8) & 0xFF)
/** return the blue byte from a QuiColor value */
#define QuiColorGetB(color)      (((color) >>  0) & 0xFF)

static constexpr inline QuiColor QuiColorSetA(QuiColor c, QUIU8CPU a) {
    return (c & 0x00FFFFFF) | (a << 24);
}


// common colors

/** transparent QuiAlpha value */
#define QUI_AlphaTRANSPARENT static_cast<QuiAlpha>(0x00)
/** opaque QuiAlpha value */
#define QUI_AlphaOPAQUE      static_cast<QuiAlpha>(0xFF)

/** transparent QuiColor value */
#define QUI_ColorTRANSPARENT static_cast<QuiColor>(0x00000000)

/** black QuiColor value */
#define QUI_ColorBLACK       static_cast<QuiColor>(0xFF000000)
/** dark gray QuiColor value */
#define QUI_ColorDKGRAY      static_cast<QuiColor>(0xFF444444)
/** gray QuiColor value */
#define QUI_ColorGRAY        static_cast<QuiColor>(0xFF888888)
/** light gray QuiColor value */
#define QUI_ColorLTGRAY      static_cast<QuiColor>(0xFFCCCCCC)
/** white QuiColor value */
#define QUI_ColorWHITE       static_cast<QuiColor>(0xFFFFFFFF)

/** red QuiColor value */
#define QUI_ColorRED         static_cast<QuiColor>(0xFFFF0000)
/** green QuiColor value */
#define QUI_ColorGREEN       static_cast<QuiColor>(0xFF00FF00)
/** blue QuiColor value */
#define QUI_ColorBLUE        static_cast<QuiColor>(0xFF0000FF)
/** yellow QuiColor value */
#define QUI_ColorYELLOW      static_cast<QuiColor>(0xFFFFFF00)
/** cyan QuiColor value */
#define QUI_ColorCYAN        static_cast<QuiColor>(0xFF00FFFF)
/** magenta QuiColor value */
#define QUI_ColorMAGENTA     static_cast<QuiColor>(0xFFFF00FF)





#endif   // QUI_CORE_COLOR_H_
