// 1.13c D2Common+0x01F29 (_sprintf): bounded adapter for the retail call sites.
// Retail's full MSVC2003 _output (+0x3985) is NOT the host formatter. These
// literal formats are all calls in the nine callers listed in the spec; %d is
// also admitted for the existing FUNCTION probe. They use only 32-bit int and
// narrow strings, whose width/precision rules agree with the host CRT.
#include <emscripten.h>
#include <cstdarg>
#include <cstdio>
#include <cstring>

extern "C" EMSCRIPTEN_KEEPALIVE int d2common_sprintf_1f29(char* dest, const char* format, ...) {
    static constexpr const char* formats[] = {
        "%d", "object %d", "item %d", "monster %d", "player %d", "missile %d",
        "Game:%02d\tFrame:%d\tClient:%02d\t",
        "%02d:%02d:%02d.%03d\t", "%01d\t%01d\t%01d\t%02d\t%.50s\t%.5d\t"
    };
    // ponytail: enforce the audited call-site ceiling before varargs reach the
    // host. Expand only with a caller audit or the retail _output implementation.
    bool supported = false;
    if (format && dest) {
        for (const char* known : formats) supported |= std::strcmp(format, known) == 0;
    }
    // This is an adapter error outside its contract, not retail error behavior.
    if (!supported) return -1;
    va_list args;
    va_start(args, format);
    const int count = std::vsprintf(dest, format, args);
    va_end(args);
    return count;
}
