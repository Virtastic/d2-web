// 1.13c D2Client+0x8E2F0: the hovered body-slot highlight. EAX = the held item's size record, EBX = the inventory.
// Gate on the mouse (D2Client+0x11B828 x, +0x11B824 y) against the screen size (D2Client+0xDBC48 / +0xDBC4C): in panel mode 1
// (D2Client+0x11BC34) the left half is ignored, the bottom 0x27 rows always; on the left half the hovered slot D2Client+0x11BC68 is
// cleared. Otherwise the cursor item (Ordinal 11017, thunk 0xC202, on the inventory) with the pickup flag D2Client+0x11BC2C set:
// its cell size comes from Ordinal 10114 (thunk 0xC7F6: item, &w, &h, the source-file string, line 0x6DA) into two frame bytes;
// the centred position retail derives from them is never read again. The slot is accepted when Ordinal 10217 (0xC826: item, slot)
// and Ordinal 10244 (0xC898: item, player, 0, 0, 0, 0) both pass, else when 0x8C2B0 (ECX = item, EAX = the slot's item through
// Ordinal 11139, 0xC262) allows the swap. 0x8C5B0 paints the slot rectangle (EDX = D2Client+0x11BCA0 + slot * 0x14, EDI = accepted).
#include <cstdint>
#include "cl_env.h"
extern "C" void d2cl_8e2f0(uint32_t eax, uint32_t ebx, const cl::Env& e) {
    (void)eax;
    constexpr uint32_t M = 0x6FAB0000u;
    const int32_t w = (int32_t)e.R(M + 0xDBC48u), mx = (int32_t)e.R(M + 0x11B828u);
    if (e.R(M + 0x11BC34u) == 1 && mx < w / 2) return;
    if ((int32_t)e.R(M + 0x11B824u) >= (int32_t)e.R(M + 0xDBC4Cu) - 0x27) return;
    if (mx < w / 2) { e.W(M + 0x11BC68u, 0); return; }
    const uint32_t it = e.H(0xc202, ebx);
    if (!it || !e.R(M + 0x11BC2Cu)) return;
    uint8_t wh[8] = {};
    e.H(0xc7f6, it, (uint32_t)(uintptr_t)&wh[0], (uint32_t)(uintptr_t)&wh[1], M + 0xD478Cu, 0x6da);
    uint32_t ok = 0;
    if (e.H(0xc826, it, e.R(M + 0x11BC68u)) && e.H(0xc898, it, e.R(M + 0x11BBFCu), 0, 0, 0)) ok = 1;
    else {
        const uint32_t o = e.H(0xc262, ebx, e.R(M + 0x11BC68u));
        ok = e.H(0x8c2b0, 0, 0, 0, 0, 0, o, it) ? 1 : 0;
    }
    const uint32_t slot = e.R(M + 0x11BC68u);
    e.H(0x8c5b0, 0, 0, 0, 0, 0, 0, 0, M + 0x11BCA0u + slot * 0x14u);   // EAX (the slot) and ECX are not logged: the callee reads EDX and EDI
    e.Edj(0x8c5b0, ok);
}
