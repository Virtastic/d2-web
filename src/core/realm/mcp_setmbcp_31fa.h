#pragma once
#include <cstdint>
#include <cstddef>

// State is a byte-exact window of retail globals RVA 0x12268..0x1239B.
// The decompiler prints absolute addresses 0x6FA32268..0x6FA3239B.
struct D2McpMbcpState {
    uint32_t locale;
    uint32_t reserved_04;
    uint32_t multibyte;
    uint8_t reserved_0c[12];
    uint8_t classes[257];
    uint8_t reserved_119[3];
    uint32_t codepage;
    uint8_t reserved_120[8];
    uint32_t extra[3];
};
static_assert(offsetof(D2McpMbcpState, locale) == 0);
static_assert(offsetof(D2McpMbcpState, multibyte) == 8);
static_assert(offsetof(D2McpMbcpState, classes) == 0x18);
static_assert(offsetof(D2McpMbcpState, codepage) == 0x11c);
static_assert(offsetof(D2McpMbcpState, extra) == 0x128);
static_assert(sizeof(D2McpMbcpState) == 0x134);
struct D2McpMbcpInfo {
    uint32_t max_char_size;
    uint8_t default_char[2];
    uint8_t lead_byte[12];
};
static_assert(offsetof(D2McpMbcpInfo, default_char) == 4);
static_assert(offsetof(D2McpMbcpInfo, lead_byte) == 6);
static_assert(sizeof(D2McpMbcpInfo) == 20);
struct D2McpMbcpEnv {
    int32_t (*get_cp_info)(uint32_t, D2McpMbcpInfo*);
    // Runs setMBCS against the SAME state, preserving the category bits already set.
    void (*derive)(D2McpMbcpState*);
};
extern "C" int32_t d2_mcp_setmbcp_31fa(uint32_t cp, uint32_t pseudo,
                                     D2McpMbcpState* state, const D2McpMbcpEnv* env);
