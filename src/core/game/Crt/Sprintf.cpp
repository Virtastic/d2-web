// D2Common's statically linked VS2003 _sprintf. Prior art used: none in the 1.10f source (it does not port the bundled CRT);
// the shape is the same as Storm's _vsprintf port (src/core/crt/storm_vprintf.cpp) and D2CMP+0x2901.
// A string stream {dest, INT32_MAX, dest, 0x42} is formatted by _output (D2Common+0x3985), then the count drops by
// one and a NUL is stored (__flsbuf when the stream is exhausted). wasm64 has no 32-bit stack argument block: the
// variadic arguments come from the caller's va_list. The float formatters are not ported (no caller formats a float).
#include <emscripten.h>
#include <bit>
#include <cerrno>
#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include "core/d2common/output_3985.h"

namespace {
struct VaSource { va_list ap; };
uint32_t vaU32(void* c) { return va_arg(static_cast<VaSource*>(c)->ap, uint32_t); }
uint64_t vaU64(void* c) { return va_arg(static_cast<VaSource*>(c)->ap, uint64_t); }
const void* vaPtr(void* c) { return va_arg(static_cast<VaSource*>(c)->ap, const void*); }
// __flsbuf on a string stream (_IOSTRG): _IOERR, EOF.
int stringFlush(int, D2CommonStringStream2814* s) { s->flags |= 0x20; return -1; }
// _wctomb in the "C" locale: one byte, or EILSEQ above 0xFF.
int cWctomb(char* dst, char16_t wc) {
    if (wc > 0xff) { errno = EILSEQ; return -1; }
    *dst = static_cast<char>(wc);
    return 1;
}
int cIsLeadByte(int) { return 0; }
void noCfltcvt(const uint64_t*, char* buf, int, int, int) { buf[0] = '\0'; }
void noFix(char*) {}
const D2CommonOutputCalls3985 kCalls{stringFlush, cWctomb, cIsLeadByte, std::malloc, std::free,
                                     noCfltcvt, noFix, noFix};
}

// 1.13c D2Common+0x1F29
extern "C" EMSCRIPTEN_KEEPALIVE int d2common_sprintf_1f29(char* dest, const char* format, ...) {
    D2CommonStringStream2814 s{dest, INT32_MAX, dest, 0x42};
    VaSource src;
    va_start(src.ap, format);
    const D2CommonOutputArgs3985 args{&src, vaU32, vaU64, vaPtr};
    const int r = D2Common_3985_Output(&s, format, &args, &kCalls);
    va_end(src.ap);
    if (dest) {
        s.remaining = std::bit_cast<int32_t>(uint32_t(s.remaining) - 1u);
        if (s.remaining < 0) stringFlush(0, &s);
        else *s.cursor = '\0';
    }
    return r;
}
