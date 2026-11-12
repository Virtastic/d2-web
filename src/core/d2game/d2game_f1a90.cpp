// 1.13c D2Game+0xF1A90 -- ECX = a, EDX = b, stack: P, out (ret 8). A null P or P with a nonzero type word returns 0. d = 0xB0A20(EBX = b, stack P) above 0xF (signed) returns 0. With the table check 0x15D0 (EAX = b +4 word or -1, EDX = 9) answering 0, out+4 = d, out+0 = P and P is returned; otherwise the test 0x34EA0 (EDI = P, stack a, b) must answer nonzero and d below out+4 (signed) for the same stores, else 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t ff1a90(GE& e, uint32_t a, uint32_t b, uint32_t P, uint32_t out)
{
    if (!P || e.rd32(P) != 0) return 0;
    const uint32_t d = e.h(0xB0A20, 0, 0, 0, b, 0, P);
    if ((int32_t)d > 0xf) return 0;
    const uint32_t k = b ? e.rd32(b + 4) : 0xffffffffu;
    if (e.h(0x15D0, k, 0, 9, 0, 0) == 0) {
        e.wr32(out + 4, d); e.wr32(out, P);
        return P;
    }
    const uint32_t t = e.h(0x34EA0, 0, 0, 0, 0, 0, a, b);
    e.j(0x34EA0, P);
    if (t == 0) return 0;
    if ((int32_t)d < (int32_t)e.rd32(out + 4)) {
        e.wr32(out + 4, d); e.wr32(out, P);
        return P;
    }
    return 0;
}
}
