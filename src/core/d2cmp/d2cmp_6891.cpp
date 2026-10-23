// src/core/d2cmp/d2cmp_6891.cpp -- 1.13c D2CMP+0x6891: statically linked MSVC CRT locale ctype init
// (builds the SBCS ctype flag table and the upper/lower map for the current code page).
#include <cstdint>

struct D2CmpCtypeEnv {
    bool (*getCPInfo)(uint32_t cp, uint8_t leadByte[12]); // GetCPInfo; fills LeadByte[12] (pairs, 0-terminated)
    void (*stringType)(const uint8_t* s, uint16_t* out);  // CT_CTYPE1 over 256 bytes (___crtGetStringTypeA)
    void (*mapString)(uint32_t flags, const uint8_t* s, uint8_t* out); // LCMapString 0x100 lower / 0x200 upper, 256 bytes
};

// ctype[0..256] is DAT_6ff13c20 (entry i+1 is byte i; bit 0x10 upper, 0x20 lower), xmap[256] is DAT_6ff13d40.
// 1.13c D2CMP+0x6891
extern "C" void d2cmp_ctype_init(const D2CmpCtypeEnv* env, uint32_t cp, uint8_t* ctype, uint8_t* xmap) {
    uint8_t lead[12];
    if (env->getCPInfo(cp, lead)) {
        uint8_t s[256], lo[256], up[256];
        uint16_t t[256];
        for (int i = 0; i < 256; i++) s[i] = (uint8_t)i;
        s[0] = ' ';
        for (int i = 0; i < 12 && lead[i]; i += 2)
            for (int c = lead[i]; c <= lead[i + 1]; c++) s[c] = ' ';
        env->stringType(s, t);
        env->mapString(0x100, s, lo);
        env->mapString(0x200, s, up);
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
