// Fill the grid rectangle rows r0..r1, columns c0..c1 (inclusive, signed) with level/colour word `colour` and a clear flag word.
#include "d2client_light_env.h"
using namespace cllight;
extern "C" void d2c_a8560(int32_t r1, int32_t r0, uint32_t colour, int32_t c1, int32_t c0, const cl::Env& e) {
    if (r0 > r1) return;
    for (int32_t r = r0; r <= r1; ++r) {
        if (c0 > c1) continue;
        for (int32_t c = c0; c <= c1; ++c) { const uint32_t p = cell(c, r); e.W(p, 0); e.W(p + 4, colour); }
    }
}
