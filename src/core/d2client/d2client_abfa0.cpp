// 1.13c D2Client+0xABFA0 -- packet handler: tail-jumps to the routine taking the byte at +1 in ECX.
#include "cl_tbl.h"
extern "C" uint32_t d2c_abfa0(uint32_t pkt, const cl::Env& e) {
    return e.H(0x523b0, 0, 0, 0, 0, 0, 0, e.R8(pkt + 1));
}
