#ifndef QUI_CORE_APPLICATION_H_
#define QUI_CORE_APPLICATION_H_

#include "qui/core/qui_prerequisites.h"

class QUI_EXPORT QuiApplication final {
public:
    static void SetInstance(HINSTANCE instance);
    static HINSTANCE GetInstance();
    static int Run();
};

#endif   // QUI_CORE_APPLICATION_H_
