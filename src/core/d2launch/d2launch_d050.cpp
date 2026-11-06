// 1.13c D2Launch+0xD050  sub_D050: EAX = a string (kept in EDI for the callees). Clears 0x25EE4. Mode 0x25AD4 == 1: 996A and 98CE(EDX = the
// string, 0) on the control at 0x25AA8, then CD70, A8C0 and a tail jump to B600. Modes 0, 2, 3: AE60, then C3F0 when 980E returns 0, then
// CC50; every mode but 1 ends in A8C0 and the B600 tail jump (B600's EAX is the result). g is the module data range from 0x24DB0.
#include <cstdint>
#include "d2launch_env.h"
extern "C" uint32_t d2l_d050(const D2lSeams* s, uint32_t* g, uint32_t eax) {
    auto W = [&](uint32_t rva) -> uint32_t& { return g[(rva - 0x24DB0u) / 4]; };
    const uint32_t m = W(0x25AD4);
    W(0x25EE4) = 0;
    if (m == 1) {
        s->t996a(W(0x25AA8));
        s->t98ce(W(0x25AA8), eax, 0);
        s->cd70();
        s->a8c0();
        return s->b600();
    }
    if (m == 0 || m == 3 || m == 2) {
        s->ae60();
        if (s->t980e() == 0) s->c3f0();
        s->cc50();
    }
    s->a8c0();
    return s->b600();
}
