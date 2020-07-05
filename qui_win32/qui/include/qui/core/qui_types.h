#ifndef QUI_CORE_TYPES_H_
#define QUI_CORE_TYPES_H_

#include "qui/core/qui_export.h"

/** Called internally if we hit an unrecoverable error.
The platform implementation must not return, but should either throw
an exception or otherwise exit.
*/
QUI_EXPORT extern void qui_abort_no_print(void);

#ifndef QuiDebugf
QUI_EXPORT void QuiDebugf(const char format[], ...);
#endif

/**
* The clang static analyzer likes to know that when the program is not
* expected to continue (crash, assertion failure, etc). It will notice that
* some combination of parameters lead to a function call that does not return.
* It can then make appropriate assumptions about the parameters in code
* executed only if the non-returning function was *not* called.
*/
#if !defined(QuiNO_RETURN_HINT)
#define QuiNO_RETURN_HINT() do {} while (false)
#endif

#if defined(WIN32)
// permits visual studio to follow error back to source
#define QUI_DUMP_LINE_FORMAT(message) \
    QuiDebugf("%s(%d): fatal error: \"%s\"\n", __FILE__, __LINE__, message)
#endif

#ifndef QUI_ABORT
#  define QUI_ABORT(message) \
    do { \
       QuiNO_RETURN_HINT(); \
       QUI_DUMP_LINE_FORMAT(message); \
       qui_abort_no_print(); \
    } while (false)
#endif

#define QuiASSERT_RELEASE(cond) \
        static_cast<void>( (cond) ? (void)0 : []{ QUI_ABORT("assert(" #cond ")"); }() )

#ifdef QUI_DEBUG
#define QuiASSERT(cond) QuiASSERT_RELEASE(cond)
#define QuiASSERTF(cond, fmt, ...) static_cast<void>( (cond) ? (void)0 : [&]{ \
                                          QuiDebugf(fmt"\n", __VA_ARGS__);        \
                                          QUI_ABORT("assert(" #cond ")");         \
                                      }() )
#define QuiDEBUGFAIL(message)        QUI_ABORT(message)
#define QuiDEBUGFAILF(fmt, ...)      QuiASSERTF(false, fmt, ##__VA_ARGS__)
#define QuiDEBUGCODE(...)            __VA_ARGS__
#define QuiDEBUGF(args       )       QuiDebugf args
#define QuiAssertResult(cond)        QuiASSERT(cond)
#else
#define QuiASSERT(cond)            static_cast<void>(0)
#define QuiASSERTF(cond, fmt, ...) static_cast<void>(0)
#define QuiDEBUGFAIL(message)
#define QuiDEBUGFAILF(fmt, ...)
#define QuiDEBUGCODE(...)
#define QuiDEBUGF(args)

// unlike QuiASSERT, this guy executes its condition in the non-debug build.
// The if is present so that this can be used with functions marked QUI_WARN_UNUSED_RESULT.
#define QuiAssertResult(cond)         if (cond) {} do {} while(false)
#endif


///////////////////////////////////////////////////////////////////////

/**
*  Fast type for signed 8 bits. Use for parameter passing and local variables,
*  not for storage.
*/
typedef int QUIS8CPU;

/**
*  Fast type for unsigned 8 bits. Use for parameter passing and local
*  variables, not for storage
*/
typedef unsigned QUIU8CPU;

/**
*  Fast type for signed 16 bits. Use for parameter passing and local variables,
*  not for storage
*/
typedef int QUIS16CPU;

/**
*  Fast type for unsigned 16 bits. Use for parameter passing and local
*  variables, not for storage
*/
typedef unsigned QUIU16CPU;

/**
*  Meant to be a small version of bool, for storage purposes. Will be 0 or 1
*/
typedef uint8_t QUISkBool8;


#endif   // QUI_CORE_TYPES_H_
