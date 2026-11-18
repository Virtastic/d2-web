// 1.13c D2Client+0x7B3D0 (EAX = first node of a two-word list {payload, next}): for each node free the payload (line 0x60b), then the node (line 0x60c).
// The payload and next words are read before the first free; the payload is freed even when it is zero.
#include "cl_env.h"
extern "C" void d2c_7b3d0(uint32_t node, const cl::Env& e) {
    const uint32_t kFile = 0xB20D4BDCu;
    while (node) {
        uint32_t pay = e.R(node), nxt = e.R(node + 4);
        e.H(0xbf86, 0x60b, 0, 0, 0, 0, 0, pay, kFile);
        e.H(0xbf86, 0x60c, 0, 0, 0, 0, 0, node, kFile);
        node = nxt;
    }
}
