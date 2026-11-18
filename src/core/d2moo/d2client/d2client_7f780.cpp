// 1.13c D2Client+0x7F780 -- speech/chat packet handler. Stack arg: the packet (+1 type, +2 byte, +3 flag, +4 unit id, +8 byte, +10 name[16], +0x1A text).
// Gate: flag != 0, else proceed when 0x9D630(EDX = id) == 0 or 0x9DD80() == 0. Six wide buffers (A 0x100, B and C 0x264, D and E 0x10, W 0x100) are cleared
// by the D2Lang char constructor (import slot 0xCEB00, one call per char; the port clears them directly, the slot is left real in the capture).
// 0x7B4F0(EAX = A, EDX = text, EDI = byte +2) must return a non-zero low byte. Type 4: 0x7D850(A, byte +8). Otherwise 0xA4E20(EAX = id & 0x7F,
// ECX = id, EDX = table 0x10A608 + flag * 0x200, flag) gives the unit (only type 5 uses it). Type 6: widen text to C (0xCEAC0, 0x100) and name to D (0x10),
// sprintf(0x264, B, str 0xE4A, D, C), 0xBE760(B), 0x7D850(B, 2). Type 2 and type 1 (flag not 0 or 1) split on the name length (1..15) and the text length
// (<= 0x131): a valid name builds B from the widened name, the D2Lang strings 0xE46 (type 2) or 0xFD0 (type 1, bracketed by 0xBF5F0(4) and 0xBF5F0(0) with EDI = the
// string being coloured) and A; otherwise B is a copy of A; then 0x7D850(B, 2) for type 2, 0x7D850(B, byte +8) for type 1; type 1 with flag 0 or 1 calls 0x7D850(A, byte +8).
// Type 5 with a unit: 0x7B470(EDX = text, byte +2). Type 7: MultiByteToWideChar(0, 1, text, 0x100, W, 0x100) (slot 0xCEF8C), 0xD1E6(), 0x97A70(EDX = W, byte +8).
#include <cstring>
#include "cl_env.h"
extern "C" void d2c_7f780(uint32_t pkt, const cl::Env& e) {
    uint16_t A[0x100], B[0x264], C[0x264], D[0x10], E[0x10], W[0x100];
    const uint32_t a = (uint32_t)(uintptr_t)A, b = (uint32_t)(uintptr_t)B, c = (uint32_t)(uintptr_t)C, d = (uint32_t)(uintptr_t)D, ee = (uint32_t)(uintptr_t)E, w = (uint32_t)(uintptr_t)W;
    const uint32_t ty = e.R8(pkt + 1), p2 = e.R8(pkt + 2), fl = e.R8(pkt + 3), id = e.R(pkt + 4), p8 = e.R8(pkt + 8), name = pkt + 10, text = pkt + 0x1a;
    if (fl == 0 && e.H(0x9d630, 0, 0, 0, 0, 0, 0, 0, id) && e.H(0x9dd80)) return;
    memset(A, 0, sizeof A);
    const uint32_t ok = e.H(0x7b4f0, 0, 0, 0, 0, 0, a, 0, text);
    e.Ej(0x7b4f0, p2);
    if (!(ok & 0xff)) return;
    A[0xff] = 0;
    if (ty == 4) { e.H(0x7d850, a, p8); return; }
    const uint32_t unit = e.H(0xa4e20, fl, 0, 0, 0, 0, id & 0x7f, id, 0xB2000000u | (0x10A608 + fl * 0x200));
    memset(B, 0, sizeof B); memset(C, 0, sizeof C); memset(D, 0, sizeof D);
    auto slen = [&](uint32_t p) { int32_t n = 0; while (e.R8(p + n)) n++; return n; };
    if (ty == 6) {
        e.H(0xceac0, 0x100, 0, 0, 0, 0, 0, c, text);
        e.H(0xceac0, 0x10, 0, 0, 0, 0, 0, d, name);
        uint32_t f = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0xe4a);
        e.H(0xceab4, 0x264, b, f, d, c);
        e.H(0xbe760, b);
        e.H(0x7d850, b, 2);
        return;
    }
    if (ty == 2) {
        int32_t nl = slen(name), tl = slen(text);
        if (nl > 0 && nl < 0x10) {
            e.H(0xceac0, 0x10, 0, 0, 0, 0, 0, d, name);
            e.H(0xceab8, 0, 0, 0, 0, 0, 0, b, d);
            uint32_t f = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0xe46);
            e.H(0xceabc, 0, 0, 0, 0, 0, 0, b, f);
            e.H(0xceab8, 0, 0, 0, 0, 0, 0, c, a);
            e.H(0xceabc, 0, 0, 0, 0, 0, 0, b, c);
            e.H(0x7d850, b, 2);
            return;
        }
        if (tl > 0x131) return;
        e.H(0xceab8, 0, 0, 0, 0, 0, 0, b, a);
        e.H(0x7d850, b, 2);
        return;
    }
    if (ty == 1) {
        if (fl == 0 || fl == 1) { e.H(0x7d850, a, p8); return; }
        int32_t nl = slen(name), tl = slen(text);
        if (nl > 0 && nl < 0x10) {
            memset(E, 0, sizeof E);
            e.H(0xceac0, 0x10, 0, 0, 0, 0, 0, ee, name);
            e.H(0xceab8, 0, 0, 0, 0, 0, 0, b, ee);
            e.H(0xbf5f0, 4); e.Ej(0xbf5f0, b);
            uint32_t f = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0xfd0);
            e.H(0xceab8, 0, 0, 0, 0, 0, 0, c, f);
            e.H(0xceabc, 0, 0, 0, 0, 0, 0, c, a);
            e.H(0xbf5f0, 0); e.Ej(0xbf5f0, c);
            e.H(0xceabc, 0, 0, 0, 0, 0, 0, b, c);
            e.H(0x7d850, b, p8);
            return;
        }
        if (tl > 0x131) return;
        e.H(0xceab8, 0, 0, 0, 0, 0, 0, b, a);
        e.H(0x7d850, b, p8);
        return;
    }
    if (ty == 5) {
        if (unit) e.H(0x7b470, p2, 0, 0, 0, 0, 0, 0, text);
        return;
    }
    if (ty == 7) {
        memset(W, 0, sizeof W);
        e.H(0xcef8c, 0, 1, text, 0x100, w, 0, 0, 0, 0, 0);
        (void)e.H(0xd1e6);
        e.H(0x97a70, p8, 0, 0, 0, 0, 0, 0, w);
    }
}
