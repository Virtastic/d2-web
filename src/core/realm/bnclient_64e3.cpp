// src/core/realm/bnclient_64e3.cpp -- the BNClient CRT _setmbcp worker (VC7.1 `setmbcp`, cdecl(cp), /GS frame).
// Selects the multibyte code page: a built-in row (five 0x30-byte rows: cp, three extra dwords, four 8-byte
// lead-range lists) fills the 257-byte ctype table from the four class flags; otherwise GetCPInfo decides; a zero
// code page, or a failed GetCPInfo while the pseudo-code-page flag is set, resets to SBCS. Every success path
// then rebuilds the case tables (sub 0x6348).
#include <cstdint>
#include <cstring>
#include "core/realm/bnclient_mbcp.h"

namespace d2::bnclient::leaves { std::uint32_t language_for_codepage(std::uint32_t codepage); }

// 1.13c BNClient+0x064E3 (404 B, caller 0x6677). Returns 0 or -1.
extern "C" int32_t bn_setmbcp(uint32_t cp, const BnMbcpGlobals* g, const BnSetMbcpSeams* s) {
    if (cp != 0) {
        for (uint32_t i = 0; i < 5; i++) {
            const uint8_t* row = s->builtin + i * 0x30;
            uint32_t rcp; std::memcpy(&rcp, row, 4);
            if (rcp != cp) continue;
            std::memset(g->ctype, 0, 257);
            for (uint32_t j = 0; j < 4; j++)
                for (const uint8_t* p = row + 0x10 + j * 8; p[0] && p[1]; p += 2)
                    for (uint32_t c = p[0]; c <= p[1]; c++) g->ctype[c + 1] |= s->flags4[j];
            *g->cp = cp;
            *g->mb_flag = 1;
            *g->lang = d2::bnclient::leaves::language_for_codepage(cp);
            std::memcpy(g->extra, row + 4, 12);
            s->setsbuplow();
            return 0;
        }
        uint8_t info[20] = {};  // CPINFO: MaxCharSize, DefaultChar[2], LeadByte[12]
        if (s->get_cp_info(cp, info) == 1) {
            std::memset(g->ctype, 0, 257);
            *g->cp = cp;
            *g->lang = 0;
            uint32_t maxc; std::memcpy(&maxc, info, 4);
            if (maxc < 2) {
                *g->mb_flag = 0;
            } else {
                const uint8_t* lead = info + 6;
                if (lead[0]) {
                    for (const uint8_t* q = lead + 1; q[0]; q += 2) {
                        for (uint32_t c = q[-1]; c <= q[0]; c++) g->ctype[c + 1] |= 4;
                        if (!q[1]) break;
                    }
                }
                for (uint32_t c = 1; c < 0xff; c++) g->ctype[c + 1] |= 8;
                *g->lang = d2::bnclient::leaves::language_for_codepage(cp);
                *g->mb_flag = 1;
            }
            std::memset(g->extra, 0, 12);
            s->setsbuplow();
            return 0;
        }
        if (*g->has_default == 0) return -1;
    }
    s->set_sbcs();
    s->setsbuplow();
    return 0;
}
