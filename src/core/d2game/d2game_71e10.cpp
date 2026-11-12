// 1.13c D2Game+0x71E10, transcribed from the disassembly (PlrTrade accept: move each traded item into the other player's inventory).
// Convention: EAX = receiving player B, stack arg0 = game, arg1 = offering unit, ret 8. Every callee goes through Ops::k (rva, eax, ecx, edx, ebx, esi, edi,
// stack words s[0..6], s[0] = the last pushed), the 8A3E0 packet send through Ops::send. A failure sends {0x77,0x0a} to the offer's client and {0x77,0x09} to
// B's client and returns 0; success returns 1. The local slot arg0 is overwritten by the second loop (retail passes its address to #10758).
#include <emscripten.h>
#include <cstdint>
#include <cstring>

struct D2Game71E10Ops {
    void* ctx;
    uint32_t (*k)(void* ctx, uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t ebx, uint32_t esi, uint32_t edi, const uint32_t* s);
    void (*send)(void* ctx, uint32_t client, const uint8_t* buf, uint32_t len);
};

namespace {
inline uint32_t R32(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; }
inline uint32_t R8(uint32_t p, uint32_t o) { return *(const uint8_t*)(uintptr_t)(p + o); }
inline void W32(uint32_t p, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(p + o), &v, 4); }
}

extern "C" uint32_t d2_fn_d2game_71E10(uint32_t B, uint32_t game, uint32_t offer, const D2Game71E10Ops* o)
{
    auto K = [&](uint32_t rva, uint32_t eax = 0, uint32_t ecx = 0, uint32_t edx = 0, uint32_t ebx = 0, uint32_t esi = 0, uint32_t edi = 0,
                 uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0, uint32_t s4 = 0, uint32_t s5 = 0, uint32_t s6 = 0) -> uint32_t {
        const uint32_t s[7] = { s0, s1, s2, s3, s4, s5, s6 };
        return o->k(o->ctx, rva, eax, ecx, edx, ebx, esi, edi, s);
    };
    auto clientOf = [&](uint32_t u) -> uint32_t { return (u && R32(u, 0) == 0) ? R32(K(0xA4C0, 0, 0, 0, 0, 0, 0, u), 0x9c) : 0; };
    auto fail = [&]() -> uint32_t {
        // stack locals, as retail builds them (byte stores, so the optimiser cannot fold them into a read-only constant)
        uint8_t m1[2], m2[2];
        m1[0] = 0x77; m1[1] = 0x0a; m2[0] = 0x77; m2[1] = 0x09;
        o->send(o->ctx, clientOf(offer), m1, 2);
        o->send(o->ctx, clientOf(B), m2, 2);
        return 0;
    };
    const uint32_t flags = R8(clientOf(offer), 0xa);        // the client data byte (retail dereferences it unguarded)
    const uint32_t mask = flags & 4;
    uint32_t node = K(0xA3DC, 0, 0, 0, 0, 0, 0, R32(offer, 0x60));
    while (node) {
        const uint32_t unit = K(0xA46C, 0, 0, 0, 0, 0, 0, node);
        const uint32_t next = K(0xA4CC, 0, 0, 0, 0, 0, 0, node);
        if ((K(0xA6C4, 0, 0, 0, 0, 0, 0, unit) & 0xff) == 2) {
            const uint32_t item = K(0x107E0, game, 0, 0, 0, 0, 0, unit, 0);
            if (item) {
                K(0x70010, 0, 0, 0, 0, item, 0, mask);
                K(0xA4A2, 0, 0, 0, 0, 0, 0, item, 4);
                K(0xA92E, 0, 0, 0, 0, 0, 0, item, 0);
                const uint32_t id = R32(item, 0xc);
                if (!game || !B) return fail();
                K(0xD29F0, 0, 0, 0, 0, 0, 0, B);
                uint32_t p = R32(game, 0x1720 + (id & 0x7f) * 4);
                while (p && R32(p, 0xc) != id) p = R32(p, 0xe4);
                if (!p || R32(p, 0) != 4 || R32(p, 0x10) != 4) return fail();
                const uint32_t ty = K(0xA6C4, 0, 0, 0, 0, 0, 0, p) & 0xff;
                const uint32_t inv = R32(B, 0x60);
                if (!inv) return fail();
                const uint32_t edx = R32(game, 0x70);
                uint32_t v14 = 1, v18 = 0;
                const uint32_t r = K(0xA634, 0, 0, 0, 0, 0, 0, B, ty, edx);
                if (R32(B, 0) == 0) { if (ty == 1) v14 = 0; else if (ty == 2) v18 = 1; }
                if (!K(0xAA9C, 0, 0, 0, 0, 0, 0, inv, p, r, 0, ty, 0x6fd1bd3c, 0x9cc)) return fail();
                if (!K(0xA9B8, 0, 0, 0, 0, 0, 0, inv, p, 1)) return fail();
                if ((K(0xA6C4, 0, 0, 0, 0, 0, 0, p) & 0xff) != 4) K(0xD4670, p, B);
                W32(p, 0xc4, R32(p, 0xc4) & 0xfffffffd);
                if (K(0xA640, 0, 0, 0, 0, 0, 0, p, B)) K(0xD47C0, 0, 0, 0, 0, B, p, 0);
                if (v14) K(0xAA54, 0, 0, 0, 0, 0, 0, inv, 0);
                K(0xA4A2, 0, 0, 0, 0, 0, 0, p, 0);
                K(0xA916, 0, 0, 0, 0, 0, 0, p, 2, 1);
                if (K(0xD1BC0, p)) K(0xA65E, 0, 0, 0, 0, 0, 0, p, 1, 1);
                if (K(0xA8A4, 0, 0, 0, 0, 0, 0, p, 0x4000, 0x8ef, 0x6fd1b14c)) K(0xA65E, 0, 0, 0, 0, 0, 0, p, 0x4000, 0);
                K(0xA9A6, 0, 0, 0, 0, 0, 0, B, 1);
                K(0xA994, 0, 0, 0, 0, 0, 0, inv, p);
                if (v18 == 1) K(0x71B00, p, game, 0, 0, 0, 0, B);
                if (K(0xA640, 0, 0, 0, 0, 0, 0, p, B)) { K(0xD51A0, 0, 0, 0, 0, 0, 0, game, B, 0); K(0xA9A6, 0, 0, 0, 0, 0, 0, B, 1); }
                if (unit && R32(unit, 0) == 4 && K(0xA9EE, 0, 0, 0, 0, 0, 0, unit)) {
                    const uint32_t x = R32(unit, 0x60);
                    if (x && K(0xA490, 0, 0, 0, 0, 0, 0, x) > 0) {
                        if (!K(0xD5080, unit, 0, 0, 0, 0, 0, game, B, item, 1)) return fail();
                    }
                }
            }
        }
        node = next;
    }
    clientOf(offer);                                        // the list head is re-read for the second loop
    uint32_t arg0slot = 0;
    node = K(0xA3DC, 0, 0, 0, 0, 0, 0, R32(offer, 0x60));
    while (node) {
        const uint32_t next = K(0xA4CC, 0, 0, 0, 0, 0, 0, node);
        const uint32_t u = K(0xA46C, 0, 0, 0, 0, 0, 0, node);
        const uint32_t t = K(0xA6C4, 0, 0, 0, 0, 0, 0, u) & 0xff;
        if (t == 1 || t == 2) {
            if (t == 2) K(0xD2BE0, u);
            arg0slot = 0;
            K(0xAA5A, 0, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)&arg0slot, 0x20, 1);
            K(0xA958, 0, 0, 0, 0, 0, 0, u, t);
            K(0xAC810, u, 0, 0, 0, 0, offer, arg0slot);
            K(0xD2D90, offer, 0, 0, 0, 0, 0, game, 0);
        }
        node = next;
    }
    return 1;
}
