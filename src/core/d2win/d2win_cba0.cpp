// 1.13c D2Win+0xcba0 -- merged component table builder.
// Retail layout: EAX = ECX = the excel context handed to 0xC810 three times; the module globals 0x6F9A8C68.. are the
// 32-bit State words below (offsets from 0x6F9A8C68). Pointers are 32-bit retail words; every callee is an injectable seam.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2WinCba0State {          // module globals 0x6F9A8C68..0x6F9A8CA8
    uint32_t total;              // +0x00 c68
    uint32_t base;               // +0x04 c6c
    uint32_t cur0;               // +0x08 c70
    uint32_t n0;                 // +0x0c c74
    uint32_t cur1;               // +0x10 c78
    uint32_t n1;                 // +0x14 c7c
    uint32_t cursor;             // +0x18 c80
    uint32_t n2;                 // +0x1c c84
    uint32_t index;              // +0x20 c88
    uint32_t pad[5];             // +0x24..0x34
    uint32_t names;              // +0x38 ca0
    uint32_t extra;              // +0x3c ca4
};
struct D2WinCba0Env {
    uint32_t (*c810)(uint32_t name, uint32_t ctx, const uint32_t* cols, uint32_t* count, uint32_t stride);   // 0xC810 (ret 0xC)
    uint32_t (*alloc)(uint32_t size, uint32_t line);     // thunk 0x76AE (Fog 10045)
    void (*free_)(uint32_t p, uint32_t line);            // thunk 0x76E4 (Fog 10046)
    uint32_t (*nameNew)(uint32_t line);                  // thunk 0x76C6 (Fog 10211)
    void (*nameAdd)(uint32_t tbl, uint32_t key);         // thunk 0x76D8 (Fog 10215)
};

extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_cba0_env(uint32_t ctx, D2WinCba0State* g, const D2WinCba0Env* e) {
    const uint32_t M = 0x6F8E0000u;           // retail image base: names are module addresses
    uint32_t c[60] = {};
    auto col = [&](int i, uint32_t nameRva, uint32_t type, uint32_t off, uint32_t extra) {
        c[i * 5] = M + nameRva; c[i * 5 + 1] = type; c[i * 5 + 3] = off; c[i * 5 + 4] = extra;
    };
    col(0, 0x1cda0, 3, 0xf6, 0);        col(1, 0x1cd98, 0xf, 0x11e, M + 0xc8ca4);
    col(2, 0x1cd90, 9, 0x80, 0);        col(3, 0x1cd80, 9, 0x90, 0);
    col(4, 0x1cd74, 4, 0x133, 0);       col(5, 0x1cd6c, 4, 0xfd, 0);
    col(6, 0x1cd60, 4, 0x115, 0);       col(7, 0x1cd58, 9, 0xc0, 0);
    col(8, 0x1cd48, 9, 0xc4, 0);        col(9, 0x1cd3c, 6, 0x13d, 0);
    col(10, 0x1cd30, 4, 0x141, 0);      col(11, 0x1cd2c, 0, 0, 0);
    uint32_t a = e->c810(M + 0x1cd24, ctx, c, &g->n0, 0x1a8);
    uint32_t b = e->c810(M + 0x1cd1c, ctx, c, &g->n1, 0x1a8);
    uint32_t d = e->c810(M + 0x1cd14, ctx, c, &g->n2, 0x1a8);
    g->total = g->n2 + g->n1 + g->n0;
    uint32_t p = e->alloc(g->total * 0x1a8, 0xa29);
    g->cursor = p; g->base = p; g->cur0 = p;
    memcpy((void*)(uintptr_t)p, (const void*)(uintptr_t)a, g->n0 * 0x1a8);
    p += g->n0 * 0x1a8; g->cursor = p; g->cur1 = p;
    memcpy((void*)(uintptr_t)p, (const void*)(uintptr_t)b, g->n1 * 0x1a8);
    p += g->n1 * 0x1a8; g->cursor = p;
    memcpy((void*)(uintptr_t)p, (const void*)(uintptr_t)d, g->n2 * 0x1a8);
    e->free_(a - 4, 0xa39); e->free_(b - 4, 0xa3a); e->free_(d - 4, 0xa3b);
    g->index = e->alloc(g->total * 2, 0xa40);
    memset((void*)(uintptr_t)g->index, 0, g->total * 2);
    uint32_t k = 0;
    for (int32_t i = 0; i < (int32_t)g->total; i++)
        if (*(int16_t*)(uintptr_t)(g->base + (uint32_t)i * 0x1a8 + 0xf6) == 0) { ((uint16_t*)(uintptr_t)g->index)[k++] = (uint16_t)i; }
    g->names = e->nameNew(0xa4b);
    for (int32_t i = 0; i < (int32_t)g->total; i++)
        e->nameAdd(g->names, *(uint32_t*)(uintptr_t)(g->base + (uint32_t)i * 0x1a8 + 0x80));
}
