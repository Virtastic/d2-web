// D2Client+0xC9FF7: MSVC CRT x87 fdiv-by-register dispatcher (selector = EAX & 0x3f, family of
// __fdiv/__fdivr _sti_st stubs). Per slot k=(sel>>3) it picks which two FP-stack operands feed the
// 0xC9EE0 classifier. Decompile shows fxch as swi(6) fall-through; modelled as swap(ST0,STk).
// UNVERIFIED: ordinals of the r=1,3 fxch cases and the FP result register (no capture).
#include <cstdint>
struct X80 { uint32_t lo, hi; uint16_t se; };
uint32_t sub_C9EE0_ext(const X80& a, const X80& b); // 1.13c D2Client+0xC9EE0 (d2client_batch_c88a0.cpp)

// 1.13c D2Client+0xC9FF7
// st[0..7] = FP stack; returns the classifier result, or 0 for the k==0 (x/x) cases that skip it.
uint32_t d2client_fdiv_dispatch_c9ff7(uint32_t sel, X80 st[8])
{
    sel &= 0x3f;
    const uint32_t k = sel >> 3, r = sel & 7;
    if (k == 0) return 0;                       // cases 0..7: ST0/ST0 or return STk, no callee
    if (r == 1 || r == 3) { X80 t = st[0]; st[0] = st[k]; st[k] = t; }   // fxch
    switch (r) {
    case 0: case 6: case 7: return sub_C9EE0_ext(st[k], st[0]);
    case 5: case 4: case 3: case 2: case 1: return sub_C9EE0_ext(st[0], st[k]);
    }
    return 0;
}
