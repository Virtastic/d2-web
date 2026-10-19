// src/core/d2gfx/d2gfx_setmbcp_nolock_558c.cpp -- one UNMAPPED 1.13c D2Gfx.dll row reimplemented from the
// cached decompile build-wasm/ghidra-d2gfx/decomp/0x558C.c. Alpha lane: implementation + marker only, no
// spec, no vector. This is MSVC 2003 (VC7.1) Release CRT `__setmbcp_nolock`, the code-page selector the
// D2Gfx static initialisers reach through 0x5720 __setmbcp. The same CRT body is already ported for
// D2DDraw at src/core/gfx/d2ddraw_batch_3e87.cpp 0x407A; the five-record static table and the four
// category flags are the CRT object's own constants and were recovered there. Callees 0x05399 (code
// page -> locale id), 0x053C8 setSBCS and 0x053F1 (table derivation) are UNMAPPED rows of this DLL, so
// they are host seams after the retail arguments, as in d2gfx_batch_535e.cpp.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

namespace {

// Retail module state, named at its RVA (base 0x6FA80000).
uint32_t g_codepage;        // +0x09D7C4 DAT_6fa9d7c4: selected code page
uint32_t g_mb_cur_max;      // +0x09D6B4 DAT_6fa9d6b4: multi-byte flag (1 for a DBCS table)
uint32_t g_locale_id;       // +0x09D6AC DAT_6fa9d6ac: locale id for GetStringType/LCMapString
uint32_t g_cp_extra[3];     // +0x09D7D0/D4/D8: record dwords 1..3
uint8_t  g_mbctype[0x101];  // +0x09D6C0 DAT_6fa9d6c0: per-byte flags, index = char + 1
uint32_t g_setmbcp_flag;    // +0x0910DC DAT_6fa910dc: caller asked for a pseudo code page

// +0x090888: four lead-byte category flags. +0x090890: five 0x30-byte code-page records:
// +0 code page, +4/+8/+0xC three words, +0x10..+0x2F four 8-byte groups of [lo,hi] byte pairs.
const uint8_t g_mbcs_lead_flags[4] = { 0x01, 0x02, 0x04, 0x08 };
const uint8_t g_mbcs_table[5][0x30] = {
    { 0xA4,0x03,0x00,0x00, 0x60,0x82,0x79,0x82, 0x21,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
      0xA6,0xDF,0x00,0x00, 0x00,0x00,0x00,0x00, 0xA1,0xA5,0x00,0x00, 0x00,0x00,0x00,0x00,
      0x81,0x9F,0xE0,0xFC, 0x00,0x00,0x00,0x00, 0x40,0x7E,0x80,0xFC, 0x00,0x00,0x00,0x00 },
    { 0xA8,0x03,0x00,0x00, 0xC1,0xA3,0xDA,0xA3, 0x20,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
      0x81,0xFE,0x00,0x00, 0x00,0x00,0x00,0x00, 0x40,0xFE,0x00,0x00, 0x00,0x00,0x00,0x00 },
    { 0xB5,0x03,0x00,0x00, 0xC1,0xA3,0xDA,0xA3, 0x20,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
      0x81,0xFE,0x00,0x00, 0x00,0x00,0x00,0x00, 0x41,0xFE,0x00,0x00, 0x00,0x00,0x00,0x00 },
    { 0xB6,0x03,0x00,0x00, 0xCF,0xA2,0xE4,0xA2, 0x1A,0x00,0xE5,0xA2, 0xE8,0xA2,0x5B,0x00,
      0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
      0x81,0xFE,0x00,0x00, 0x00,0x00,0x00,0x00, 0x40,0x7E,0xA1,0xFE, 0x00,0x00,0x00,0x00 },
    { 0x51,0x05,0x00,0x00, 0x51,0xDA,0x5E,0xDA, 0x20,0x00,0x5F,0xDA, 0x6A,0xDA,0x32,0x00,
      0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
      0x81,0xD3,0xD8,0xDE, 0xE0,0xF9,0x00,0x00, 0x31,0x7E,0x81,0xFE, 0x00,0x00,0x00,0x00 },
};

// Windows _cpinfo as GetCPInfo fills it.
struct D2GfxCpInfo {
    uint32_t MaxCharSize;     // +0x00
    uint8_t  DefaultChar[2];  // +0x04
    uint8_t  LeadByte[12];    // +0x06
};
static_assert(sizeof(D2GfxCpInfo) == 0x14, "1.13c _cpinfo layout");

uint32_t ld32(const uint8_t* p) {
    uint32_t v;
    std::memcpy(&v, p, sizeof v);
    return v;
}

} // namespace

// Seams: 0x05399 (code page -> locale id), 0x053C8 setSBCS, 0x053F1 (derive __mbctype/case tables).
struct D2GfxSetmbcpEnv {
    int32_t  (*get_cp_info)(uint32_t codepage, D2GfxCpInfo* info);
    uint32_t (*locale_id)(uint32_t codepage);                        // 0x05399
    void     (*set_sbcs)(void);                                      // 0x053C8
    void     (*derive_tables)(const D2GfxSetmbcpEnv* env);           // 0x053F1
};

// 1.13c D2Gfx+0x0558C (404 B, caller 0x05720 __setmbcp): `__setmbcp_nolock`. Select `codepage`.
// A record in the five-entry table clears __mbctype, flags the ranges of each of its four lead-byte
// groups with that group's category byte, sets the code page and the multi-byte flag, resolves the
// locale id and copies the record's three words. Otherwise GetCPInfo decides: failure returns -1
// unless the caller asked for a pseudo code page (0x0910DC); MaxCharSize < 2 sets the single-byte
// flag, a DBCS code page sets lead-byte 0x04 over every LeadByte range and 0x08 over bytes
// 1..0xFE, sets the locale id and the multi-byte flag. Code page 0, a pseudo code page, or a
// failed GetCPInfo selects setSBCS. Every changing path ends in the table derivation 0x053F1 and
// returns 0.
// 1.13c D2Gfx+0x0558C
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2gfx_sub_558C(uint32_t codepage, const D2GfxSetmbcpEnv* env) {
    if (codepage != 0) {
        int32_t idx = -1;
        for (int32_t i = 0; i < 5; ++i) {
            if (ld32(g_mbcs_table[i]) == codepage) { idx = i; break; }
        }
        if (idx >= 0) {
            std::memset(g_mbctype, 0, sizeof g_mbctype);
            uint32_t group = 0;
            const uint8_t* ent = g_mbcs_table[idx] + 0x10;
            do {
                uint8_t lo = ent[0];
                const uint8_t* p = ent;
                while (lo != 0 && p[1] != 0) {
                    uint8_t hi = p[1];
                    if (lo <= hi) {
                        uint8_t flag = g_mbcs_lead_flags[group];
                        for (uint32_t u = lo; u <= hi; ++u) g_mbctype[u + 1] |= flag;
                    }
                    p += 2;
                    lo = *p;
                }
                ++group;
                ent += 8;
            } while (group < 4);
            g_codepage = codepage;
            g_mb_cur_max = 1;
            g_locale_id = env->locale_id(codepage);
            std::memcpy(g_cp_extra, g_mbcs_table[idx] + 4, sizeof g_cp_extra);
            goto tail;
        }

        D2GfxCpInfo info;
        if (env->get_cp_info(codepage, &info) == 1) {
            std::memset(g_mbctype, 0, sizeof g_mbctype);
            g_codepage = codepage;
            g_locale_id = 0;
            if (info.MaxCharSize < 2) {
                g_mb_cur_max = 0;
            } else {
                if (info.LeadByte[0] != 0) {
                    const uint8_t* p = info.LeadByte + 1;
                    for (;;) {
                        uint8_t hi = *p;
                        if (hi == 0) break;
                        for (uint32_t u = p[-1]; u <= hi; ++u) g_mbctype[u + 1] |= 4;
                        uint8_t next = p[1];
                        p += 2;
                        if (next == 0) break;
                    }
                }
                for (uint32_t u = 1; u < 0xFF; ++u) g_mbctype[u + 1] |= 8;
                g_locale_id = env->locale_id(codepage);
                g_mb_cur_max = 1;
            }
            g_cp_extra[0] = g_cp_extra[1] = g_cp_extra[2] = 0;
            goto tail;
        }
        if (g_setmbcp_flag == 0) return -1;
    }
    env->set_sbcs();
tail:
    env->derive_tables(env);
    return 0;
}
