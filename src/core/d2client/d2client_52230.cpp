// 1.13c D2Client+0x52230 -- request 0x133 for an id (EDX = id): 23 zeroed words, [1] = [2] = id, [4] = 0x133, [12] = 1,
// handed to 0xB30F0 (ECX = block). The result is ignored.
#include "cl_env.h"
extern "C" void d2c_52230(uint32_t id, const cl::Env& e) {
    uint32_t q[23] = {};
    q[1] = id; q[2] = id; q[4] = 0x133; q[12] = 1;
    e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
}
