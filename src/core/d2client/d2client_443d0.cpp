// 1.13c D2Client+0x443D0 -- client game-session init. Decompile-derived from the 1.13c disassembly.
// Resets the per-game client globals, creates the client pool, reapplies the saved resolution, opens the loading
// screen and stamps the loading and join clocks. Every callee goes through the D2cHost seam in retail order, with the
// registers and stack words retail passes; the globals are reached through D2c443D0G, one pointer per contiguous
// range in retail layout.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include "d2client_443d0.h"

namespace {
inline uint32_t addr(const void* p) { return (uint32_t)(uintptr_t)p; }
uint32_t call(D2cHost h, uint32_t rva, uint32_t eax = 0, uint32_t ecx = 0, uint32_t edx = 0, uint32_t ebx = 0, uint32_t esi = 0,
              uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0) {
    D2cCall c = { rva, eax, ecx, edx, ebx, esi, { s0, s1, s2, s3 }, { 0, 0, 0, 0 }, 0, 0 };
    return h(&c);
}
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_443d0(D2cHost h, const D2c443D0G* g) {
    static const char kPool[] = "Client Pool System", kKey[] = "Diablo II", kVal[] = "Resolution",
                      kSrc[] = "..\\Source\\D2Client\\GAME\\Game.cpp";
    uint32_t* s = g->s37c;
    s[21] = 0; s[13] = 0; s[12] = 0; s[4] = 0; s[11] = 0; s[29] = 0;      // 3D0 3B0 3AC 38C 3A8 3F0
    for (int i = 0; i < 0x20; ++i) g->p97e0[i] = 0;
    call(h, 0xC0AC, 0, 0, 0, 0, 0, addr(&g->p97e0[2]), addr(kPool), 0x40000, 0x1000);
    if (*g->bbfc) call(h, 0xC0250);
    *g->bbfc = 0;
    call(h, 0x419D0);
    if (!s[18]) s[18] = call(h, 0x8A090);
    uint32_t mode = 0;   // the `push ecx` local the registry read fills (retail leaves it uninitialised)
    call(h, 0xBE8A, 0, 0, 0, 0, 0, addr(kKey), addr(kVal), 0, addr(&mode));
    call(h, 0x10DF0, 0, 0, 0, 0, mode == 1 ? 2u : 0u);
    call(h, 0xC1580);
    call(h, 0xC1760, 0, 0, 0, 0, 0, 1);
    const uint32_t st = s[6];
    if (st == 7 || st == 9 || st == 6 || st == 8) call(h, 0x14D90, 0, 0, 0, 0, 0, 1);
    *g->c570 = g->handler;
    call(h, 0x538F0);
    call(h, 0xC172);
    call(h, 0xA04D0, 0, 0, 0, g->p97e0[2], 0, 1);
    s[10] = *g->aae20 != 0;
    g->p97e0[41] = g->tick();
    s[0] = g->tick();
    g->p97e0[1] = g->frameCb; g->p97e0[6] = 0; g->p97e0[5] = 0;
    uint32_t* ss = (uint32_t*)(uintptr_t)s[18];
    if (ss[9] || ss[10]) { const uint32_t f = ss[0]; ss[9] = 0; ss[10] = 0; ss[0] = f | 1; }
    s[39] = 0; s[38] = 0;                                                  // 418 414
    call(h, 0xD1B0, 0, 0);
    call(h, 0x8A4D0, g->p9e14[1], g->p9e14[0], 0, 0, s[18], 0);
    for (int i = 0; i < 7; ++i) g->b800[i] = 0;
    g->p212c[0] = 0xFFFFFFFFu; g->p212c[2] = 0xFFFFFFFFu;
    g->b800[7] = 0;
    for (int i = 0; i < 11; ++i) g->p960c[i] = 0;
    g->p97e0[31] = 0;                                                      // 0x11985C
    for (int i = 0; i < 4; ++i) g->c2e0[i] = 0;
    s[16] = call(h, 0xBF7A, 0, 8, addr(kSrc), 0, 0, 0x53F, 0);             // 3BC
    for (int i = 0; i < 0x600; ++i) g->p9a08[i] = 0;
    call(h, 0xA9C90, 0, 0, 0, 0, 0, 1);
    call(h, 0x80320);
    *g->p3f5c = 0;
    const uint16_t z16 = 0; const uint32_t z32 = 0;
    memcpy(g->c024, &z16, 2); memcpy(g->c024 + 4, &z32, 4);
    s[34] = 0; s[9] = 0;                                                   // 404 3A0
    g->p27d0[0] = 7; g->p27d0[1] = 7;
}
