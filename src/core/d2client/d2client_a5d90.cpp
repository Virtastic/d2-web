// 1.13c D2Client+0xA5D90, written from the disassembly: the display-name string of a (kind, ...) record. EAX = obj, no stack
// arguments, plain ret; ECX at entry only supplies the stale upper half of the string id where the code writes just CX.
// obj+0 selects the kind (jump table at 0xA5F54): 1 / 2 / 4 as below, every other kind (0, 3, 5, > 5) returns 0; a null obj is
// the Fog assert 0x99D then _exit. 0xD1EC is the D2Lang string lookup (ECX = id). 0xA5310 takes EAX = the node the unit test
// 0xA5340 found and the unit as its stack word. The pointer pair 0xC3DC fills is (arr, n): stack words 1 and 2.
#include "cl_env.h"
extern "C" uint32_t d2cl_a5d90(const cl::Env& e, uint32_t obj, uint32_t ecx0) {
    const auto P = [](const void* p) { return (uint32_t)(uintptr_t)p; };
    const auto D = [&](uint32_t id) { return e.H(0xd1ec, 0, 0, 0, 0, 0, 0, id); };
    if (!obj) { const uint32_t m = e.H(0xbf92, 0x99d); e.H(0xbf80, 0x6fb8359fu, m); e.H(0x3664, 0xffffffffu); return 0; }
    switch (e.R(obj)) {
    case 1: {
        const uint32_t s = e.H(0x16e0, 0, 0, 0, 0, 0, obj);
        if (e.H(0xceab0, 0, 0, 0, 0, 0, 0, s)) return s;
        const uint32_t p = e.H(0x1230, 0, 0, 0, 0, 0, e.R(obj + 4));
        return D((s & 0xffff0000u) | ((e.R(p + 4) >> 16) & 0xffffu));
    }
    case 2: {
        const uint32_t rec = e.R(obj + 0x14);
        uint32_t ci = e.R(rec);
        if ((e.R(ci + 0x1b8) & 0xff) && e.R(obj + 0x10) == 2) {
            const uint32_t me = e.H(0xc1f0, obj);
            uint32_t f = e.H(0xa5340, 0, 0, 0, 0, 0, e.R(me + 0x74));
            if (f) return e.H(0xa5310, me, 0, 0, 0, 0, f);
            uint32_t arr = 0, n = 0;
            e.H(0xc3dc, me, P(&arr), P(&n));
            for (uint32_t i = 0; i < n; ++i) {
                const uint32_t el = e.R(arr + 4 * i);
                if (el == me) continue;
                f = e.H(0xa5340, 0, 0, 0, 0, 0, e.R(el + 0x74));
                if (f) return e.H(0xa5310, e.R(arr + 4 * i), 0, 0, 0, 0, f);
            }
        }
        ci = e.R(rec);
        if (e.R(ci + 0x13a) & 0xff) {
            const uint32_t k = e.R(obj + 0x10);
            return D(k == 5 || k == 4 ? 0xc9c : k == 0 ? 0xc99 : k == 6 ? 0xc9d : 0xc9b);
        }
        if (e.H(0xc6d6, obj)) return D(((rec & 0xffff0000u) | (e.R(rec + 4) & 0xffu)) + 0x2a39);
        if ((e.R(ci + 0x171) & 0xff) && (e.R(rec + 4) & 0x80)) return D(0xcbe);
        return ci + 0x40;
    }
    case 4: {
        const uint32_t p = e.H(0xc1ba, e.R(obj + 4));
        return D((ecx0 & 0xffff0000u) | (e.R(p + 0xf4) & 0xffffu));
    }
    default: return 0;
    }
}
