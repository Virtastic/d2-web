#include <cstdint>
#include <cstring>

// 1.13c D2Common+0x06DB8 (411 B, caller 0x6F53) CRT single-byte ctype/case-map init (setSBUpLow).
// GetCPInfo(codepage DAT 0x6fdf15e4) ok: bytes 0..255 (byte 0 -> ' '), lead-byte ranges blanked to ' ',
// then ___crtGetStringTypeA (0x64D3, CT_CTYPE1) and two LCMapStringA (0x4EF4, 0x100 lower / 0x200 upper)
// calls; per byte: bit0 (upper) -> ctype |= 0x10, map = lower-mapped; else bit1 (lower) -> ctype |= 0x20,
// map = upper-mapped; else map = 0. ctype is DAT 0x6fdf14e0+1+i, map is DAT 0x6fdf1600. GetCPInfo failure:
// ASCII fallback (A-Z -> 0x10, +0x20; a-z -> 0x20, -0x20; else 0). Win32/CRT calls are host seams.
struct D2CommonCtypeInitEnv {
    int32_t (*getCPInfo)(uint32_t cp, uint8_t leadByte[12]);                 // MaxCharSize/DefaultChar skipped; LeadByte[12]
    void (*getStringTypeA)(const char* src, uint16_t* out);                  // 0x64D3, 256 bytes
    void (*lcMapStringA)(uint32_t flags, const char* src, char* dst);        // 0x4EF4, 256 bytes
    uint32_t codePage;                                                       // DAT 0x6fdf15e4
};

extern "C" void d2common_crt_ctype_init_6db8(uint8_t* ctype /*0x6fdf14e0+1, 256*/, uint8_t* map /*0x6fdf1600*/,
                                             const D2CommonCtypeInitEnv* env) {
    uint8_t lead[12];
    if (env->getCPInfo(env->codePage, lead) == 1) {
        char s[256]; uint16_t t[256]; char lo[256], up[256];
        for (uint32_t i = 0; i < 256; i++) s[i] = (char)i;
        s[0] = ' ';
        for (const uint8_t* p = lead; p[0] != 0 && p + 1 < lead + 12; p += 2)
            for (uint32_t c = p[0]; c <= p[1]; c++) s[c] = ' ';
        env->getStringTypeA(s, t);
        env->lcMapStringA(0x100, s, lo);
        env->lcMapStringA(0x200, s, up);
        for (uint32_t i = 0; i < 256; i++) {
            if (t[i] & 1) { ctype[i] |= 0x10; map[i] = (uint8_t)lo[i]; }
            else if (t[i] & 2) { ctype[i] |= 0x20; map[i] = (uint8_t)up[i]; }
            else map[i] = 0;
        }
    } else {
        for (uint32_t i = 0; i < 256; i++) {
            if (i >= 0x41 && i <= 0x5a) { ctype[i] |= 0x10; map[i] = (uint8_t)(i + 0x20); }
            else if (i >= 0x61 && i <= 0x7a) { ctype[i] |= 0x20; map[i] = (uint8_t)(i - 0x20); }
            else map[i] = 0;
        }
    }
}
