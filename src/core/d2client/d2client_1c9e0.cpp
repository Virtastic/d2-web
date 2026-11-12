// 1.13c D2Client+0x1C9E0 -- table entry: clears the byte at 0x1223F3 and the pending continuation, and stores ECX as
// the continuation's argument. Returns 0.
#include "cl_tbl.h"
extern "C" uint32_t d2c_1c9e0(uint32_t ecx, const cl::Env& e) {
    clt::S8(e, 0x1223F3, 0);
    clt::S(e, 0x11BF94, 0);
    clt::S(e, 0x11BF98, ecx);
    return 0;
}
