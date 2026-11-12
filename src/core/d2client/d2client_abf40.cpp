// 1.13c D2Client+0xABF40 -- packet handler: tail-jumps to the routine taking the packet in EAX.
#include "cl_tbl.h"
extern "C" uint32_t d2c_abf40(uint32_t pkt, const cl::Env& e) {
    return e.H(0x422e0, 0, 0, 0, 0, 0, pkt);
}
