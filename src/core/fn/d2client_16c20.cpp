// 1.13c D2Client+0x16C20 probe. r[0] = source unit (EDI), r[1] = predicate (EBX, ignored: the probe's
// predicate is the recorder of 0x16E50), r[2]/r[3] = the two out-pointer stack words. Blobs: 0 lg, 1 u, 2 ra, 3 r0, 4 r1, 5 un, 6 sc.
// The retail blobs hold 32-bit images; each call marshals the fields the search reads (room +0x74 unit head, unit +0xE8 next) into
// native shells. The two callees and the predicate answer from the sc control blob and log through HookEnv exactly as the capture's hooks.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include "hookenv.h"
#include "../d2moo/include/Units/Units.h"
#include "../d2moo/include/Path/Path.h"
#include "../d2moo/include/Drlg/D2DrlgDrlg.h"

namespace c16c20 {
using Predicate = int32_t (*)(D2UnitStrc*, D2UnitStrc*, D2UnitStrc**, int32_t*);
struct Ops {
    D2ActiveRoomStrc* (*room)(D2UnitStrc*);
    void (*adjacent)(D2ActiveRoomStrc*, D2ActiveRoomStrc***, int32_t*);
    void* pathX; void* pathY; void* distance; void* action;
};
struct NU { D2UnitStrc u; uint32_t addr; };
struct NR { D2ActiveRoomStrc r; };
HookEnv* he; NU shells[16]; int nShell; NR rshell[4]; D2ActiveRoomStrc* rlist[4];
NU src; NR dummy; uint32_t srcAddr, roomAddr, raAddr, a2, a3; bool roomOn;
uint32_t rd(uint32_t a, uint32_t o) { return HookEnv::rd32(a + o); }
D2ActiveRoomStrc* room(D2UnitStrc*) {
    const uint32_t v = he->h(0xc1f0, 0, 0, 0, 0, 0, srcAddr);
    return v ? &dummy.r : nullptr;
}
void adjacent(D2ActiveRoomStrc*, D2ActiveRoomStrc*** out, int32_t* cnt) {
    uint32_t rooms32 = 0, cnt32 = 0;
    he->h(0xc3dc, 0, 0, 0, 0, 0, roomAddr, he->stk(&rooms32), he->stk(&cnt32));
    nShell = 0;
    const uint32_t n = cnt32 < 4 ? cnt32 : 4;   // retail reads only as many rooms as the walk reaches; the tuples keep the rest of ra zero
    for (uint32_t i = 0; i < n; ++i) {
        const uint32_t rb = rd(rooms32, 4 * i);
        std::memset(&rshell[i], 0, sizeof rshell[i]);
        NU* prev = nullptr;
        for (uint32_t ua = rd(rb, 0x74); ua && nShell < 16; ua = rd(ua, 0xe8)) {
            NU& s = shells[nShell++]; std::memset(&s, 0, sizeof s); s.addr = ua; s.u.dwUnitType = rd(ua, 0);
            if (prev) prev->u.pRoomNext = &s.u; else rshell[i].r.pUnitFirst = &s.u;
            prev = &s;
        }
        rlist[i] = &rshell[i].r;
    }
    *out = rlist; *cnt = (int32_t)cnt32;
}
uint32_t addrOf(D2UnitStrc* u) { for (int i = 0; i < nShell; ++i) if (&shells[i].u == u) return shells[i].addr; return 0; }
int32_t predicate(D2UnitStrc* s, D2UnitStrc* c, D2UnitStrc**, int32_t*) {
    (void)s; return (int32_t)he->h(0x16e50, 0, srcAddr, addrOf(c), 0, 0, a2, a3);
}
}

extern "C" int32_t d2_client_16c20(D2UnitStrc* source, c16c20::Predicate predicate, D2UnitStrc** result, int32_t* distance, const c16c20::Ops* ops);

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_16c20(uint32_t* r) {
    using namespace c16c20;
    const uint32_t lg = r[14], ra = r[18], r0 = r[20], sc = r[26];
    uint32_t pairs[64]; int n = 0;
    const uint32_t nv = rd(sc, 8);
    pairs[n++] = 0xc1f0; pairs[n++] = rd(sc, 0) ? r0 : 0;
    pairs[n++] = (2u << 24) | 0xc3dc; pairs[n++] = ra;
    pairs[n++] = (3u << 24) | 0xc3dc; pairs[n++] = rd(sc, 4);
    for (uint32_t i = 0; i < nv && i < 8; ++i) { pairs[n++] = 0x16e50; pairs[n++] = rd(sc, 12 + 4 * i); }
    pairs[n++] = 0; pairs[n++] = 0;
    HookEnv h((uint32_t*)(uintptr_t)lg, pairs); he = &h;
    srcAddr = r[0]; roomAddr = r0; raAddr = ra; a2 = r[2]; a3 = r[3]; nShell = 0;
    std::memset(&src, 0, sizeof src); std::memset(&dummy, 0, sizeof dummy);
    const Ops ops = { room, adjacent, nullptr, nullptr, nullptr, nullptr };
    r[4] = (uint32_t)d2_client_16c20(&src.u, predicate, (D2UnitStrc**)(uintptr_t)r[2], (int32_t*)(uintptr_t)r[3], &ops);
}
