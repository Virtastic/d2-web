// Click/command router of D2Client. Callees through D2cHost (client_host.h) in retail order.
#include "client_host.h"
#include <emscripten.h>

// 1.13c D2Client+0x1BF4C: stdcall(kind, a, b, flagsByte) ret 0x10, hidden EBP = null-target sentinel (nul).
// Sets DAT_6FBCC4D8 = 1 (*gC4D8), 0x3F5F0(); player = DAT_6FBCBBFC (gPlayer). rec[8] zeroed, rec[1] = player.
// target = 0x51A80(); rec[2] = target, rec[3] = a, rec[4] = b (the ebx/edi stack-slot values 0x1A80 leaves are the arg words).
// target != nul and *target in {2, 4}: rec[3] = 0x1630(ecx=target), rec[4] = 0x1660(ecx=target) (retail stores to rec[3], rec[4]).
// flags = 0; flagsByte & 8 and 0xC1C6(player, 10, nul) != 0 -> flags = 0x40; flagsByte & 4 -> flags |= 0x20.
// kind 0: nul target -> gC2F4 = gC2F8 = nul, else 0x51E30(esi=target); flags |= 5; rec[0] = flags; 0x1BDE0(ESI=&rec).
//      1: flags |= 9; 0x1BCC0(EDI=&rec).   2: flags |= 0x11, gC2F4 = gC2F8 = nul; 0x1B620(EAX=&rec).
//      3: nul -> gC2F4 = gC2F8 = nul, else 0x51E30; flags |= 6; 0x1BC40(ESI=&rec).
//      4: flags |= 0xA; 0x1BBC0(ESI=&rec).   5: flags |= 0x12, gC2F4 = gC2F8 = nul; 0x18A00(ECX=&rec).
//      >5: return 1.   Returns the route's eax.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_1bf4c(D2cHost h, uint32_t nul, uint32_t kind, uint32_t a, uint32_t b,
        uint32_t flagsByte, uint32_t player, uint32_t* rec8, uint32_t recTok, uint32_t* gC4D8, uint32_t* gC2F4, uint32_t* gC2F8,
        const uint32_t* targetView /* words at the target; null if none */) {
    *gC4D8 = 1;
    D2cCall z = { 0x3F5F0, 0, 0, 0, 0, 0, { 0, 0, 0, 0 } };
    h(&z);
    for (int i = 0; i < 8; ++i) rec8[i] = 0;
    rec8[1] = player;
    D2cCall t = { 0x51A80, 0, 0, 0, 0, 0, { 0, 0, 0, 0 } };
    uint32_t tgt = h(&t);
    rec8[2] = tgt; rec8[3] = a; rec8[4] = b;
    if (tgt != nul && targetView && (targetView[0] == 2 || targetView[0] == 4)) {
        D2cCall g1 = { 0x1630, 0, tgt, 0, 0, 0, { 0, 0, 0, 0 } };
        rec8[3] = h(&g1);
        D2cCall g2 = { 0x1660, 0, tgt, 0, 0, 0, { 0, 0, 0, 0 } };
        rec8[4] = h(&g2);
    }
    uint32_t f = 0;
    if (flagsByte & 8) {
        D2cCall q = { 0xC1C6, 0, 0, 0, 0, 0, { player, 10, nul, 0 } };
        if (h(&q)) f = 0x40;
    }
    if (flagsByte & 4) f |= 0x20;
    auto clearG = [&]() { *gC2F8 = nul; *gC2F4 = nul; };
    auto route = [&](uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t esi, uint32_t edi, uint32_t flg) {
        rec8[0] = flg;
        D2cCall c = { rva, eax, ecx, 0, 0, esi, { 0, 0, 0, 0 } };
        c.s[3] = edi;   // the EDI register argument rides in s[3]
        return h(&c);
    };
    switch (kind) {
    case 0: case 3: {
        uint32_t rva = kind == 0 ? 0x1BDE0 : 0x1BC40;
        if (tgt == nul) clearG();
        else { D2cCall e = { 0x51E30, 0, 0, 0, 0, tgt, { 0, 0, 0, 0 } }; h(&e); }
        return route(rva, 0, 0, recTok, 0, f | (kind == 0 ? 5u : 6u));
    }
    case 1: return route(0x1BCC0, 0, 0, 0, recTok, f | 9);
    case 2: clearG(); return route(0x1B620, recTok, 0, 0, 0, f | 0x11);
    case 4: return route(0x1BBC0, 0, 0, recTok, 0, f | 0xA);
    case 5: clearG(); return route(0x18A00, 0, recTok, 0, 0, f | 0x12);
    default: return 1;
    }
}
