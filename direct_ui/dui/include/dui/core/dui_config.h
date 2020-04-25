#ifndef DUI_CORE_CONFIG_H_
#define DUI_CORE_CONFIG_H_


#if !defined(DUI_BUILD_FOR_ANDROID) && !defined(DUI_BUILD_FOR_IOS) && !defined(DUI_BUILD_FOR_WIN32) && !defined(DUI_BUILD_FOR_UNIX) && !defined(DUI_BUILD_FOR_MAC)
#ifdef __APPLE__
#include "TargetConditionals.h"
#endif
#if defined(_WIN32) || defined(__SYMBIAN32__)
#define DUI_BUILD_FOR_WIN32
#elif defined(ANDROID) || defined(__ANDROID__)
#define DUI_BUILD_FOR_ANDROID
#elif defined(linux) || defined(__linux) || defined(__FreeBSD__) || \
      defined(__OpenBSD__) || defined(__sun) || defined(__NetBSD__) || \
      defined(__DragonFly__) || defined(__Fuchsia__) || \
      defined(__GLIBC__) || defined(__GNU__) || defined(__unix__)
#define DUI_BUILD_FOR_UNIX
#elif TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
#define DUI_BUILD_FOR_IOS
#else
#define DUI_BUILD_FOR_MAC
#endif
#endif


//////////////////////////////////////////////////////////////////////

#if !defined(DUI_CPU_BENDIAN) && !defined(DUI_CPU_LENDIAN)
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define DUI_CPU_BENDIAN
#elif defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define DUI_CPU_LENDIAN
#elif defined(__sparc) || defined(__sparc__) || \
  defined(_POWER) || defined(__powerpc__) || \
  defined(__ppc__) || defined(__hppa) || \
  defined(__PPC__) || defined(__PPC64__) || \
  defined(_MIPSEB) || defined(__ARMEB__) || \
  defined(__s390__) || \
  (defined(__sh__) && defined(__BIG_ENDIAN__)) || \
  (defined(__ia64) && defined(__BIG_ENDIAN__))
     #define DUI_CPU_BENDIAN
#else
#define DUI_CPU_LENDIAN
#endif
#endif

//////////////////////////////////////////////////////////////////////

#if defined(__i386) || defined(_M_IX86) ||  defined(__x86_64__) || defined(_M_X64)
#define DUI_CPU_X86 1
#endif

/**
 *  DUI_CPU_SSE_LEVEL
 *
 *  If defined, DUI_CPU_SSE_LEVEL should be set to the highest supported level.
 *  On non-intel CPU this should be undefined.
 */

#define DUI_CPU_SSE_LEVEL_SSE1     10
#define DUI_CPU_SSE_LEVEL_SSE2     20
#define DUI_CPU_SSE_LEVEL_SSE3     30
#define DUI_CPU_SSE_LEVEL_SSSE3    31
#define DUI_CPU_SSE_LEVEL_SSE41    41
#define DUI_CPU_SSE_LEVEL_SSE42    42
#define DUI_CPU_SSE_LEVEL_AVX      51
#define DUI_CPU_SSE_LEVEL_AVX2     52

// When targetting iOS and using gyp to generate the build files, it is not
// possible to select files to build depending on the architecture (i.e. it
// is not possible to use hand optimized assembly implementation). In that
// configuration DUI_BUILD_NO_OPTS is defined. Remove optimisation then.
#ifdef DUI_BUILD_NO_OPTS
#define DUI_CPU_SSE_LEVEL 0
#endif

// Are we in GCC?
#ifndef DUI_CPU_SSE_LEVEL
// These checks must be done in descending order to ensure we set the highest
// available SSE level.
#if defined(__AVX2__)
#define DUI_CPU_SSE_LEVEL    DUI_CPU_SSE_LEVEL_AVX2
#elif defined(__AVX__)
#define DUI_CPU_SSE_LEVEL    DUI_CPU_SSE_LEVEL_AVX
#elif defined(__SSE4_2__)
#define DUI_CPU_SSE_LEVEL    DUI_CPU_SSE_LEVEL_SSE42
#elif defined(__SSE4_1__)
#define DUI_CPU_SSE_LEVEL    DUI_CPU_SSE_LEVEL_SSE41
#elif defined(__SSSE3__)
#define DUI_CPU_SSE_LEVEL    DUI_CPU_SSE_LEVEL_SSSE3
#elif defined(__SSE3__)
#define DUI_CPU_SSE_LEVEL    DUI_CPU_SSE_LEVEL_SSE3
#elif defined(__SSE2__)
#define DUI_CPU_SSE_LEVEL    DUI_CPU_SSE_LEVEL_SSE2
#endif
#endif

// Are we in VisualStudio?
#ifndef DUI_CPU_SSE_LEVEL
// These checks must be done in descending order to ensure we set the highest
// available SSE level. 64-bit intel guarantees at least SSE2 support.
#if defined(__AVX2__)
#define DUI_CPU_SSE_LEVEL        DUI_CPU_SSE_LEVEL_AVX2
#elif defined(__AVX__)
#define DUI_CPU_SSE_LEVEL        DUI_CPU_SSE_LEVEL_AVX
#elif defined(_M_X64) || defined(_M_AMD64)
#define DUI_CPU_SSE_LEVEL        DUI_CPU_SSE_LEVEL_SSE2
#elif defined(_M_IX86_FP)
#if _M_IX86_FP >= 2
    #define DUI_CPU_SSE_LEVEL    DUI_CPU_SSE_LEVEL_SSE2
#elif _M_IX86_FP == 1
    #define DUI_CPU_SSE_LEVEL    DUI_CPU_SSE_LEVEL_SSE1
#endif
#endif
#endif

//////////////////////////////////////////////////////////////////////
// ARM defines

#if defined(__arm__) && (!defined(__APPLE__) || !TARGET_IPHONE_SIMULATOR)
    #define DUI_CPU_ARM32

    #if defined(__GNUC__)
        #if defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) \
                || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) \
                || defined(__ARM_ARCH_7EM__) || defined(_ARM_ARCH_7)
            #define SK_ARM_ARCH 7
        #elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) \
                || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) \
                || defined(__ARM_ARCH_6ZK__) || defined(__ARM_ARCH_6T2__) \
                || defined(__ARM_ARCH_6M__) || defined(_ARM_ARCH_6)
            #define SK_ARM_ARCH 6
        #elif defined(__ARM_ARCH_5__) || defined(__ARM_ARCH_5T__) \
                || defined(__ARM_ARCH_5E__) || defined(__ARM_ARCH_5TE__) \
                || defined(__ARM_ARCH_5TEJ__) || defined(_ARM_ARCH_5)
            #define SK_ARM_ARCH 5
        #elif defined(__ARM_ARCH_4__) || defined(__ARM_ARCH_4T__) || defined(_ARM_ARCH_4)
            #define SK_ARM_ARCH 4
        #else
            #define SK_ARM_ARCH 3
        #endif
    #endif
#endif

#if defined(__aarch64__) && !defined(DUI_BUILD_NO_OPTS)
    #define DUI_CPU_ARM64
#endif

// All 64-bit ARM chips have NEON.  Many 32-bit ARM chips do too.
#if !defined(SK_ARM_HAS_NEON) && !defined(DUI_BUILD_NO_OPTS) && (defined(__ARM_NEON__) || defined(__ARM_NEON))
    #define SK_ARM_HAS_NEON
#endif

// Really this __APPLE__ check shouldn't be necessary, but it seems that Apple's Clang defines
// __ARM_FEATURE_CRC32 for -arch arm64, even though their chips don't support those instructions!
#if defined(__ARM_FEATURE_CRC32) && !defined(__APPLE__)
    #define SK_ARM_HAS_CRC32
#endif

//////////////////////////////////////////////////////////////////////

#if !defined(DUI_IMPLEMENTATION)
#define DUI_IMPLEMENTATION 0
#endif

#if !defined(DUI_API)
#if defined(DUI_DLL)
#if defined(_MSC_VER)
#if DUI_IMPLEMENTATION
#define DUI_API __declspec(dllexport)
#else
#define DUI_API __declspec(dllimport)
#endif
#else
#define DUI_API __attribute__((visibility("default")))
#endif
#else
#define DUI_API
#endif
#endif

#endif  //DUI_CORE_CONFIG_H_
