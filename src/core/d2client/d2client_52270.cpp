// 1.13c D2Client+0x52270 -- three requests for an id (ESI = id), each 23 zeroed words with [1] = [2] = id, [12] = 1 and
// [4] = 0x12B, 0x166, 0x132 in turn, handed to 0xB30F0. A non-null result of the first goes to 0xC448(r, 0x4B) then
// 0xC394(r, 0x4B).
#include <cstring>
#include "cl_env.h"
extern "C" void d2c_52270(uint32_t id, const cl::Env& e) {
    uint32_t q[23] = {};
    q[1] = id; q[2] = id; q[4] = 0x12b; q[12] = 1;
    uint32_t r = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    if (r) { e.H(0xc448, r, 0x4b); e.H(0xc394, r, 0x4b); }
    memset(q, 0, sizeof q); q[1] = id; q[2] = id; q[4] = 0x166; q[12] = 1;
    e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    memset(q, 0, sizeof q); q[1] = id; q[2] = id; q[4] = 0x132; q[12] = 1;
    e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
}
