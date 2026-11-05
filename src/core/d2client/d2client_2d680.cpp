// 1.13c D2Client+0x2D680 (59 B, ECX = unit, EDX = p2, stack s0..s5, ret 0x18): eax := D2Common 10061([unit+0x60]) when that
// word is nonzero, else 0; *s2 := 0, *s3 := 0; returns 0x2C4F0(EAX = eax, ESI = unit; s2, s3, s1, p2).
#include <cstdint>
#include <cstring>
struct D2Client2D680Env {
    uint32_t (*ord10061)(uint32_t v);   // thunk 0xCA0C, stdcall 1
    uint32_t (*f2c4f0)(uint32_t eax, const uint8_t* unit, uint32_t* s2, uint32_t* s3, uint32_t s1, uint32_t p2);
};
extern "C" uint32_t d2c_port_2d680(const uint8_t* unit, uint32_t p2, uint32_t s1, uint32_t* s2, uint32_t* s3, const D2Client2D680Env& e) {
    uint32_t v; memcpy(&v, unit + 0x60, 4);
    uint32_t a = v ? e.ord10061(v) : 0;
    *s2 = 0; *s3 = 0;
    return e.f2c4f0(a, unit, s2, s3, s1, p2);
}
