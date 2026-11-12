// 1.13c D2Client+0x62C30 -- table entry with three stack words: asserts the word at 0x11C1AC (line 0x81), then asks
// the three-word reader; when it answers 0 the fallback copy runs with (word 1, the fixed string, word 2) and 0 is
// returned, else 1.
#include "cl_tbl.h"
extern "C" uint32_t d2c_62c30(uint32_t s0, uint32_t s1, uint32_t s2, const cl::Env& e) {
    if (!clt::G(e, 0x11C1AC)) clt::assertAt(e, 0x81);
    if (e.H(0xbf1a, s0, s1, s2)) return 1;
    e.H(0xbe96, s1, 0x6FB85218u, s2);
    return 0;
}
