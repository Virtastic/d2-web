// 1.13c D2Launch+0xEA90  sub_EA90: EAX and ECX go into a two-word stack block {EAX, ECX}; E510 takes it in ECX. EAX of E510 stays the result.
#include <cstdint>
#include "d2launch_env.h"
extern "C" uint32_t d2l_ea90(const D2lSeams* s, uint32_t eax, uint32_t ecx) {
    const uint32_t blk[2] = { eax, ecx };
    return s->e510(blk);
}
