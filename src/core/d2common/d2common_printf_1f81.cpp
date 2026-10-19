#include <cstdarg>
#include <cstdio>

// 1.13c D2Common+0x01F81
// Narrow printf wrapper, despite Ghidra's conflicting _wprintf library label. The formatter at
// +0x3985 consumes individual format bytes. Host stdio owns the stdout lock, buffering and cleanup;
// its FILE layout must never be substituted for the retail 32-byte FILE at DLL+0x99328.
extern "C" int D2Common_1F81_Printf(const char* format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    const int result = std::vprintf(format, arguments);
    va_end(arguments);
    return result;
}
