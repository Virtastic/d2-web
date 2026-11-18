// ESI = missile request block (23 dwords), stack arg 0 =
// alternate block or null (ret 4). Sixty-four requests go to 0xB30F0 (ECX = block), each with entry i of the module
// tables 0xD4390 (to +0x1C) and 0xD4290 (to +0x20). Without an alternate the block flips bit 0x4000 of word 0 per
// request when word 4 is 0xB0; with one, a seed step on the block's unit (+4, seed at +0x20) sends the alternate
// when the new low word is a multiple of 6.
#include <cstdint>
#include "cl_env.h"
namespace { constexpr uint32_t kBase = 0x6FAB0000u; }
// 1.13c D2Client+0xA0DB0
extern "C" void d2c_a0db0(uint32_t self, uint32_t alt, const cl::Env& e) {
    for (uint32_t i = 0; i < 64; ++i) {
        const uint32_t hi = e.R(kBase + 0xd4390 + 4 * i), lo = e.R(kBase + 0xd4290 + 4 * i);
        uint32_t t = self;
        if (alt) {
            if (e.Lcg(e.R(self + 4) + 0x20) % 6 == 0) t = alt;
            e.W(t + 0x1c, hi); e.W(t + 0x20, lo);
        } else {
            e.W(self + 0x1c, hi); e.W(self + 0x20, lo);
            if (e.R(self + 0x10) == 0xb0) e.W(self, e.R(self) ^ 0x4000);
        }
        e.HB(0xb30f0, t);
    }
}
