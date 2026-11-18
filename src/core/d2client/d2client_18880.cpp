// 1.13c D2Client+0x18880: EBX = out dword; s0 = unit/key, s1 = key2, s2 = out dword; playerSelf required. rec = 0xC640(ecx = s0);
// [ebx] = 0xC316(self, rec+0x64, (i16)rec[0], 0xC718(self, s0, 1)); [s2] = 0xC190(self, s1); returns [s2 value] <= [ebx] (signed) (ret 0xC)
#include "cl17.h"
extern "C" uint32_t d2c_18880(uint32_t outb, uint32_t s0, uint32_t s1, uint32_t s2, const cl::Env& e) {
    uint32_t self = e.self;
    if (!self || !s0 || !s1) return 0;
    uint32_t rec = e.H(0xc640, 0, 0, 0, 0, 0, 0, s0);
    if (!rec) return 0;
    uint32_t k = e.H(0xc718, self, s0, 1);
    uint32_t v = e.H(0xc316, self, e.R(rec + 0x64), (uint32_t)e.R16s(rec), k);
    e.W(outb, v);
    uint32_t q = e.H(0xc190, self, s1);
    e.W(s2, q);
    return (int32_t)q <= (int32_t)e.R(outb);
}
