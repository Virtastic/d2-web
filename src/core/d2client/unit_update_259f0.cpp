// 1.13c D2Client batch lane: per-unit update at +0x259F0 (ECX = unit u, EAX = in_EAX pointer). Callees are
// host seams; u/state bytes are the serialized 32-bit layouts.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2c259Env {
    void (*ord11090)(void*, uint32_t);          // (u, eax[1])
    void (*f6a220)(void);
    void (*f6b300)(void*, int, int, int, int);
    void (*ord10408)(void*);
    void (*f24920)(void*);
    void (*f25040)(uint32_t);                   // (*eax)
    void (*f237e0)(void*);
    void (*ord10595)(void*);
};

static uint32_t rd32(const void* p, unsigned o) { uint32_t v; std::memcpy(&v, static_cast<const uint8_t*>(p) + o, 4); return v; }
static void wr32(void* p, unsigned o, uint32_t v) { std::memcpy(static_cast<uint8_t*>(p) + o, &v, 4); }

// 1.13c D2Client+0x259F0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_259f0(uint8_t* u, const uint32_t* eax, const D2c259Env* e) {
    const uint8_t* s = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(rd32(u, 0x14)));  // *(u+0x14) -> state ptr
    const uint32_t k = rd32(u, 0x10);
    if (s[0x13a] == 0) {
        e->ord11090(u, eax[1]);
        e->f6a220();
        const uint32_t t = rd32(u, 4);
        if (s[0x1b5] && (t == 0x162 || t == 0x163 || t == 0x18d || t == 0x195 || t == 0x197 || t == 0x196 || t == 0x164) && k == 0)
            e->f6b300(u, 3, 0, 0, 0);
        const uint32_t f = s[k + 0xc4] ? (rd32(u, 0xc4) | 2) : (rd32(u, 0xc4) & ~2u);
        wr32(u, 0xc4, f);
        if (s[k + 0x120] == 0) e->ord10408(u);
        if (s[k + 0x131] == 1) wr32(u, 0xc4, rd32(u, 0xc4) | 0x100000);
        e->f24920(u);
    } else {
        e->f25040(eax[0]);
    }
    e->f237e0(u);
    e->ord10595(u);
    if (s[k + 0x118] == 0) wr32(u, 0xc4, rd32(u, 0xc4) | 0x20);
    else wr32(u, 0xc4, rd32(u, 0xc4) & ~0x20u);
}
