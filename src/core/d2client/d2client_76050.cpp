// 1.13c D2Client+0x76050 (149 B, cdecl, no arguments): the party panel's buy/sell button cell. An 18-dword draw block is zeroed;
// while the cell global gB9C4 is empty, the path "DATA\GLOBAL\ui\panel\buysellbtn" is formatted into a 260-byte local
// (wsprintfA "%s\ui\panel\buysellbtn") and 0x2B420 (EAX = the path, one stack word 0) loads it into gB9C4. Block [0] = 10 + (gBFF4 != 0),
// [13] = gB9C4, [16] = 0; D2Gfx 10041 (0xD096) draws it with stack (block, gB9A0 + 0x116, 0x1A5 - gB9A4, -1, 5, 0).
// Returns the draw call's EAX.
#include <cstdint>
#include <emscripten.h>

struct D2c76050Host {
    uint32_t (*load)(const char* path, uint32_t s0);                                                     // 0x2B420: EAX = path
    uint32_t (*draw)(const uint32_t* blk, int32_t x, int32_t y, int32_t a, int32_t b, int32_t c);        // 0xD096 (D2Gfx 10041)
};

extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_76050(uint32_t* g_b9c4, int32_t g_bff4, int32_t g_b9a0, int32_t g_b9a4, const D2c76050Host* h) {
    uint32_t blk[18] = {};
    if (*g_b9c4 == 0) {
        char path[260];
        const char* a = "DATA\\GLOBAL"; const char* b = "\\ui\\panel\\buysellbtn";
        int n = 0; while (*a) path[n++] = *a++; while (*b) path[n++] = *b++; path[n] = 0;
        *g_b9c4 = h->load(path, 0);
    }
    blk[0] = (g_bff4 != 0) + 10u;
    blk[13] = *g_b9c4;
    blk[16] = 0;
    return h->draw(blk, (int32_t)((uint32_t)g_b9a0 + 0x116u), (int32_t)(0x1a5u - (uint32_t)g_b9a4), -1, 5, 0);
}
