// 1.13c D2Win+0x120C0 (caller 0x12410): width of up to n UTF-16 chars.
// LF (0x0A) is skipped; a 0xFF 'c' [0-6] colour code (3 chars) is skipped while more than 3 chars remain.
// Every other char adds the advance byte (+3) of the glyph record the getter at DAT_6F901508 (RVA 0x21508) returns.
// __fastcall: ECX = text, EDX = n. Implementation only; the getter is a seam in D2Win120C0Env.
#include <cstdint>

struct D2Win120C0Env {
    const uint8_t* (*glyph)(uint32_t ch);   // the DAT_6F901508 getter; glyph(ch)[3] is the advance
};

extern "C" int32_t d2win_120c0(const uint16_t* text, int32_t n, const D2Win120C0Env* env) {
    int32_t i = 0, w = 0, lim = 3;
    const uint16_t* s = text;
    while (*s) {
        if (n <= i) return w;
        uint16_t ch = *s;
        if (ch == 10) { ++s; ++i; ++lim; continue; }
        if (n > lim && ch == 0xff && s[1] == 'c' && s[2] >= 0x30 && s[2] <= 0x36) { s += 3; i += 3; lim += 3; continue; }
        w += env->glyph(ch)[3];
        ++s; ++i; ++lim;
    }
    return w;
}
