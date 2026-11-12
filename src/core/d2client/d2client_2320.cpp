// 1.13c D2Client+0x2320: release every chunk of a hash container, EDI = container. While the head at +0x40 is a positive word:
// 0x2750(head) then the free thunk 0xBE6C(head, file string, -2, 0); then the same over the head at +0x30 with 0x21D0. The callees
// advance the heads. EAX is the last free's result, or the entry EAX when both lists are empty.
#include "d2c_seam.h"
extern "C" uint32_t d2c_2320(const D2cSeam* s, const uint8_t* t, uint32_t eax) {
    auto head = [&](uint32_t o) { int32_t v; std::memcpy(&v, t + o, 4); return v; };
    for (int32_t n; (n = head(0x40)) > 0;) {
        (*s)(0x2750, 0, 0, 0, 0, 0, (uint32_t)n);
        eax = (uint32_t)(*s)(0xbe6c, 0, 0, 0, 0, 0, (uint32_t)n, s->mod(0xf9ed0), 0xfffffffeu, 0);
    }
    for (int32_t n; (n = head(0x30)) > 0;) {
        (*s)(0x21d0, 0, 0, 0, 0, 0, (uint32_t)n);
        eax = (uint32_t)(*s)(0xbe6c, 0, 0, 0, 0, 0, (uint32_t)n, s->mod(0xf9f48), 0xfffffffeu, 0);
    }
    return eax;
}
