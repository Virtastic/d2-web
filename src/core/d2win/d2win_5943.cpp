// 1.13c D2Win+0x5943 (CRT _setmbcp worker): build the MBCS ctype table for a code page. Returns 0, or -1 if the
// code page is unknown to both the built-in table and GetCPInfo (and no SBCS fallback is allowed).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

// The wasm compat shim has no GetCPInfo/CPINFO; every code page the game uses is in the built-in table above,
// so an unknown page reports failure (host kernel call unavailable) and takes the SBCS fallback.
struct CPINFO { uint32_t MaxCharSize; uint8_t DefaultChar[2]; uint8_t LeadByte[12]; };
static int GetCPInfo(uint32_t, CPINFO*) { return 0; }

struct CpEnt { uint32_t cp; uint32_t f[3]; uint8_t rng[4][8]; };      // 0x30 B rows at 0x6F8FF998
extern "C" CpEnt d2win_g_cptab[5];                                     // 0x6F8FF998
extern "C" uint8_t d2win_g_cpbits[4];                                  // 0x6F8FF990 {1,1,1,1}-style flag per range row
extern "C" uint8_t d2win_g_mbctype[0x101];                             // 0x6F9AA8E0 (index +1)
extern "C" uint32_t d2win_g_mbcp, d2win_g_mbcinit, d2win_g_mblcid, d2win_g_mbf[3]; // 0x6F9AA9E4/8C0/8B8/9F0..8
extern "C" int32_t d2win_g_sbcsok;                                     // 0x6F901390
extern "C" void d2win_sub_57A8(void);
extern "C" void d2win_setSBCS(void);                                   // 0x577F

// 1.13c D2Win+0x5750: code page -> LCID (Japanese, Chinese-S, Korean, Chinese-T), else 0.
static uint32_t cp_lcid(uint32_t cp) {
    return cp == 0x3a4 ? 0x411 : cp == 0x3a8 ? 0x804 : cp == 0x3b5 ? 0x412 : cp == 0x3b6 ? 0x404 : 0;
}

extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_5943(uint32_t cp) {
    if (cp) {
        for (int i = 0; i < 5; i++) {
            if (d2win_g_cptab[i].cp != cp) continue;
            memset(d2win_g_mbctype, 0, sizeof d2win_g_mbctype);
            for (int r = 0; r < 4; r++)
                for (const uint8_t* p = d2win_g_cptab[i].rng[r]; p[0] && p[1]; p += 2)
                    for (uint32_t c = p[0]; c <= p[1]; c++) d2win_g_mbctype[c + 1] |= d2win_g_cpbits[r];
            d2win_g_mbcp = cp; d2win_g_mbcinit = 1; d2win_g_mblcid = cp_lcid(cp);
            for (int k = 0; k < 3; k++) d2win_g_mbf[k] = d2win_g_cptab[i].f[k];
            d2win_sub_57A8();
            return 0;
        }
        CPINFO ci;
        if (GetCPInfo(cp, &ci) == 1) {
            memset(d2win_g_mbctype, 0, sizeof d2win_g_mbctype);
            d2win_g_mbcp = cp; d2win_g_mblcid = 0;
            if (ci.MaxCharSize < 2) d2win_g_mbcinit = 0;
            else {
                for (const uint8_t* p = ci.LeadByte + 1; p[-1] && *p; p += 2)
                    for (uint32_t c = p[-1]; c <= *p; c++) d2win_g_mbctype[c + 1] |= 4;
                for (uint32_t c = 1; c < 0xff; c++) d2win_g_mbctype[c + 1] |= 8;
                d2win_g_mblcid = cp_lcid(cp); d2win_g_mbcinit = 1;
            }
            d2win_g_mbf[0] = d2win_g_mbf[1] = d2win_g_mbf[2] = 0;
            d2win_sub_57A8();
            return 0;
        }
        if (!d2win_g_sbcsok) return -1;
    }
    d2win_setSBCS();
    d2win_sub_57A8();
    return 0;
}
