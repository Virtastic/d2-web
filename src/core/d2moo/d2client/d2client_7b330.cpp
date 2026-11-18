// 1.13c D2Client+0x7B330 (EDI = chat line node): free the six string pointers at node+0..0x14 that are non-null, then the node itself.
// Every free is the Fog free thunk (D2Client+0xBF86): ECX = block, EDX = module string +0xD4BDC, stack line id, 0.
#include "cl_env.h"
extern "C" void d2c_7b330(uint32_t node, const cl::Env& e) {
    const uint32_t kFile = 0xB20D4BDCu;   // the retail 0x6FB84BDC, logged module-relative
    for (uint32_t i = 0; i < 6; i++) {
        uint32_t p = e.R(node + i * 4);
        if (p) e.H(0xbf86, 0x6af, 0, 0, 0, 0, 0, p, kFile);
    }
    e.H(0xbf86, 0x6b2, 0, 0, 0, 0, 0, node, kFile);
}
