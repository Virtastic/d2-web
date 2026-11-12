// 1.13c D2Client+0x8C2B0: ECX = unit, EAX = object. Operate gate: both present, game mode
// (D2Client+0x11BC34) not 10/11, then six D2Common predicates (all stdcall thunks), then the object's +0x60 link count vs its cap.
#include "cl_env.h"
extern "C" uint32_t d2cl_8c2b0(uint32_t unit, uint32_t obj, const cl::Env& e) {
    if (!unit || !obj) return 0;
    const uint32_t m = e.R(0x6FAB0000u + 0x11BC34u);
    if (m == 10 || m == 11) return 0;
    const uint32_t F = 0xB2000000u | 0xD476Cu;   // the source-file string retail passes to the flag checks
    if (!e.H(0xc82c, unit)) return 0;
    if (!e.H(0xc1a8, obj, 0x800, 0x5cc, F)) return 0;
    if (!e.H(0xc90a, obj)) return 0;
    if (!(e.H(0xc97c, obj) & 0xff)) return 0;
    if (!e.H(0xc1a8, obj, 0x10, 0x5d2, F)) return 0;
    if (e.H(0xc1a8, obj, 0x100, 0x5d4, F)) return 0;
    const uint32_t o60 = e.R(obj + 0x60);
    if (!o60) return 0;
    const uint32_t cap = e.H(0xc988, obj) & 0xff;
    return e.H(0xc8d4, o60) < cap ? 1 : 0;
}
