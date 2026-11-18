// 1.13c D2Multi+0xFC90: stdcall (const char* utf8, int colour). Decodes the text into a local 576-character Unicode buffer, strips colour tokens with 0xEB70,
// and when DAT_6fa0a0c8 is set hands the buffer to Ordinal_10000(colour, 0, 1) with EDX = the buffer. Returns 1.
#include "d2multi_env.h"
#include "d2multi_ucs.h"

uint32_t d2multi_fc90(const D2mEnv& e, const uint8_t* s, uint32_t colour, uint32_t dat0a0c8) {
    uint16_t buf[576];
    int n = 0;
    while (*s && n < 575) {
        uint32_t c = *s++;
        if (c >= 0xe0 && (s[0] & 0xc0) == 0x80 && (s[1] & 0xc0) == 0x80) { c = ((c & 0xf) << 12) | ((s[0] & 0x3f) << 6) | (s[1] & 0x3f); s += 2; }
        else if (c >= 0xc0 && (s[0] & 0xc0) == 0x80) { c = ((c & 0x1f) << 6) | (s[0] & 0x3f); s++; }
        buf[n++] = (uint16_t)c;
    }
    buf[n] = 0;
    d2m_call(e, 0xeb70, 0, 0, 0, 0, 0, d2m_ptr(buf));
    if (dat0a0c8) d2m_call(e, 0x7ce0, 0, 0, d2m_ptr(buf), 0, 0, colour, 0, 1);
    return 1;
}
