// src/core/realm/mcp_setmbcs_305f.cpp -- one UNMAPPED 1.13c D2MCPClient.dll row reimplemented from the
// decompile in build-wasm/ghidra-d2mcpclient/decomp/0x305F.c. Base 0x6FA20000; every global below is
// named at its retail RVA. This is VC7.1 (MSVC 2003 Release) `setMBCS`, the MBCS half of `__setmbcp`
// (0x31FA): it asks the host for the codepage's lead-byte ranges and character classes and rebuilds
// the module's case/ctype tables from the answers. The three kernel32/CRT calls are seams; the two
// tables are module state this file owns (the same retail globals the landed sibling
// src/core/realm/mcp_crt_batch_b56.cpp models with its own statics, and mcp_crt_batch_b58.cpp zeroes
// from setSBCS 0x3036).
#include <cstdint>
#include <emscripten.h>

// Windows _cpinfo as GetCPInfo fills it: MaxCharSize, DefaultChar[2], LeadByte[12].
struct D2McpCpInfo {
    uint32_t max_char_size;   // +0x00
    uint8_t  default_char[2]; // +0x04
    uint8_t  lead_byte[12];   // +0x06
};
static_assert(sizeof(D2McpCpInfo) == 0x14, "1.13c _cpinfo layout");

// +0x32280 DAT_6fa32280: class bytes, indexed from 1 (byte 0 unused), 0x100 entries through +0x32380.
uint8_t d2mcp305f_ctype[0x101];
// +0x323A0 DAT_6fa323a0: 0x100 case-mapped characters.
uint8_t d2mcp305f_charmap[0x100];

// 1.13c D2MCPClient+0x0305F (411 B, caller 0x031FA __setmbcp): `setMBCS`. Read the codepage
// (DAT_6fa32384) and the locale id (DAT_6fa32268, itself codepage-to-locale mapped by 0x03007) from
// module state; they arrive here as arguments because 0x31FA owns the writes. With GetCPInfo saying
// the codepage exists: build the 0x100-byte charset 0,1,..,0xFF, replace index 0 with a space, blank
// every lead-byte range (inclusive pairs from LeadByte[1] onward, an odd start/end pair ignored), ask
// GetStringTypeA for CT_CTYPE1 (1) over it, ask LCMapStringA for LCMAP_LOWERCASE (0x100) and
// LCMAP_UPPERCASE (0x200) over it, then for each byte i: if ctype1[i] has bit 1 (uppercase class) set
// class byte [i+1] |= 0x10 and store the lowercase mapping `lower[i]`; else if bit 2 (lowercase) set
// [i+1] |= 0x20 and store `upper[i]`; else store 0. Class bytes are OR-ed, not assigned, so a caller
// that has already set the 0x04 lead-byte bits keeps them (0x31FA does exactly that). If GetCPInfo
// fails instead, the pure ASCII fallback: 'A'..'Z' get |= 0x10 and i+0x20, 'a'..'z' get |= 0x20 and
// i-0x20, every other byte maps to 0. Returns nothing.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_setmbcs_305f(uint32_t codepage, uint32_t locale, int32_t (*get_cp_info)(uint32_t, D2McpCpInfo*), int32_t (*get_string_type)(int32_t, const char*, int32_t, uint16_t*, uint32_t, uint32_t, int32_t), int32_t (*lcmap_string)(uint32_t, uint32_t, const char*, int32_t, char*, int32_t, uint32_t, int32_t)) {  // 1.13c D2MCPClient+0x0305F
    D2McpCpInfo info{};
    if (get_cp_info(codepage, &info) == 1) {
        char charset[0x100];
        uint16_t ctype1[0x100];
        char lower[0x100];
        char upper[0x100];

        for (uint32_t i = 0; i < 0x100; ++i) charset[i] = (char)i;
        charset[0] = ' ';
        if (info.lead_byte[0] != 0) {
            for (const uint8_t* p = info.lead_byte + 1;; p += 2) {
                for (uint32_t i = p[-1]; i <= *p; ++i) charset[i] = ' ';
                if (p[1] == 0) break;
            }
        }
        get_string_type(1, charset, 0x100, ctype1, codepage, locale, 0);
        lcmap_string(locale, 0x100, charset, 0x100, lower, 0x100, codepage, 0);
        lcmap_string(locale, 0x200, charset, 0x100, upper, 0x100, codepage, 0);
        for (uint32_t i = 0; i < 0x100; ++i) {
            if ((ctype1[i] & 1) == 0) {
                if ((ctype1[i] & 2) != 0) {
                    d2mcp305f_ctype[i + 1] |= 0x20;
                    d2mcp305f_charmap[i] = (uint8_t)upper[i];
                } else {
                    d2mcp305f_charmap[i] = 0;
                }
            } else {
                d2mcp305f_ctype[i + 1] |= 0x10;
                d2mcp305f_charmap[i] = (uint8_t)lower[i];
            }
        }
    } else {
        for (uint32_t i = 0; i < 0x100; ++i) {
            if (i < 0x41 || i > 0x5a) {
                if (i > 0x60 && i < 0x7b) {
                    d2mcp305f_ctype[i + 1] |= 0x20;
                    d2mcp305f_charmap[i] = (uint8_t)(i - 0x20);
                } else {
                    d2mcp305f_charmap[i] = 0;
                }
            } else {
                d2mcp305f_ctype[i + 1] |= 0x10;
                d2mcp305f_charmap[i] = (uint8_t)(i + 0x20);
            }
        }
    }
}
