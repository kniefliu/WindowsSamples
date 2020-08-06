#ifndef QUI_CORE_EXPORT_H_
#define QUI_CORE_EXPORT_H_

#if defined(COMPONENT_BUILD)
#if defined(WIN32)

#if defined(QUI_IMPLEMENTATION)
#define QUI_EXPORT __declspec(dllexport)
#else
#define QUI_EXPORT __declspec(dllimport)
#endif   // QUI_IMPLEMENTATION

#else  // defined(WIN32)
#if defined(BASE_IMPLEMENTATION)
#define QUI_EXPORT __attribute__((visibility("default")))
#else
#define QUI_EXPORT
#endif  // defined(QUI_IMPLEMENTATION)
#endif

#else  // defined(COMPONENT_BUILD)
#define QUI_EXPORT
#endif

#endif   // QUI_CORE_EXPORT_H_
