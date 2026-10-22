// src/core/realm/bnclient_batch_6348.cpp -- 1.13c BNClient.dll row 0x6348 (alpha lane, Stage A: no spec,
// no vector), from build-wasm/ghidra-bnclient/decomp/0x6348.c. MSVC CRT _setSBUpLow: builds the
// single-byte ctype (_pctype+1) and upper/lower-case (_pclmap) tables for the active code page.
#include <cstdint>
#include <emscripten.h>

struct BnSetSbUpLowEnv {
    // GetCPInfo(cp): 1 on success; leadbytes = up to 12 bytes of (lo,hi) pairs, zero-terminated.
    int32_t (*get_cp_info)(uint32_t cp, uint8_t leadbytes[12]);
    // __crtGetStringTypeA(chars[256]) -> types[256] (C1 flags: 1 upper, 2 lower)
    void (*get_string_type)(const char* chars, uint16_t* types);
    // __crtLCMapStringA(flags 0x100 = lower, 0x200 = upper) over 256 bytes
    void (*lc_map)(uint32_t flags, const char* in, char* out);
};

// 1.13c BNClient+0x06348 (411 B, caller 0x64E3). ctype[] is the 257-byte table at 0x3F980 (index i+1
// is char i), lmap[] the 256-byte table at 0x3FAA0. Callers pass them as ctype/lmap.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_setsbuplow(uint32_t cp, uint8_t* ctype, uint8_t* lmap,
                                        const BnSetSbUpLowEnv* env) {
    uint8_t lead[12] = {0};
    if (env->get_cp_info(cp, lead) == 1) {
        char chars[256];
        for (int i = 0; i < 256; i++) chars[i] = (char)i;
        chars[0] = ' ';
        for (int k = 0; k < 12 && lead[k]; k += 2)
            for (uint32_t c = lead[k]; c <= lead[k + 1]; c++) chars[c] = ' ';
        uint16_t types[256];
        char lower[256], upper[256];
        env->get_string_type(chars, types);
        env->lc_map(0x100, chars, lower);
        env->lc_map(0x200, chars, upper);
        for (int i = 0; i < 256; i++) {
            if (types[i] & 1)      { ctype[i + 1] |= 0x10; lmap[i] = (uint8_t)lower[i]; }
            else if (types[i] & 2) { ctype[i + 1] |= 0x20; lmap[i] = (uint8_t)upper[i]; }
            else lmap[i] = 0;
        }
    } else {
        for (int i = 0; i < 256; i++) {
            if (i >= 0x41 && i <= 0x5a)      { ctype[i + 1] |= 0x10; lmap[i] = (uint8_t)(i + 0x20); }
            else if (i >= 0x61 && i <= 0x7a) { ctype[i + 1] |= 0x20; lmap[i] = (uint8_t)(i - 0x20); }
            else lmap[i] = 0;
        }
    }
}
