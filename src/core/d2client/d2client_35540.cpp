// 1.13c D2Client+0x35540 (853 B, ECX = this, EDX = p2, stack s0..s4, ret 0x14): damage-range tooltip line with the equipment bonus.
// p2 == 0 or s0 == 0 returns. 0x34D70(eax = &X18, ecx = 0, edx = p2; this, &X20, &X1C) fills X18, X20, X1C; X24 = 0x2C830(byte[s0+0x1DC]).
// With id = (int16)s0[0] and the four thunks f(this, id, s1, flag): X14 = (C85C(.., 0) >> 8) + (CEC2(.., 1) >> 8), X10 = (C7F0(.., 0) >> 8) + (CECE(.., 1) >> 8)
// (arithmetic shifts; C85C, C7F0, CEC2, CECE called in that order). 0x2D6E0(al = byte[s0+0x1DC], esi = this, ebx = &X14, edi = &X10) may rewrite both.
// X20 == X1C == 0: 0x2F0A0(X10, X14; X24, s2, s3, s4). Else X14 == X10 == 0: 0x2F0A0(X1C, X20; X18, s2, s3, s4).
// Else text1 = pair(X20, X1C), text2 = pair(X14, X10) (d2client_pairfmt.h) and 0x2D3C0(text1, text2, s2, s3, s4, X18, X24).
#include <cstdint>
#include "d2client_pairfmt.h"
namespace d2client_35540 {
using U = uint32_t; using I = int32_t;
struct Env {
    void* c;
    void (*f34d70)(void*, U self, U p2, U* x18, U* x20, U* x1c);
    U (*f2c830)(void*, U byte);
    I (*thunk)(void*, int which /* 0 C85C, 1 C7F0, 2 CEC2, 3 CECE */, U self, I id, U s1);
    void (*f2d6e0)(void*, U eax, U self, U* x14, U* x10);
    void (*f2f0a0)(void*, U a, U c, U w0, U s2, U s3, U s4);
    void (*f2d3c0)(void*, const uint16_t* t1, const uint16_t* t2, U s2, U s3, U s4, U x18, U x24);
};
void sub_35540(const Env& e, U self, U p2, const uint8_t* s0, U s1, U s2, U s3, U s4) {
    if (!p2 || !s0) return;
    U x18 = 0, x1c = 0, x20 = 0;
    e.f34d70(e.c, self, p2, &x18, &x20, &x1c);
    const U b1dc = s0[0x1DC];
    const U x24 = e.f2c830(e.c, b1dc);
    const I id = (int16_t)(uint16_t)(s0[0] | s0[1] << 8);
    const I a = e.thunk(e.c, 0, self, id, s1) >> 8;
    const I b = e.thunk(e.c, 1, self, id, s1) >> 8;
    const I c = e.thunk(e.c, 2, self, id, s1) >> 8;
    const I d = e.thunk(e.c, 3, self, id, s1) >> 8;
    U x14 = (U)a + (U)c, x10 = (U)b + (U)d;
    e.f2d6e0(e.c, ((U)d & ~0xFFu) | b1dc, self, &x14, &x10);   // EAX = the last shifted answer with its low byte replaced
    const I esi = (I)x20, edi = (I)x1c;
    if (esi == 0 && edi == 0) { e.f2f0a0(e.c, x10, x14, x24, s2, s3, s4); return; }
    if (x14 == 0 && x10 == 0) { e.f2f0a0(e.c, (U)edi, (U)esi, x18, s2, s3, s4); return; }
    uint16_t t1[32] = {}, t2[32] = {};
    d2client_pairfmt::text(t1, esi, edi);
    d2client_pairfmt::text(t2, (I)x14, (I)x10);
    e.f2d3c0(e.c, t1, t2, s2, s3, s4, x18, x24);
}
}
