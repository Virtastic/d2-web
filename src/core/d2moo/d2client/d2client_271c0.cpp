// 1.13c D2Client+0x271C0: five panel cells (frames 0..4) of the same lazily loaded cell at [0x11A77C].
#include "d2client_callee.h"
// 1.13c D2Client+0x271C0
extern "C" void d2c_271c0(const D2cSvc& v, uint32_t* cellGlobal /*0x11A77C*/) {
    if (!*cellGlobal) *cellGlobal = d2c_call(v, 0xbf6c0, 0, 0, 0, 0, 0xd6afc);   // cell loader, ESI = path string at D2Client+0xD6AFC
    static const uint32_t xy[5][2] = { { 0, 0xfd }, { 0x100, 0x3f }, { 0, 0x1e4 }, { 0, 0x229 }, { 0x100, 0x229 } };
    for (uint32_t i = 0; i < 5; ++i) {
        uint32_t blk[18] = {};
        blk[0] = i; blk[13] = *cellGlobal;
        d2c_call(v, 0xd096, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)blk, xy[i][0], xy[i][1], 0xffffffffu, 5, 0);   // D2Gfx #10041 cel draw
    }
}
