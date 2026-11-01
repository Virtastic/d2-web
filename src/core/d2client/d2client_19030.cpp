// 1.13c D2Client+0x19030: EAX = unit, ESI = {int cur; int idx} (ret 4: s0 passes to 0x18F90). Looks the entry up from idx (0xC4EA), lets the local-player gate (0x18F90) rewrite it, then re-resolves it through 0xC42A (Player.cpp:0x32C); a changed result invalidates idx
#include "cl17.h"
extern "C" uint32_t d2c_18f90(uint32_t unit, uint32_t ebx, uint32_t outp, const cl::Env& e);
extern "C" uint32_t d2c_19030(uint32_t unit, uint32_t esi, uint32_t s0, const cl::Env& e) {
    uint32_t local = e.H(0xc4ea, e.R(esi + 4), 0, 0, 0, 0, 0, unit);
    if (!local) return 1;
    if (!d2c_18f90(unit, cl17::ptr32(&local), s0, e)) return 0;
    uint32_t old = e.R(esi);
    uint32_t n = e.H(0xc42a, local, 0xB2000000u | 0xD6F7Cu, 0x32c);
    e.W(esi, n);
    if (n != old) e.W(esi + 4, 0xffffffffu);
    return 1;
}
