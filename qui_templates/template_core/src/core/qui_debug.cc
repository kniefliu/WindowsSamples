#include "qui/core/qui_types.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <Windows.h>

void qui_abort_no_print(void)
{
    __debugbreak();
    abort();
}

static const size_t kBufferSize = 2048;

void QuiDebugf(const char format[], ...)
{
    char    buffer[kBufferSize + 1];
    va_list args;

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fflush(stderr);  // stderr seems to be buffered on Windows.

    va_start(args, format);
    vsnprintf(buffer, kBufferSize, format, args);
    va_end(args);

    OutputDebugStringA(buffer);
}
