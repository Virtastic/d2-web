// 1.13c D2Client+0x760F0 (207 B): the party panel's local-player name. ECX = argument handed on to 0x9D850 (ESI = gBBFC, the local unit),
// which returns the narrow name. No unit: returns the entry EAX; no name: returns 0. n = unicodenwidth(name, strlen) + 1; n > 11 asserts
// first (0xD3A8, ECX = n < 14 ? 0 : 6). A 2n-byte buffer comes from Fog 0xBF7A (ECX = 2n, stack 0x211, 0), takes toUnicode(name, n), and is
// drawn by 0xD37E (ECX = buffer, EDX = gB9A0 + 0x14 + ((0xAB - width) >> 1 when narrower than 0xAB), stack 0x1F - gB9A4, 0, 0), then
// 0xD3A8 (ECX 0) and Fog 0xBF86 frees it (ECX = buffer, stack 0x21A, 0). Returns the free call's EAX.
#include <cstdint>
#include <emscripten.h>
#include "d2c_tounicode.h"

struct D2c760f0Host {
    const char* (*name)(uint32_t ecx, uint32_t unit);                                      // 0x9D850: ECX, ESI
    uint32_t (*bad)(uint32_t ecx);                                                          // 0xD3A8
    uint16_t* (*alloc)(uint32_t size, uint32_t line, uint32_t z);                           // 0xBF7A: ECX = size, stack line, 0
    int32_t (*width)(const uint16_t* s);                                                    // 0xD35A
    uint32_t (*draw)(const uint16_t* s, int32_t x, int32_t y, uint32_t col, uint32_t z);    // 0xD37E
    uint32_t (*free)(uint16_t* p, uint32_t line, uint32_t z);                               // 0xBF86: ECX = block, stack line, 0
};

extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_760f0(uint32_t ecx, uint32_t eax_in, uint32_t g_bbfc, int32_t g_b9a0, int32_t g_b9a4, const D2c760f0Host* h) {
    if (!g_bbfc) return eax_in;
    const char* nm = h->name(ecx, g_bbfc);
    if (!nm) return 0;
    int32_t len = 0; while (nm[len]) ++len;
    const uint32_t n = d2c::unicodenwidth(nm, len) + 1;
    if (n > 0xb) h->bad(n < 0xe ? 0 : 6);
    uint16_t* buf = h->alloc(n * 2, 0x211, 0);
    d2c::toUnicode(buf, nm, (int32_t)n);
    const int32_t y = (int32_t)(0x1fu - (uint32_t)g_b9a4);
    int32_t x = (int32_t)((uint32_t)g_b9a0 + 0x14u);
    const int32_t w = h->width(buf);
    if (w < 0xab) x += (0xab - w) >> 1;
    h->draw(buf, x, y, 0, 0);
    h->bad(0);
    return h->free(buf, 0x21a, 0);
}
