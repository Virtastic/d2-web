// 1.13c D2Client+0x34640 (800 B, ECX = this, EDX = p2, stack s0..s4, ret 0x14): damage-range tooltip line. p2 == 0 or s0 == 0 returns.
// 0x32F30(this, &X14; &X10, &X1c, 0, 0, p2, 0, 0) fills X14, X10, X1c; esi = 0x14A0(byte[s0+0x1A5], 0x80, X14), edi = 0x14A0(.., 0x80, X10).
// X18, X10, X14 are cleared, then 0x2E190(eax = s0, ebx = s1, ecx = &X18, edx = &X14; this, &X18, &X10, &X14) fills C = X18, A = X10, B = X14.
// esi == edi == 0: 0x2F0A0(A, C; B, s2, s3, s4). Else C == A == 0: 0x2F0A0(edi, esi; X1c, s2, s3, s4).
// Else text1 = pair(esi, edi), text2 = pair(C, A) (d2client_pairfmt.h) and 0x2D3C0(text1, text2, s2, s3, s4, X1c, B).
#include <cstdint>
#include "d2client_pairfmt.h"
namespace d2client_34640 {
using U = uint32_t; using I = int32_t;
struct Env {
    void* c;
    void (*f32f30)(void*, U self, U* x14, U* x10, U* x1c, U p2);
    I (*f14a0)(void*, U byte, U val);
    void (*fe190)(void*, const uint8_t* s0, U s1, U self, U* x18, U* x10, U* x14);   // 0x2E190: EAX = s0, EBX = s1
    void (*f2f0a0)(void*, U a, U c, U w0, U s2, U s3, U s4);
    void (*f2d3c0)(void*, const uint16_t* t1, const uint16_t* t2, U s2, U s3, U s4, U x1c, U b);
};
void sub_34640(const Env& e, U self, U p2, const uint8_t* s0, U s1, U s2, U s3, U s4) {
    if (!p2 || !s0) return;
    const U byte = s0[0x1A5];
    U x10 = 0, x14 = 0, x18, x1c = 0;
    e.f32f30(e.c, self, &x14, &x10, &x1c, p2);
    const I esi = e.f14a0(e.c, byte, x14);
    const I edi = e.f14a0(e.c, byte, x10);
    x18 = x10 = x14 = 0;
    e.fe190(e.c, s0, s1, self, &x18, &x10, &x14);
    if (esi == 0 && edi == 0) { e.f2f0a0(e.c, x10, x18, x14, s2, s3, s4); return; }
    if (x18 == 0 && x10 == 0) { e.f2f0a0(e.c, (U)edi, (U)esi, x1c, s2, s3, s4); return; }
    uint16_t t1[32] = {}, t2[32] = {};
    d2client_pairfmt::text(t1, esi, edi);
    d2client_pairfmt::text(t2, (I)x18, (I)x10);
    e.f2d3c0(e.c, t1, t2, s2, s3, s4, x1c, x14);
}
}
