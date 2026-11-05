// 1.13c D2Client+0x8B2F0: stdcall 1 (panel). Up to four cel draws through D2Gfx #10041 (thunk 0xD096,
// stdcall 6: block, x, y, -1, 5, 0) from one zeroed 18-dword request block: word 0 = frame, word 13 = cel file. obj = panel+0x60;
// D2Common #11139 (thunk 0xC262, (obj, n)) suppresses buttons 3 and 1; #10051 (thunk 0xC89E, (panel, obj)) == 2 marks the
// pressed state for the last pair. Positions are a base table (D2Client+0xE1118..) plus an offset table (D2Client+0x11BD94..).
#include "cl_env.h"
extern "C" void d2cl_8b2f0(uint32_t panel, const cl::Env& e) {
    uint32_t blk[18] = {0};
    auto G = [&](uint32_t rva) { return e.R(0x6FAB0000u + rva); };
    auto draw = [&](uint32_t frame, uint32_t cel, uint32_t bx, uint32_t ox, uint32_t by, uint32_t oy) {
        blk[0] = frame; blk[13] = G(cel);
        e.H(0xd096, (uint32_t)(uintptr_t)blk, G(bx) + G(ox), G(by) + G(oy), 0xffffffffu, 5);
        if (e.blk) e.blk(e.ctx, 0xd096, (uint32_t)(uintptr_t)blk, 18);
    };
    const uint32_t obj = e.R(panel + 0x60);
    if (!obj || !e.H(0xc262, obj, 3)) draw(0, 0x11BEB4, 0xE1118, 0x11BDBC, 0xE111C, 0x11BDC8);
    if (!obj || !e.H(0xc262, obj, 1)) draw(1, 0x11BEC0, 0xE1130, 0x11BD94, 0xE1134, 0x11BDA0);
    uint32_t b4 = 0, b5 = 0, pressed = 0;
    if (obj) { b4 = e.H(0xc262, obj, 4); b5 = e.H(0xc262, obj, 5); pressed = e.H(0xc89e, panel, obj) == 2; }
    if (!b5 && (!b4 || !pressed)) draw(0, 0x11BEC8, 0xE1160, 0x11BDE4, 0xE1164, 0x11BDF0);
    if (b4) return;
    if (!b5 || !pressed) draw(0, 0x11BEC8, 0xE1158, 0x11BDD0, 0xE115C, 0x11BDDC);
}
