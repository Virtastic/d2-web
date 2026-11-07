// src/core/d2cmp/d2cmp_6891.cpp -- 1.13c D2CMP+0x6891: statically linked MSVC CRT locale ctype init
// (builds the SBCS ctype flag table and the upper/lower map for the current code page).
// Retail reads the code page from 0x103D24 and the locale from 0x103C04; the caller passes both.
#include <cstdint>

struct D2CmpCtypeEnv {
    uint32_t (*getCPInfo)(uint32_t cp, uint8_t info[20]);   // GetCPInfo: BOOL; info = _cpinfo (MaxCharSize, DefaultChar[2], LeadByte[12])
    // ___crtGetStringTypeA (0x7D98, cdecl): (1 = CT_CTYPE1, s, 0x100, out, cp, lcid, 0)
    void (*stringType)(uint32_t type, const uint8_t* s, uint32_t n, uint16_t* out, uint32_t cp, uint32_t lcid, uint32_t z);
    // the LCMapString wrapper 0x7FE0 (cdecl): (lcid, flags, s, 0x100, out, 0x100, cp, 0); flags 0x100 lower, 0x200 upper
    void (*mapString)(uint32_t lcid, uint32_t flags, const uint8_t* s, uint32_t n, uint8_t* out, uint32_t outn, uint32_t cp, uint32_t z);
};

// ctype[0..256] is DAT_6ff13c20 (entry i+1 is byte i; bit 0x10 upper, 0x20 lower), xmap[256] is DAT_6ff13d40.
// 1.13c D2CMP+0x6891
extern "C" void d2cmp_ctype_init(const D2CmpCtypeEnv* env, uint32_t cp, uint32_t lcid, uint8_t* ctype, uint8_t* xmap) {
    uint8_t info[20] = {};
    if (env->getCPInfo(cp, info) == 1) {
        uint8_t s[256], lo[256], up[256];
        uint16_t t[256];
        for (int i = 0; i < 256; i++) s[i] = (uint8_t)i;
        s[0] = ' ';
        const uint8_t* lead = info + 6;
        for (int i = 0; i < 12 && lead[i]; i += 2)
            for (int c = lead[i]; c <= lead[i + 1]; c++) s[c] = ' ';
        env->stringType(1, s, 0x100, t, cp, lcid, 0);
        env->mapString(lcid, 0x100, s, 0x100, lo, 0x100, cp, 0);
        env->mapString(lcid, 0x200, s, 0x100, up, 0x100, cp, 0);
        for (int i = 0; i < 256; i++) {
            if (t[i] & 1) { ctype[i + 1] |= 0x10; xmap[i] = lo[i]; }
            else if (t[i] & 2) { ctype[i + 1] |= 0x20; xmap[i] = up[i]; }
            else xmap[i] = 0;
        }
    } else {
        for (int i = 0; i < 256; i++) {
            if (i >= 0x41 && i <= 0x5a) { ctype[i + 1] |= 0x10; xmap[i] = (uint8_t)(i + 0x20); }
            else if (i >= 0x61 && i <= 0x7a) { ctype[i + 1] |= 0x20; xmap[i] = (uint8_t)(i - 0x20); }
            else xmap[i] = 0;
        }
    }
}
