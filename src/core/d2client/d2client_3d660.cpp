// 1.13c D2Client+0x3D660: walk the list headed by the global at D2Client+0x11BBFC. The head comes from 0xCE98(global); a null head is retail's fatal
// assert (0xBF92(0x47D), 0xBF80(fmt, rc), 0x3664(-1)). For each node: edi = 0xC640(ECX = node); when (byte at *(D2Client+0xCEE50)+0x10) & (edi byte +8)
// is set, 0x3D320(edi, 0xCE9E(node)); the next node is 0xC75A(node).
#include "d2c_seam.h"
extern "C" void d2c_3d660(const D2cSeam* s, uint32_t head, uint8_t flags) {
    uint32_t node = (uint32_t)(*s)(0xce98, 0, 0, 0, 0, 0, head);
    if (!node) {
        const uint64_t rc = (*s)(0xbf92, 0, 0, 0, 0, 0, 0x47d);
        (*s)(0xbf80, 0, 0, 0, 0, 0, s->mod(0xD359F), rc);
        (*s)(0x3664, 0, 0, 0, 0, 0, 0xffffffffu);
        return;
    }
    do {
        const uint32_t e = (uint32_t)(*s)(0xc640, 0, node);
        if (flags & *(const uint8_t*)(uintptr_t)(e + 8)) {
            const uint32_t z = (uint32_t)(*s)(0xce9e, 0, 0, 0, 0, 0, node);
            (*s)(0x3d320, 0, 0, 0, 0, 0, e, z);
        }
        node = (uint32_t)(*s)(0xc75a, 0, 0, 0, 0, 0, node);
    } while (node);
}
