// 1.13c D2Client+0x270D0: five panel cells (frames 5..9) of the 0x3F/0x190 block, lazy-loading the panel cell at [0x11A77C].
#include "d2client_callee.h"
// 1.13c D2Client+0x270D0
extern "C" void d2c_270d0(const D2cSvc& v, uint32_t* cellGlobal /*0x11A77C*/) {
    if (!*cellGlobal) *cellGlobal = d2c_call(v, 0xbf6c0, 0, 0, 0, 0, 0xd6afc);   // cell loader, ESI = path string at D2Client+0xD6AFC
    static const uint32_t xy[5][2] = { { 0x190, 0x3f }, { 0x220, 0xfd }, { 0x2c9, 0x1e4 }, { 0x220, 0x229 }, { 0x190, 0x229 } };
    for (uint32_t i = 0; i < 5; ++i) {
        uint32_t blk[18] = {};
        blk[0] = 5 + i; blk[13] = *cellGlobal;
        d2c_call(v, 0xd096, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)blk, xy[i][0], xy[i][1], 0xffffffffu, 5, 0);   // D2Gfx #10041 cel draw
    }
}
