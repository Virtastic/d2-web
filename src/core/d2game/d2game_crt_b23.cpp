// src/core/d2game/d2game_crt_b23.cpp -- six UNMAPPED 1.13c D2Game.dll rows reimplemented from the cached
// decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c), in RVA order: three __unlock tail thunks, the
// TLS-slot allocation, __mtterm, __initptd. MSVC 2003 (VC7.1) Release CRT glue the linker pulled into the
// DLL. Alpha batch lane (OBJECTIVE v8.3): implementation + marker only, no spec, no
// vector.
//
// Prior art: src/core/gfx/d2ddraw_batch_10da.cpp (the same CRT trio at 0x1B85/0x1B8E/0x1BAB) and
// src/core/d2game/d2game_batch_3e70.cpp (D2Game's own 0x3F76 __unlock, declared here and called).
#include <cstdint>
#include <emscripten.h>

// Defined in src/core/d2game/d2game_batch_3e70.cpp.
extern "C" void d2_fn_d2game_3f76(int32_t lockNum);

// __mtterm state: +0x1F204 the TLS index (0xFFFFFFFF = TLS_OUT_OF_INDEXES, not allocated) and +0x10FD94
// the TlsFree import pointer. __mtdeletelocks (RVA 0x3F21) is a row not ported yet, so it is a host seam.
static uint32_t g_tls_index = 0xFFFFFFFFu;   // +0x1F204  DAT_6fd1f204
static void (*g_tls_free)(uint32_t);         // +0x10FD94 DAT_6fd2fd94 = TlsFree
static void (*g_mtdeletelocks)(void);        // RVA 0x3F21, a row not ported

// 1.13c D2Game+0x3B51: tail fragment into RVA 0x3F76 with lock number 0xC -- __unlock(12). Reached from
// __updatetlocinfo (0x3B1F) when the locale it just installed is a new one.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_3b51(void) { d2_fn_d2game_3f76(0xC); }

// 1.13c D2Game+0x3B5A: TlsAlloc(), result discarded. wasm has no TLS indices, and the index would leak
// either way (retail never stores the result), so the rule is an empty statement.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_3b5a(void) { }

// 1.13c D2Game+0x3B63 (__mtterm): if the TLS index is allocated, free it and reset the slot to -1; then
// __mtdeletelocks(). Returns nothing.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_3b63(void) {
    if (g_tls_index != 0xFFFFFFFFu) {
        if (g_tls_free) g_tls_free(g_tls_index);
        g_tls_index = 0xFFFFFFFFu;
    }
    if (g_mtdeletelocks) g_mtdeletelocks();
}

// 1.13c D2Game+0x3B80 (__initptd): seed a thread-data block -- _initaddr +0x54 = the retail VA
// 0x6FD1F760 (stored as its RVA 0xFF760, the port's convention for retail VAs) and _holdrand +0x14 = 1.
// The locale argument (_Locale) is not touched. Returns nothing.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_3b80(uint8_t* ptd, void* locale) {
    (void)locale;
    *(uint32_t*)(ptd + 0x54) = 0xFF760u;   // _initaddr, retail 0x6FD1F760
    *(uint32_t*)(ptd + 0x14) = 1;          // _holdrand
}

// 1.13c D2Game+0x3D36: tail fragment into RVA 0x3F76 with lock number 0xD -- __unlock(13). One of the
// two releases in __freefls (0x3C04).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_3d36(void) { d2_fn_d2game_3f76(0xD); }

// 1.13c D2Game+0x3D42: tail fragment into RVA 0x3F76 with lock number 0xC -- __unlock(12). The other
// release in __freefls (0x3C04).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_3d42(void) { d2_fn_d2game_3f76(0xC); }
