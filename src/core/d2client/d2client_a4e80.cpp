// 1.13c D2Client+0xA4E80, written from the disassembly: copy the record node of (type, id) onto the unit u (ECX), then step
// u's mode toward the node's mode. ECX = u, no stack arguments, plain ret, returns nothing. gl = the dword at D2Client+0xCC394
// (a non-zero value is the Fog assert 0xBAA then _exit). Callees (all stdcall imports unless noted): 0xD04E(type, id) node,
// 0xC1F0(u), 0xC208(x), 0x1400() (plain function), 0xC688(D, u, W, w2, w6), 0xC6BE(S), 0xC670(D, v), 0xC310(node), 0xC628(x),
// 0xC60A(x, flags), 0xC43C(u, mode), 0xC646(u, 0). S = node+0x2c (source data), D = u+0x2c (destination data).
#include "cl_env.h"
extern "C" void d2cl_a4e80(const cl::Env& e, uint32_t u) {
    const auto C32 = [&](uint32_t D, uint32_t S, uint32_t off) { e.W(D + off, e.R(S + off)); };
    const auto C8 = [&](uint32_t D, uint32_t S, uint32_t off) { e.W(D + off, (e.R(D + off) & 0xffffff00u) | (e.R(S + off) & 0xffu)); };
    if (e.R(e.gl)) { const uint32_t m = e.H(0xbf92, 0xbaa); e.H(0xbf80, 0xB2000000u | 0xd359f, m); e.H(0x3664, 0xffffffffu); return; }
    uint32_t ty = 6, id = 0xffffffffu;
    if (u) { ty = e.R(u); id = e.R(u + 0xc); }
    const uint32_t A = e.H(0xd04e, ty, id);
    if (!A) return;
    const uint32_t S = e.R(A + 0x2c), D = e.R(u + 0x2c);
    const uint32_t W = e.H(0xc1f0, u);
    if (!W) return;
    const uint32_t w2 = e.R(S + 2) & 0xffffu, w6 = e.R(S + 6) & 0xffffu;
    if (w2 != (e.R(D + 2) & 0xffffu) || w6 != (e.R(D + 6) & 0xffffu)) {
        if (!e.H(0x1400)) return;
        e.H(0xc688, D, u, W, w2, w6);
    }
    static const uint32_t d32a[] = {0x0, 0x4, 0x8, 0xc, 0x24, 0x28, 0x38, 0x10, 0x14, 0x18, 0x5c, 0x60};
    for (uint32_t o : d32a) C32(D, S, o);
    C8(D, S, 0x67); C8(D, S, 0x68);
    static const uint32_t d32b[] = {0x7c, 0x80, 0x84, 0x88, 0x8c};
    for (uint32_t o : d32b) C32(D, S, o);
    C8(D, S, 0x91); C8(D, S, 0x92); C8(D, S, 0x93); C8(D, S, 0x94);
    C32(D, S, 0x98); C32(D, S, 0x6a); C32(D, S, 0x6e);
    for (uint32_t n = e.R(S + 0x28), i = 0; i < n; ++i) C32(D, S, 0x9c + 4 * i);
    for (uint32_t n = e.R(S + 0x1d4), i = 0; i < n; ++i) C32(D, S, 0x1d8 + 4 * i);
    C32(D, S, 0x1d4);
    uint32_t v = e.R(S + 0x3c);
    if (v != 8 && v != 0xb) e.W(D + 0x3c, v);
    v = e.R(S + 0x40);
    if (v != 8 && v != 0xb) e.W(D + 0x40, v);
    C32(D, S, 0x40); C32(D, S, 0x44);
    e.H(0xc670, D, e.H(0xc6be, S));
    const uint32_t U4 = e.R(u + 0x10), A4 = e.R(A + 0x10);
    if (U4 != A4) {
        uint32_t mode = 0xffffffffu;   // 0xffffffff = no step
        if (e.R(u) == 0) {
            const uint32_t k = A4 - 1;
            const uint32_t tab = k > 0x11 ? 4 : (A4 == 1 || A4 == 5) ? 0 : (A4 == 2 || A4 == 6) ? 1 : A4 == 3 ? 2 : A4 == 0x12 ? 3 : 4;
            if (tab == 0) {
                if (U4 >= 2 && (U4 <= 3 || U4 == 6)) mode = e.H(0xc208, e.H(0xc1f0, u)) ? 5 : 1;
            } else if (tab == 1) {
                if (U4 == 1 || U4 == 5) mode = e.H(0xc208, e.H(0xc1f0, u)) ? 6 : 2;
            } else if (tab == 2) {
                if (U4 == 1 || U4 == 3 || U4 == 5) mode = 3;
            } else if (tab == 4) {
                if (U4 == 0x13) mode = A4;
                else {
                    const uint32_t edi = e.H(0xc310, u), esi = e.H(0xc310, A);
                    if (!edi) goto done;
                    if (esi && (e.H(0xc628, esi) & 1) && !(e.H(0xc628, esi) & 2)) goto done;
                    const uint32_t f = e.H(0xc628, edi);
                    if (!(f & 1)) goto done;
                    e.H(0xc60a, edi, f | 2);
                    e.H(0xc646, u, 0);
                    return;
                }
            }
        } else if (e.R(u) == 1) {
            if (A4 == 1) { if (U4 == 2 || U4 == 0xf) mode = 1; }
            else if (A4 == 2) { if (U4 == 1) mode = 2; }
            else if (A4 == 0xf) { if (U4 == 1) mode = 0xf; }
        }
        if (mode != 0xffffffffu) e.H(0xc43c, u, mode);
    }
done:
    e.H(0xc646, u, 0);
}
