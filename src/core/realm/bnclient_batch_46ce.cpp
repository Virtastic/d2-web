// src/core/realm/bnclient_batch_46ce.cpp -- two UNMAPPED 1.13c BNClient.dll rows reimplemented from the
// decompiles in build-wasm/ghidra-bnclient/decomp/0x<RVA>.c (alpha lane). Base 0x6FF20000; every global
// is named at its retail RVA. These are MSVC 2003 (VC7.1) Release CRT leaves: the heap-kind selector
// __heap_init reads and the SBCS locale-table reset. The OS platform/version words are arguments here
// (the D2MCPClient 0x233D/0x3036 bodies are the byte-identical cross-check); the locale state is
// module state this file owns.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

// 1.13c BNClient+0x046CE (26 B, caller 0x046E8 __heap_init): ___heap_select. On NT5 and later -- the OS
// platform word DAT_6FF3E324 == 2 (VER_PLATFORM_WIN32_NT) and the major version DAT_6FF3E330 > 4 -- the
// CRT uses the OS heap and returns 1; anything else selects the small-block heap and returns 3.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t bn_heap_select(uint32_t os_platform, uint32_t win_major) {
    return (os_platform == 2 && win_major > 4) ? 1 : 3;
}

// +0x1F980 DAT_6ff3f980: the 0x40-dword (0x100-byte) codepage table, followed at +0x1FA80 by one byte.
static uint32_t g_bn_sbcs_table_1f980[0x40];
static uint8_t  g_bn_mbcs_1fa80;
// +0x1FA84, +0x1F960, +0x1F958, +0x1FA90..+0x1FA98: the remaining locale-state dwords.
static uint32_t g_bn_mbcs_1fa84;
static uint32_t g_bn_mbcs_1f960;
static uint32_t g_bn_mbcs_1f958;
static uint32_t g_bn_mbcs_1fa90;
static uint32_t g_bn_mbcs_1fa94;
static uint32_t g_bn_mbcs_1fa98;

// 1.13c BNClient+0x0631F (41 B, caller 0x064E3): VC7.1 `setSBCS`. Zero the 0x40-dword table at
// 0x1F980 and the byte after it, then zero the six dword state words. Every write is module state this
// file owns.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_set_sbcs(void) {
    std::memset(g_bn_sbcs_table_1f980, 0, sizeof g_bn_sbcs_table_1f980);
    g_bn_mbcs_1fa80 = 0;
    g_bn_mbcs_1fa84 = 0;
    g_bn_mbcs_1f960 = 0;
    g_bn_mbcs_1f958 = 0;
    g_bn_mbcs_1fa90 = 0;
    g_bn_mbcs_1fa94 = 0;
    g_bn_mbcs_1fa98 = 0;
}
