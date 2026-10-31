// 1.13c D2Client+0x2C650 -- formats a damage-range quad into the ASCII text the tooltip converts to Unicode.
#include "client_host.h"
#include <emscripten.h>

// Register args in retail: ESI, EDX, ECX, EAX are the four values (wsprintfA receives them in that order);
// stack: (Unicode* dst, int plus). Format "%d.%d-%d.%d", or "+%d.%d-%d.%d" when plus != 0 (the strings at 0x6FB869F4 / 0x6FB86A00).
// wsprintfA writes into a 32-byte stack buffer and does not bound itself; the text is cut at 31 here.
// Then Unicode::toUnicode(dst, buf, 0x20) via the import slot 0x6FB7EAC4 (rva 0xCEAC4).
static int put_int(char* o, int n, int32_t v) {
    char t[12]; int k = 0; uint32_t u = v < 0 ? 0u - (uint32_t)v : (uint32_t)v;
    do { t[k++] = (char)('0' + u % 10); u /= 10; } while (u);
    if (v < 0) t[k++] = '-';
    while (k) { if (n < 31) o[n] = t[--k]; else --k; ++n; }
    return n;
}
extern "C" EMSCRIPTEN_KEEPALIVE int d2_client_format_range_text(char* out32, uint32_t plus, int32_t a, int32_t b, int32_t c, int32_t d) {
    int n = 0;
    auto lit = [&](char ch) { if (n < 31) out32[n] = ch; ++n; };
    if (plus) lit('+');
    n = put_int(out32, n, a); lit('.'); n = put_int(out32, n, b); lit('-'); n = put_int(out32, n, c); lit('.'); n = put_int(out32, n, d);
    if (n > 31) n = 31;
    out32[n] = 0;
    return n;
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_2c650(D2cHost h, char* buf32, uint32_t bufTok, uint32_t dstTok, uint32_t plus, int32_t esi, int32_t edx, int32_t ecx, int32_t eax) {
    d2_client_format_range_text(buf32, plus, esi, edx, ecx, eax);
    D2cCall c = { 0xCEAC4, 0, dstTok, bufTok, 0, 0, { 0x20, 0, 0, 0 } };
    h(&c);
}
