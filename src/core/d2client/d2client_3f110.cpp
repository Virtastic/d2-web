// 1.13c D2Client+0x3F110 -- fastcall (ECX = a record): answers 1 when the dword at +0x34 is nonzero, else 2 when the dword at +0x38 is nonzero,
// else 0.
#include "cl_pa4.h"
extern "C" uint32_t d2c_3f110(uint32_t rec, const cl::Env& e) {
    if (e.R(rec + 0x34)) return 1;
    return e.R(rec + 0x38) ? 2u : 0u;
}
