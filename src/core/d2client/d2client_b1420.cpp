// 1.13c D2Client+0xB1420 (768 bytes): the client missile terminal handler.
// ECX = missile unit, EAX = target unit or 0, one stack word = force; `ret 4`. Returns the unit the explosion
// spawner 0xB0450 made (0 when it did not run or on any early exit). Retail 32-bit layout; every callee goes
// through the D2cHost seam in retail's call order with retail's registers and stack words.
//   flags  (esp+0x10) = 3, or 0xB0100's return on the target/owner path; bit 1 gates the hit work, bit 0 the send.
//   result (esp+0x14) = 0, then 0xB0450's return.
//   class  (esp+0x18) = (int16)row+0x16, read before anything else.
// Globals: *(0xCE474) = &sgptDataTables (rows +0xB64, count +0xB6C, stride 0x1A4); 0xCEE50 = &gdwBitMasks (byte +8);
// 0xDDA10 = per-row client-hit callback table, 0xDDB54 its count.
#include <cstdint>
#include <cstring>
#include "client_host.h"

namespace {
uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
int32_t rd16s(uint32_t a) { int16_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
uint8_t rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
uint32_t call(D2cHost h, uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t ebx, uint32_t esi,
              uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0, uint32_t s4 = 0) {
    D2cCall c{ rva, eax, ecx, edx, ebx, esi, { s0, s1, s2, s3 }, { 0, 0, 0, 0 }, s4, 0 };
    return h(&c);
}
}  // namespace

struct D2cB1420Globals {
    uint32_t dataTables;   // *sgptDataTables (the struct, not the slot)
    uint32_t bitMasks;     // &gdwBitMasks
    uint32_t cbTable;      // &DAT 0xDDA10 (dword per row callback; 0 = none)
    int32_t  cbCount;      // DAT 0xDDB54
};

// 1.13c D2Client+0xB1420
extern "C" uint32_t d2_client_b1420(uint32_t self, uint32_t target, uint32_t force, const D2cB1420Globals* g, D2cHost h) {
    uint32_t row = 0;
    if (self) {
        const int32_t id = (int32_t)rd32(self + 4);
        if (id >= 0 && id < (int32_t)rd32(g->dataTables + 0xB6C)) row = rd32(g->dataTables + 0xB64) + (uint32_t)id * 0x1A4;
    }
    const int32_t cls = rd16s(row + 0x16);
    uint32_t flags = 3, result = 0;
    const uint32_t owner = call(h, 0xA60D0, self, 0, 0, 0, self);             // EAX = unit; ESI = unit
    if (!self) return 0;
    if (target) {
        if (rd8(row + 0x188) && call(h, 0xC1FC, 0, 0, 0, owner, self, target, 0x56)) return 0;   // #10494
        if (call(h, 0xC484, 0, 0, 0, owner, self, self, target)) return 0;                       // #10227
        call(h, 0xC3A0, 0, 0, 0, owner, self, self, target);                                     // #10024
        if (!((rd32(target + 0xC4) >> 2) & 1)) return 0;
        if (owner) {
            if (!call(h, 0xA62A0, target, 0, 0, owner, self, owner) && !rd8(row + 0x187)) return 0;
        }
    }
    if (owner && target) {
        if ((rd8(g->bitMasks + 8) & rd8(row + 4)) && (int32_t)rd32(owner) >= 0 && (int32_t)rd32(owner) <= 1) {
            if (call(h, 0xC1FC, 0, 0, 0, owner, self, owner, 0x45) || call(h, 0xC454, 0, 0, 0, owner, self, owner, 0x9C, 0))
                flags = call(h, 0xB0100, 0, 0, 0, owner, self);
        }
    }
    if (target || force || rd8(row + 0x18C)) {
        if (flags & 2) {
            if (rd8(row + 0x188) && target) {
                const uint32_t o = call(h, 0xC442, 0, 0, 0, owner, self, 0, 2, rd8(row + 0x189), rd32(target), rd32(target + 0xC));  // #11013
                if (o) {
                    call(h, 0xC2F2, 0, 0, 0, o, self, o, 0x56);            // #10785
                    call(h, 0xC2BC, 0, 0, 0, o, self, o, 0x6FB5FF70u);     // #10911(o, the client callback at 0xAFF70)
                    call(h, 0xC36A, 0, 0, 0, o, self, target, o, 1);       // #10807
                    call(h, 0xC30A, 0, 0, 0, o, self, target, 0x56, 1);    // #10945
                }
            }
            call(h, 0x21400, 0, 0, 0, 0, self, self, 0);
            const int32_t sk = rd16s(row + 0xA);
            if (sk > 0 && sk < g->cbCount) {
                const uint32_t fn = rd32(g->cbTable + (uint32_t)sk * 4);
                if (fn && !call(h, fn, fn, self, target, 0, self)) return 0;   // the callback slot's value names the target
            }
            const int32_t snd = rd16s(row + 0x14);
            if (snd >= 0) call(h, 0x88A70, 0, 0, 0, (uint32_t)snd, self, self, 0, 0, 0);
            if (rd16s(row + 0x16) >= 0 && cls != -1) {
                const uint32_t ry = call(h, 0xC3FA, 0, 0, 0, (uint32_t)cls, self, self);           // #11029
                const uint32_t rx = call(h, 0xC388, 0, 0, 0, (uint32_t)cls, self, self);           // #10240
                const uint32_t nu = call(h, 0xB0450, rx, 0, 0, (uint32_t)cls, self, (uint32_t)cls, 0, 0, rx, ry);
                result = nu;
                if (nu) {
                    const uint32_t z = call(h, 0xC7EC0, self, 0, 0, nu, self);
                    const uint32_t y = call(h, 0xC7EE0, self, z, 0, nu, self);
                    const uint32_t x = call(h, 0xC7F00, self, z, y, nu, self);
                    call(h, 0xC7E30, nu, z, y, nu, self, x);
                    call(h, 0xC7C30, nu, z, y, nu, self);
                    call(h, 0xC7BF0, nu, z, y, nu, self);
                    // EAX at #10425: the path on the 0x92 branch (`mov eax,[ebx+0x2c]`), else #10042's return
                    const uint32_t path = rd32(nu + 0x2C);
                    if (rd32(nu + 4) == 0x92) {
                        const uint32_t a = call(h, 0x1330, 0x40, nu + 0x20, 0, nu, self);
                        call(h, 0xC2B6, path, nu + 0x20, 0, nu, self, path, a);                        // #10425
                    } else {
                        const uint32_t a = call(h, 0xC2D4, 0, rd32(self + 0x2C), 0, nu, self, rd32(self + 0x2C));   // #10042
                        call(h, 0xC2B6, a, rd32(self + 0x2C), path, nu, self, path, a);                // #10425
                    }
                }
            }
        }
    }
    if (target && !rd8(row + 0x186)) return 0;
    const int32_t trav = rd16s(row + 0x12);
    if (trav >= 0 && call(h, 0x215D0, (uint32_t)trav, self, 0, 0, self)) call(h, 0x88C20, 0, self, 0, 0, self, self, 0);
    uint32_t eax = call(h, 0xC3D6, 0, 0, 0, 0, self, self);                                              // #10398
    if (eax) eax = call(h, 0xA85F0, 0, 0, 0, 0, self);
    if (flags & 1) {
        if ((rd32(self + 0xC4) >> 21) & 1) call(h, 0xA7340, eax, rd32(self), rd32(self + 0xC), 0, rd32(self + 0xC));
        else call(h, 0xA5B00, eax, rd32(self), rd32(self + 0xC), 0, rd32(self + 0xC));
    }
    return result;
}
