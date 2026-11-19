// 1.13c D2Client+0x45b40 probe; drives the client-tree body d2client_45b40::sub_45B40 (src/core/d2client/sub_45b40.cpp) through its Services seam.
// r[0] = unit (blob u, r[14], 16 bytes; its first dword is the key), r[1] = ECX (the out slot of
// 0xCE20, which the port reads back). Blob lg (r[16], 512 bytes) is the hook log. Each callee answers the key's row: the
// C1C6 queue holds the stat-0x48 answer then the stat-0x46 answer.
#include <cstdint>
#include <emscripten.h>
#include "hookenv.h"
#include "blobtag.h"
namespace d2client_45b40 {
using U = uint32_t;
struct Services {
    virtual ~Services() = default;
    virtual U o11088(U u) = 0; // D2Client+0xC220
    virtual U o10087(U u) = 0; // +0xCE14
    virtual U o10865(U u) = 0; // +0xC8BC
    virtual U o11116(U u) = 0; // +0xC916
    virtual U stat(U u, U id, U layer) = 0; // #10973 via +0xC1C6
    virtual U o10048(U u) = 0; // +0xC73C
    virtual U o10463(U u) = 0; // +0xC91C
    virtual U o10607(U u, U* out) = 0; // +0xCE20
};
U sub_45B40(U u, Services& s);
}
namespace {
struct Row { uint32_t c220, ce14, c8bc, c916, c73c, c91c, c1a, c1b, ce20; };
const Row kRow[10] = {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 4, 1, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 1, 10, 0, 0, 5, 5, 0 },
    { 0, 1, 1, 10, 1, 5, 10, 3, 0 },
    { 0, 1, 1, 0, 1, 7, 7, 7, 1 },
    { 0, 1, 0, 0, 0, 0, 0, 0, 1 },
    { 0, 1, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 1, 0xFFFFFFFFu, 0, 0, 0xFFFFFFFBu, 0xFFFFFFFBu, 0 },
    { 0, 1, 1, 3, 1, 2, 3, 2, 1 },
};
struct Svc : d2client_45b40::Services {
    HookEnv* env; const uint32_t* r; uint32_t ecx;
    uint32_t q(uint32_t rva, uint32_t u, uint32_t a = 0, uint32_t b = 0) {
        BlobTag T{r};
        return env->h(rva, 0, 0, 0, 0, 0, T(u), a, b, 0);
    }
    uint32_t o11088(uint32_t u) override { return q(0xC220, u); }
    uint32_t o10087(uint32_t u) override { return q(0xCE14, u); }
    uint32_t o10865(uint32_t u) override { return q(0xC8BC, u); }
    uint32_t o11116(uint32_t u) override { return q(0xC916, u); }
    uint32_t stat(uint32_t u, uint32_t id, uint32_t layer) override { return q(0xC1C6, u, id, layer); }
    uint32_t o10048(uint32_t u) override { return q(0xC73C, u); }
    uint32_t o10463(uint32_t u) override { return q(0xC91C, u); }
    uint32_t o10607(uint32_t u, uint32_t* out) override { *out = ecx; return q(0xCE20, u, 0xB3000000u); }   // the out slot holds the caller's ECX; the log records the slot as 0xB3000000
};
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_45b40(uint32_t* r) {
    const uint32_t unit = r[0];
    const uint32_t key = *(const uint32_t*)(uintptr_t)unit;
    const Row& w = kRow[key < 10 ? key : 0];
    uint32_t sc[] = { 0xC220, w.c220, 0xCE14, w.ce14, 0xC8BC, w.c8bc, 0xC916, w.c916, 0xC73C, w.c73c,
                      0xC91C, w.c91c, 0xC1C6, w.c1a, 0xC1C6, w.c1b, 0xCE20, w.ce20, 0, 0 };
    uint32_t* lg = (uint32_t*)(uintptr_t)r[16];
    HookEnv env(lg, sc);
    env.cap = 12;
    Svc s; s.env = &env; s.r = r; s.ecx = r[1];
    r[4] = d2client_45b40::sub_45B40(unit, s);
}
