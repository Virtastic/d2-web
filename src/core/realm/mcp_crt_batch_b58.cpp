// src/core/realm/mcp_crt_batch_b58.cpp -- seven UNMAPPED 1.13c D2MCPClient.dll rows reimplemented from
// the decompiles in build-wasm/ghidra-d2mcpclient/decomp/0x<RVA>.c (alpha lane). Base 0x6FA20000; every
// global is named at its retail RVA. These are MSVC 2003 (VC7.1) Release CRT leaves the linker pulled
// into the module: the new-handler malloc, the SBCS/MBCS locale tables, __msize, the
// ___updatetlocinfo lock wrapper, and the fatal-message banner. Host or module state no argument pins
// (the heap, the lock table, the installed new handler, the fatal-message writer) is a seam after the
// retail arguments, which keep their order (src/core/realm/mcp_crt_mem.cpp pattern).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

// 1.13c D2MCPClient+0x028BC (44 B, caller 0x028E8 malloc): VC7.1 `__nh_malloc(size, nh_flag)`. A
// request of 0xFFFFFFE1 bytes or more answers null without allocating. Otherwise allocate; on success
// return the block, on failure return null when nh_flag is 0, else call the installed new handler with
// the size and retry only while it answers non-zero. __heap_alloc (0x2841) and __callnewh (0x401F) are
// the seams.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_mcp_nh_malloc(uint32_t size, int32_t nh_flag,
                                            void* (*heap_alloc)(uint32_t),
                                            int32_t (*call_newh)(uint32_t)) {
    if (size >= 0xffffffe1u) return nullptr;
    for (;;) {
        void* p = heap_alloc(size);
        if (p != nullptr) return p;
        if (nh_flag == 0) return nullptr;
        if (call_newh(size) == 0) return nullptr;
    }
}

// +0x32268 DAT_6fa32268, +0x32270 DAT_6fa32270: the SBCS/MBCS state words the table clear also zeroes.
static uint32_t g_mbcs_32270;
static uint32_t g_mbcs_32268;
// +0x32280 DAT_6fa32280: the 0x40-dword (0x100-byte) codepage table, followed at +0x32380 by one byte.
static uint32_t g_sbcs_table_32280[0x40];
static uint8_t  g_mbcs_32380;
// +0x32384, +0x32390..+0x32398: the remaining locale-state dwords.
static uint32_t g_mbcs_32384;
static uint32_t g_mbcs_32390;
static uint32_t g_mbcs_32394;
static uint32_t g_mbcs_32398;

// 1.13c D2MCPClient+0x03036 (41 B, caller 0x031FA): VC7.1 `setSBCS`. Zero the 0x40-dword table at
// 0x32280 and the byte after it, then zero the six dword state words. Every write is module state this
// file owns.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_setsbcs_3036(void) {
    memset(g_sbcs_table_32280, 0, sizeof g_sbcs_table_32280);
    g_mbcs_32380 = 0;
    g_mbcs_32384 = 0;
    g_mbcs_32270 = 0;
    g_mbcs_32268 = 0;
    g_mbcs_32390 = 0;
    g_mbcs_32394 = 0;
    g_mbcs_32398 = 0;
}

// 1.13c D2MCPClient+0x03007 (47 B, caller 0x031FA): the codepage-to-charset word table read through
// EAX (`in_EAX` in the decompile). 0x3A4 -> 0x411, 0x3A8 -> 0x804, 0x3B5 -> 0x412, 0x3B6 -> 0x404,
// anything else 0. Pure.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_codepage_map_3007(uint32_t codepage) {
    switch (codepage) {
    case 0x3a4: return 0x411;
    case 0x3a8: return 0x804;
    case 0x3b5: return 0x412;
    case 0x3b6: return 0x404;
    default: return 0;
    }
}

// 1.13c D2MCPClient+0x02FCC (50 B, caller 0x05881): VC7.1 `___updatetlocinfo`. Take CRT lock 0xC
// (0x15A3), run ___updatetlocinfo_lk (0x2F0B), release lock 0xC through 0x02FFE, and return the
// helper's answer. The lock table and the locale helper are seams.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_mcp_updatetlocinfo(void (*lock)(int32_t),
                                                 void* (*updatetlocinfo_lk)(void),
                                                 void (*unlock)(int32_t)) {
    lock(0xc);
    void* p = updatetlocinfo_lk();
    unlock(0xc);
    return p;
}

// 1.13c D2MCPClient+0x02764 (57 B, caller 0x0141D __amsg_exit): VC7.1 `__FF_MSGBANNER`. When the
// error mode DAT_6fa2aa34 is 1, or is 0 and the app type DAT_6fa2aa38 is 1: write message 0xFC, call
// the installed banner hook DAT_6fa2acac if non-null, then write message 0xFF. The two state words, the
// writer 0x25EC and the hook are seams.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_ff_msgbanner(uint32_t error_mode, uint32_t app_type,
                                              void (*nmsg_write)(int32_t),
                                              void (*banner_hook)(void)) {
    if (error_mode == 1 || (error_mode == 0 && app_type == 1)) {
        nmsg_write(0xfc);
        if (banner_hook != nullptr) banner_hook();
        nmsg_write(0xff);
    }
}

// +0x325D4 DAT_6fa325d4: the multibyte-table initialised flag.
static uint32_t g_mbctable_initialised;

// 1.13c D2MCPClient+0x034DE (30 B, callers 0x01F46, 0x02179): VC7.1 `___initmbctable`. If the flag is
// clear, run __setmbcp(-3) (0x338E, host default-codepage setup) and set the flag; always return 0.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_initmbctable(int32_t (*setmbcp)(int32_t)) {
    if (g_mbctable_initialised == 0) {
        setmbcp(-3);
        g_mbctable_initialised = 1;
    }
    return 0;
}

// 1.13c D2MCPClient+0x02B9C (106 B, caller 0x015D4 __onexit_lk): VC7.1 `__msize(void*)`. When the
// small-block heap is active (heap mode DAT_6fa324a4 == 3): take CRT lock 4, ask ___sbh_find_block
// (0x3544) which region owns the block; if one does, the size is the dword eight bytes before the
// user pointer minus 9; release lock 4 through 0x02C09; return that size when a region matched.
// Otherwise (or when none matched) return HeapSize(heap handle DAT_6fa324a0, 0, p). Every host or
// module operand is a seam.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_msize_2b9c(void* mem, uintptr_t heap_handle, uint32_t heap_mode,
                                                void (*lock)(int32_t),
                                                void (*unlock)(int32_t),
                                                uint32_t (*sbh_find_block)(void*),
                                                uint32_t (*heap_size)(uintptr_t, uint32_t, void*)) {
    if (heap_mode == 3) {
        lock(4);
        uint32_t region = sbh_find_block(mem);
        uint32_t size = 0;
        if (region != 0) {
            uint32_t header;
            memcpy(&header, (uint8_t*)mem - 4, sizeof header);
            size = header - 9u;
        }
        unlock(4);
        if (region != 0) return size;
    }
    return heap_size(heap_handle, 0, mem);
}
