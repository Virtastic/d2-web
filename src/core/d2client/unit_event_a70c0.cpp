// 1.13c D2Client+0xA70C0 (500 B): EAX = event id, ECX = unit (ESI). Dispatch (retail jump table 0xA72B4 + index bytes 0xA72D0 for 0xF2..0x157):
//   0xF2 calls 0x6B300 (EAX 0x7A, k 3); 0x14D (0xBA, k 2); 0x154..0x157 call 0xC7B00 (EAX unit) then 0xC7E30 (EAX unit, ECX 0, EDX x, stack y) with (x,y) =
//   (-0x20,-0x20) (0x20,-0x20) (0x20,0x20) (-0x20,0x20); 0x15F..0x161 call 0x6B300 (EAX = id - 0xB3, k 4, amount id - 0xB0, id = unit ? unit[4] : -1);
//   0x196 calls 0xC7B00, 0xC7E30 (ECX -0x37, EDX 0, stack 0), then writes the sub-struct unit[0x54] [0x30] (+0x10 0, +0x14 0, +0x18 0x800; +0x28 0, +0x2C 0, +0x30 0x32; |= 0x20);
//   0x1A1..0x1A2 with a unit call (0x10E, k 2); 0x209 tail-calls 0x53320 (EAX unit); 0x21F (0x113, k 3); 0x2C0 (0x7A, k 3). Every 0x6B300 call pushes k, 0, amount, 0, 0, 0 under the unit.
#include <cstdint>
#include <emscripten.h>

static inline uint32_t R(uintptr_t a) { return *(const uint32_t*)a; }
static inline void Wr(uintptr_t a, uint32_t v) { *(uint32_t*)a = v; }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a70c0(int32_t ev, uintptr_t unit, void (*f1b300)(int32_t eax, uintptr_t unit, uint32_t k, uint32_t z0, int32_t amt),
                                                     void (*fc7b00)(uintptr_t eax), void (*fc7e30)(uintptr_t eax, int32_t ecx, int32_t edx, int32_t stk), void (*f53320)(uintptr_t eax)) {
    if (ev > 0x161) {
        if (ev > 0x209) {
            if (ev == 0x21f) f1b300(0x113, unit, 3, 0, 0);
            else if (ev == 0x2c0) f1b300(0x7a, unit, 3, 0, 0);
            return;
        }
        if (ev == 0x209) { f53320(unit); return; }
        if (ev == 0x196) {
            fc7b00(unit);
            fc7e30(unit, -0x37, 0, 0);
            if (!unit) return;
            uintptr_t s = R(unit + 0x54), d = s ? R(s + 0x30) : 0;
            if (d) { Wr(d + 0x10, 0); Wr(d + 0x14, 0); Wr(d + 0x18, 0x800); }
            s = R(unit + 0x54); d = s ? R(s + 0x30) : 0;
            if (d) { Wr(d + 0x28, 0); Wr(d + 0x2c, 0); Wr(d + 0x30, 0x32); }
            s = R(unit + 0x54); d = s ? R(s + 0x30) : 0;
            if (d) Wr(d, R(d) | 0x20);
            return;
        }
        if (ev > 0x1a0 && ev <= 0x1a2 && unit) f1b300(0x10e, unit, 2, 0, 0);
        return;
    }
    if (ev >= 0x15f) {
        int32_t id = unit ? (int32_t)R(unit + 4) : -1;
        f1b300(id - 0xb3, unit, 4, 0, id - 0xb0);
        return;
    }
    uint32_t i = (uint32_t)ev - 0xf2;
    if (i > 0x65) return;
    if (i == 0) { f1b300(0x7a, unit, 3, 0, 0); return; }
    if (i == 0x5b) { f1b300(0xba, unit, 2, 0, 0); return; }
    if (i >= 0x62 && i <= 0x65) {
        static const int8_t X[4] = {-0x20, 0x20, 0x20, -0x20}, Y[4] = {-0x20, -0x20, 0x20, 0x20};
        fc7b00(unit);
        fc7e30(unit, 0, X[i - 0x62], Y[i - 0x62]);
    }
}
