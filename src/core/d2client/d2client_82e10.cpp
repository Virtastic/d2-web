// 1.13c D2Client+0x82E10 (ESI = item packet): decode the packet tail into a 24-byte stack block (Ordinal 11105), look the packet's item id
// (dword +4) up in the client unit hash (D2Client+0x10AE08: 128 32-bit chain heads, chain link +0xE4, id +0xC; a hit of another unit
// type is the fatal 0x5A report) and hand the packet to 0x82A00 (mode +0x10 == 2: EAX = packet, block on the stack) or 0x82B20 (mode 4:
// packet, block). Returns the EAX retail leaves: 0 on a miss, the callee's return after a call, else the item's mode.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

typedef uintptr_t U;
struct D2cEnv82 {
    U    (*ord)(uint32_t n, U a, U b, U c, U d);
    U    (*fn)(uint32_t rva, U a, U b, U c, U d);
    void (*fatal)(uint32_t code);
};
struct D2cGlob82e10 { uint8_t* player; const uint8_t* unitHash; };   // the leading words of D2cGlob82 this function reads

static uint32_t rd32(U a) { uint32_t v; memcpy(&v, (const void*)a, 4); return v; }

// 1.13c D2Client+0x82E10
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_82e10(const D2cGlob82e10* g, const D2cEnv82* e, const uint8_t* esi) {
    uint8_t loc[24];
    e->ord(11105, (U)(esi + 8), esi[2] - 8u, 0, (U)loc);
    const uint32_t id = rd32((U)(esi + 4));
    U u = rd32((U)g->unitHash + (id & 0x7f) * 4);
    while (u && rd32(u + 0xC) != id) u = rd32(u + 0xE4);
    if (!u) return 0;
    if (rd32(u) != 4) e->fatal(0x5A);
    const uint32_t m = rd32(u + 0x10);
    if (m == 2) return (uint32_t)e->fn(0x82a00, (U)esi, (U)loc, 0, 0);   // EAX = pkt, [esp+4] = block
    if (m == 4) return (uint32_t)e->fn(0x82b20, (U)esi, (U)loc, 0, 0);   // stdcall: pkt, block
    return m;
}
