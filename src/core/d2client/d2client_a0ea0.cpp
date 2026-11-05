// ECX = unit (ret 4). With 10889 set, 0xA0A30's verdict;
// otherwise 0 when the unit's 10641 item has stat 0x26 (10744), else 1.
#include <cstdint>
#include "cl_env.h"
extern "C" uint32_t d2c_a0a30(uint32_t u, const cl::Env& e);
// 1.13c D2Client+0xA0EA0
extern "C" uint32_t d2c_a0ea0(uint32_t u, const cl::Env& e) {
    if (e.H(0xc748, u)) return d2c_a0a30(u, e) ? 1u : 0u;          // 10889
    const uint32_t v = e.H(0xc6ee, u, 0);                           // 10641
    if (v && e.H(0xc1cc, v, 0x26)) return 0;                        // 10744
    return 1;
}
