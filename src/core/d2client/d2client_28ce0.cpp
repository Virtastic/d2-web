// 1.13c D2Client+0x28CE0: the two weapon-switch slot descriptions of the player at global 0x11BBFC. For each side (#C5BC
// left, #C5CE right) an item the player cannot hold (#C718(player, item, 1) <= 0) is dropped: #C42A(item, panel.cpp, line 0x289 / 0x294)
// takes three words and leaves the file/line pair as the 3rd/4th of #C706, which removes it; #C50E / #C52C clear the slot and the side is read again. 0x28BC0 then draws it
// at x 0x75 (left, z 1) or global 0xDBC48 - 0xA5 (right, z 0), y global 0xDBC4C.
#include "d2client_callee.h"
// 1.13c D2Client+0x28CE0
extern "C" void d2c_28ce0(const D2cSvc& v, uint32_t player /*[0x11BBFC]*/, uint32_t g4c /*[0xDBC4C]*/, uint32_t g48 /*[0xDBC48]*/) {
    if (!player) return;
    const uint32_t src = 0xB2000000u | 0xd6b38;   // "..\Source\D2Client\UI\panel.cpp"
    uint32_t it = d2c_call(v, 0xc5bc, 0, 0, 0, 0, 0, player);
    if (it && (int32_t)d2c_call(v, 0xc718, 0, 0, 0, 0, 0, player, it, 1) <= 0) {
        const uint32_t r = d2c_call(v, 0xc42a, 0, 0, 0, 0, 0, it, src, 0x289);
        d2c_call(v, 0xc706, 0, 0, 0, 0, 0, player, r, src, 0x289);
        d2c_call(v, 0xc50e, 0, 0, 0, 0, 0, player, 0, 0xffffffffu);
        it = d2c_call(v, 0xc5bc, 0, 0, 0, 0, 0, player);
    }
    d2c_call(v, 0x28bc0, player, 0, 0, 0, 0, it, 0x75, g4c, 1);
    it = d2c_call(v, 0xc5ce, 0, 0, 0, 0, 0, player);
    if (it && (int32_t)d2c_call(v, 0xc718, 0, 0, 0, 0, 0, player, it, 1) <= 0) {
        const uint32_t r = d2c_call(v, 0xc42a, 0, 0, 0, 0, 0, it, src, 0x294);
        d2c_call(v, 0xc706, 0, 0, 0, 0, 0, player, r, src, 0x294);
        d2c_call(v, 0xc52c, 0, 0, 0, 0, 0, player, 0, 0xffffffffu);
        it = d2c_call(v, 0xc5ce, 0, 0, 0, 0, 0, player);
    }
    d2c_call(v, 0x28bc0, player, 0, 0, 0, 0, it, g48 - 0xa5, g4c, 0);
}
