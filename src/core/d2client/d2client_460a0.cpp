// D2Client 0x460A0: clear the ten 0x10-strided slots at 0x119685, then walk the unit's +0x60 list
// and hand each entry to 0xA6EF0 (caller 0x46A60).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "client_host.h"
// 1.13c D2Client+0x460A0 ; unit = EAX (or null), clr = the 0x94 bytes at 0x119685 (dwords at +0, +0x10 .. +0x90, every one a
// dword store, the first included). The list calls are D2Common ordinals by thunk: 0xC4F0 first (Ordinal_10460), 0xC568 next
// (Ordinal_10464), 0xC526 entry (Ordinal_11147), then 0xCE3E (Ordinal_11024) and 0xCE1A (Ordinal_10251) on the handle. Each entry
// {type +0, .., key +0xC} goes to 0xA6EF0 as ECX = 0x10A608 + type*0x200 (module), EDX = key & 0x7F, stack key, type. The next
// node is fetched before the entry, as retail does.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_460a0(const uint8_t* unit, uint8_t* clr, uint32_t base, uint32_t* (*at)(uint32_t), D2cHost host) {
    for (int i = 0; i < 10; ++i) std::memset(clr + i * 0x10, 0, 4);
    if (!unit) return;
    uint32_t hnd; std::memcpy(&hnd, unit + 0x60, 4);
    if (!hnd) return;
    D2cCall c = {}; c.rva = 0xC4F0; c.s[0] = hnd;
    for (uint32_t it = host(&c); it;) {
        D2cCall n = {}; n.rva = 0xC568; n.s[0] = it; uint32_t nx = host(&n);
        D2cCall e = {}; e.rva = 0xC526; e.s[0] = it; uint32_t r = host(&e);
        if (r) {
            const uint32_t* p = at(r);
            D2cCall f = {}; f.rva = 0xA6EF0; f.ecx = base + 0x10A608u + p[0] * 0x200u; f.edx = p[3] & 0x7F; f.s[0] = p[3]; f.s[1] = p[0];
            host(&f);
        }
        it = nx;
    }
    D2cCall a = {}; a.rva = 0xCE3E; a.s[0] = hnd; host(&a);
    D2cCall b = {}; b.rva = 0xCE1A; b.s[0] = hnd; host(&b);
}
