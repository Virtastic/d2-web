// 1.13c D2Client+0xA62A0, written from the disassembly: the relation of two (type, id) records. EAX = A, stack word 0 = B, ret 4.
// Record: +0 type (6 when the pointer is null), +0xC id (-1 when null), +4 / +0x10 / +0x94 / +0x98 / +0xC4 / +0xC8 read below.
// gl = the dword at D2Client+0x11C4D4 (head of the remap list: node +8 = id, +0xC = mapped id or -1, +0x30 next).
// Register callees: 0x9DC30 (EAX = idB, EDX = idA), 0x216A0 (ECX), 0xA51F0 / 0xA5440 (EAX), 0x1510 (EAX, EDX), 0x4FD90 (EDX, stack 8).
// Retail dereferences A+0x94 with A unchecked when B+0xC8 has bit 10; the vectors keep A non-null there.
#include "cl_env.h"
extern "C" uint32_t d2cl_a62a0(const cl::Env& e, uint32_t A, uint32_t B) {
    uint32_t tB = 6, idB = 0xFFFFFFFFu, tA = 6, idA = 0xFFFFFFFFu;
    if (B) { tB = e.R(B); idB = e.R(B + 0xc); }
    if (A) { tA = e.R(A); idA = e.R(A + 0xc); }
    const uint32_t r = e.H(0xc1f0, B);
    if (e.H(0xc208, r)) {
        if (A && e.R(A) == 0) {
            bool call = false;
            if (e.R(A + 0xc4) & 0x10000) call = true;
            else { const uint32_t t = e.R(A + 0x10); call = t == 0 || t == 0x11; }
            if (call && !e.H(0x9dc30, 0, 0, 0, 0, 0, idB, 0, idA)) return 0;
        }
        if (!B || e.R(B) != 0 || !A || e.R(A) != 1) return 1;
        if (e.H(0x216a0, 0, 0, 0, 0, 0, 0, idA) == 0xFFFFFFFFu) return 1;
        return e.H(0xa51f0, 0, 0, 0, 0, 0, B) ? 1 : 0;
    }
    const uint32_t G = e.R(e.gl);
    if (tB == 1) {
        for (uint32_t n = G; n; n = e.R(n + 0x30))
            if (e.R(n + 8) == idB) { if (e.R(n + 0xc) != 0xFFFFFFFFu) { tB = 0; idB = e.R(n + 0xc); } break; }
        if (B && ((e.R(B + 0xc8) >> 10) & 1) && e.R(A + 0x94) == 0) { idB = e.R(B + 0x98); tB = 0; }
    }
    if (tA == 1) {
        for (uint32_t n = G; n; n = e.R(n + 0x30))
            if (e.R(n + 8) == idA) { if (e.R(n + 0xc) != 0xFFFFFFFFu) { tA = 0; idA = e.R(n + 0xc); } break; }
        if (A && ((e.R(A + 0xc8) >> 10) & 1) && e.R(A + 0x94) == 0) { idA = e.R(A + 0x98); tA = 0; }
    }
    if (tB == tA && idB == idA) return 0;
    if (tB == 0 && tA == 0) {
        if (A && e.R(A) == 0 && e.H(0xa5440, 0, 0, 0, 0, 0, A) && e.H(0x9dc30, 0, 0, 0, 0, 0, idB, 0, e.R(A + 0xc))) return 1;
        return e.H(0x4fd90, 8, 0, 0, 0, 0, 0, 0, idB);
    }
    if (tA == 1) {
        const uint32_t x = A ? e.R(A + 4) : 0xFFFFFFFFu;
        if (e.H(0x1510, 0, 0, 0, 0, 0, x, 0, 4)) return 1;
        if (e.H(0x1510, 0, 0, 0, 0, 0, x, 0, 5)) return 0;
    }
    if (tB == 0 && (tA == 4 || tA == 2)) return 1;
    return e.H(0xc6a6, B, A) == 0 ? 1 : 0;
}
