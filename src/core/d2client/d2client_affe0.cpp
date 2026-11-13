// 1.13c D2Client+0xAFFE0 (fastcall, ECX unused, EDX = flag): a zero EDX clears the three words 0x11BEFC, 0x11BF00 and 0x10B9DC and returns 1; any other EDX returns 0.
#include "cl_genv.h"
using namespace cl;
// 1.13c D2Client+0xAFFE0
extern "C" uint32_t d2c13_affe0(uint32_t edx, const Env& e, const GEnv& g) {
    if (edx) return 0;
    e.W(g.G(0x11befc), 0);
    e.W(g.G(0x11bf00), 0);
    e.W(g.G(0x10b9dc), 0);
    return 1;
}
