// src/core/realm/bnclient_batch_4a11.cpp -- one UNMAPPED 1.13c BNClient.dll alpha-lane row (Stage A:
// implementation + marker only, no spec, no vector), transcribed from
// build-wasm/ghidra-bnclient/decomp/0x4A11.c (image base 0x6FF20000).
//
// Row: ___sbh_find_block 0x4A11, MSVC 2003 (VC7.1) Release CRT leaf. Same body as the D2MCPClient row
// 0x3544 (src/core/realm/mcp_sbh_find_block.cpp); the globals DAT_6FF40BCC (region list head) and
// DAT_6FF40BC8 (region count) arrive as arguments.
#include <cstdint>
#include <emscripten.h>

extern "C" uint32_t d2_mcp_sbh_find_block(uint32_t headerList, uint32_t headerCount, uint32_t block);

// 1.13c BNClient+0x04A11 (43 B, callers 0x2275/0x2427/0x3CF4): ___sbh_find_block(p). Walk the 0x14-byte
// sbh region headers from head to head + count*0x14; return the first whose base (+0x0C) is within
// 0x100000 bytes below p, else 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t bn_sbh_find_block(uint32_t header_list, uint32_t header_count,
                                                uint32_t block) {
    return d2_mcp_sbh_find_block(header_list, header_count, block);
}
