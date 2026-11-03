// 1.13c D2Client+0x5AFA0 -- free the hireable UI resources (no args, plain ret). gl block: +0 the array pointer 0x1177C4, +4..+0x1C the seven
// slots 0x1177C8..0x1177E0, +0x20 the import slot 0xCE474 (slot, then cell, then sgptDataTables; the count is dword +0xBF0 of the tables).
// Each non-zero slot: #10065(slot) (thunk 0xC18A), then 0xBF86(ECX slot, EDX D6AB0, stack 0x7A, 0), then the slot := 0. A non-zero array pointer:
// the same release for its (count * 5, signed) entries, then 0xC016(ECX 0, EDX array, stack file D568C, 0xB7, 0) and the pointer := 0.
#include "cl_env.h"
extern "C" void d2c_5afa0(const cl::Env& e) {
    const uint32_t g = e.gl;
    auto rel = [&](uint32_t s) {
        e.H(0xc18a, s);
        e.H(0xbf86, 0x7a, 0, 0, 0, 0, 0, s, 0xB2000000u | 0xD6AB0);
    };
    for (uint32_t i = 0; i < 7; ++i) {
        const uint32_t s = e.R(g + 4 + 4 * i);
        if (s) { rel(s); e.W(g + 4 + 4 * i, 0); }
    }
    const uint32_t arr = e.R(g);
    if (!arr) return;
    const int32_t n = (int32_t)(e.R(e.R(e.R(g + 0x20)) + 0xbf0) * 5u);
    for (int32_t i = 0; i < n; ++i) {
        const uint32_t s = e.R(e.R(g) + 4 * (uint32_t)i);
        if (s) rel(s);
    }
    e.H(0xc016, 0xB2000000u | 0xD568C, 0xb7, 0, 0, 0, 0, 0, e.R(g));
    e.W(g, 0);
}
