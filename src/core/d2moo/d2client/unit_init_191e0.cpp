// 1.13c D2Client+0x191E0: client-unit init. Callees go through cb(rva, eax, stack words, n).
#include <emscripten.h>
#include <cstdint>
typedef uint32_t (*Cb191E0)(uint32_t rva, uint32_t eax, const uint32_t* st, uint32_t n);
// g[0] = 0x11B7FC, g[1..8] = the unit slot table at 0x11B800; self = [0x11BBFC]; unit/ecx = u, eax = e, a1/a2 stack args (ret 8).
extern "C" void d2_client_191e0(uint32_t* g, uint32_t self, uint32_t* u, uint32_t e, uint32_t a1, uint32_t a2, Cb191E0 cb) {
    for (int i = 0; i < 8; ++i) if (g[1 + i] == 0) { g[1 + i] = (uint32_t)(uintptr_t)u; break; }
    const uint32_t U = (uint32_t)(uintptr_t)u;
    auto c = [&](uint32_t rva, uint32_t n, uint32_t a, uint32_t b = 0, uint32_t d = 0, uint32_t f = 0, uint32_t h = 0, uint32_t k = 0) { uint32_t s[6] = { a, b, d, f, h, k }; return cb(rva, e, s, n); };
    c(0xC3F4, 6, 0, e, a1, a2, U, 1);
    if (u[0x60 / 4] == 0) u[0x60 / 4] = c(0xC63A, 2, 0, U);
    u[0xA8 / 4] = c(0xCD6C, 1, 0);
    c(0xCFF4, 1, U);
    c(0xC490, 4, U, 0, 0xB2000000u | 0x18C20, 0);
    c(0xC40C, 4, U, 0x44, 100, 0); c(0xC40C, 4, U, 0x43, 100, 0); c(0xC40C, 4, U, 0x45, 100, 0);
    g[0] = c(0xC208, 1, e);
    c(0xC43C, 2, U, 5);
    u[0xC4 / 4] = (u[0xC4 / 4] & 0xFFFFFFDFu) | 0xE;
    if (u[0x10 / 4] == 0x11 || u[0x10 / 4] == 0) { c(0xCDBA, 2, U, 1); c(0xCDD2, 2, U, 5); c(0xC400, 2, u[0x2C / 4], 0x8000); }
    if (c(0xC55C, 1, e) == 0) {
        uint32_t p = c(0xA8C20, 5, 13, 255, 255, 255, 255);
        if (p) { u[0x64 / 4] = p; u[0x68 / 4] = (uint32_t)((int32_t)((const uint32_t*)(uintptr_t)p)[0x18 / 4] >> 3); }
    }
    c(0xCFCA, 1, U);
    if (U != self) {
        uint64_t v = (uint64_t)u[0x20 / 4] * 0x6AC690C5u + u[0x24 / 4];
        u[0x20 / 4] = (uint32_t)v; u[0x24 / 4] = (uint32_t)(v >> 32);
        c(0xC3C4, 2, u[0x2C / 4], (uint32_t)v & 0x3F);
    }
    c(0x190B0, 0, 0);
}
