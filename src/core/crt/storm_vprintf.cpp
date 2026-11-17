// Storm's statically linked VS2003 CRT formatters: _vsprintf (Storm+0x553B) and __vsnprintf
// (Storm+0x5801), the two SStrPrintf/SStrVPrintf (Storm+0x3BA80/0x3BA00) call. Each builds a string
// stream {dest, count, dest, _IOWRT|_IOSTRG}, formats through _output (Storm+0x7B77, the same 1946-byte
// VS2003 routine as D2Common+0x3985, whose port runs here), then stores the terminator or calls
// __flsbuf when the stream is exhausted. Returns _output's count (-1 after an overflow).
// wasm64 has no 32-bit stack argument block: the arguments come from the caller's va_list.
#include <bit>
#include <cerrno>
#include <climits>
#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include "core/d2common/output_3985.h"

namespace {
struct VaSource { va_list ap; };
uint32_t vaU32(void* c) { return va_arg(static_cast<VaSource*>(c)->ap, uint32_t); }
uint64_t vaU64(void* c) { return va_arg(static_cast<VaSource*>(c)->ap, uint64_t); }
const void* vaPtr(void* c) { return va_arg(static_cast<VaSource*>(c)->ap, const void*); }
// Storm+0x79D0 __flsbuf on a string stream (_IOSTRG): _IOERR, EOF.
int stringFlush(int, D2CommonStringStream2814* s) { s->flags |= 0x20; return -1; }
// _wctomb in the "C" locale (Storm+0xCAFC -> 0xCA9C, LC_CTYPE handle 0): one byte, or EILSEQ above 0xFF.
int cWctomb(char* dst, char16_t wc) {
    if (wc > 0xff) { errno = EILSEQ; return -1; }
    *dst = static_cast<char>(wc);
    return 1;
}
int cIsLeadByte(int) { return 0; }   // "C" locale: no MBCS lead bytes
// _fptrap slots (_cfltcvt, _forcdecpt, _cropzeros): no ported Storm float formatter yet.
void noCfltcvt(const uint64_t*, char* buf, int, int, int) { buf[0] = '\0'; }
void noFix(char*) {}
const D2CommonOutputCalls3985 kCalls{stringFlush, cWctomb, cIsLeadByte, std::malloc, std::free,
                                     noCfltcvt, noFix, noFix};

int stormStringFormat(char* dest, int32_t count, const char* fmt, va_list ap) {
    D2CommonStringStream2814 s{dest, count, dest, 0x42};
    VaSource src;
    va_copy(src.ap, ap);
    const D2CommonOutputArgs3985 args{&src, vaU32, vaU64, vaPtr};
    const int r = D2Common_3985_Output(&s, fmt, &args, &kCalls);
    va_end(src.ap);
    if (dest) {
        s.remaining = std::bit_cast<int32_t>(uint32_t(s.remaining) - 1u);
        if (s.remaining < 0) stringFlush(0, &s);
        else *s.cursor = '\0';
    }
    return r;
}
}

// 1.13c Storm+0x0553B
extern "C" int storm__vsprintf(char* dest, const char* fmt, va_list ap) {
    return stormStringFormat(dest, INT32_MAX, fmt, ap);
}

// 1.13c Storm+0x05801
extern "C" int storm___vsnprintf(char* dest, size_t count, const char* fmt, va_list ap) {
    return stormStringFormat(dest, static_cast<int32_t>(count), fmt, ap);
}
