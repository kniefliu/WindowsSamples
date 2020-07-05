#ifndef QUI_CORE_WINDOW_TYPES_H_
#define QUI_CORE_WINDOW_TYPES_H_

#include "qui/core/qui_prerequisites.h"


// platform related 
typedef void * QuiWindowHandle;
typedef uint32_t QuiWindowStyle;
typedef uint32_t QuiWindowExStyle;
typedef uint32_t QuiWindowClassStyle;
typedef uint32_t QuiWindowMessageID;
typedef void * QuiWindowWParam;
typedef void * QuiWindowLParam;
typedef void * QuiWindowLResult;
typedef uint32_t QuiWindowIconID;
#define QuiCreateWindow_USEDEFAULT       ((int)0x80000000)


#endif   // QUI_CORE_WINDOW_TYPES_H_
