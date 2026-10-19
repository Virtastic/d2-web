#include "vsprintf_2814.h"
#include <bit>

// Prior art used: D2MOO's D2Common sources do not port the bundled CRT formatter.
// The cached 1.13c library identification and body establish this wrapper; the
// distinct _output and __flsbuf implementations are explicit callee seams.
// 1.13c D2Common+0x02814
extern "C" int D2Common_2814_Vsprintf(char* destination, const char* format,
                                      va_list arguments, const D2CommonVsprintf2814Calls* calls)
{
    D2CommonStringStream2814 stream{destination, INT32_MAX, destination, 0x42};
    const int result = calls->output(&stream, format, arguments);
    if (destination) {
        // Retail decrements the signed 32-bit count with x86 wrap semantics.
        stream.remaining = std::bit_cast<int32_t>(uint32_t(stream.remaining) - 1u);
        if (stream.remaining < 0)
            calls->flushByte(0, &stream);
        else
            *stream.cursor = '\0';
    }
    return result;
}
