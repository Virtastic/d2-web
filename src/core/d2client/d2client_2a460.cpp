// 1.13c D2Client+0x2A460: move-to (EAX unit, stack x, y, flag; ret 0xC). The unit position comes from the static
// path (+0xC/+0x10) for types 2/4/5 and from D2Common #10867/#10750 on the dynamic path otherwise (0 without one).
// Within one cell on both axes, or a type-1 unit 0x1C10 rejects, the path is stopped (#10559(path, 0)). Else a
// 7-dword request {x, y, ?, type1 ? 1 : ?, 100, 15, ?} goes to 0x2A750 with kind 1 (0x17 for a type-0 unit that
// is not the player at global 0x11BBFC); a non-player type-0 unit with `flag` set goes to 0x2A1B0 instead.
#include "d2client_callee.h"
// 1.13c D2Client+0x2A460
extern "C" void d2c_2a460(const D2cSvc& v, const uint32_t* unit /*EAX*/, int32_t x, int32_t y, uint32_t flag, uint32_t player /*[0x11BBFC]*/) {
    const uint32_t type = unit[0], path = unit[0xb];
    const bool stat = type == 2 || (type > 3 && type <= 5);
    int32_t ux = stat ? (int32_t)((const uint32_t*)(uintptr_t)path)[3] : path ? (int32_t)d2c_call(v, 0xc3be, 0, 0, 0, 0, 0, path) : 0;
    int32_t dx = x - ux; if (dx < 0) dx = -dx;
    uint32_t far = dx > 1;
    int32_t uy = stat ? (int32_t)((const uint32_t*)(uintptr_t)path)[4] : path ? (int32_t)d2c_call(v, 0xc2ec, 0, 0, 0, 0, 0, path) : 0;
    int32_t dy = y - uy; if (dy < 0) dy = -dy;
    if (dy > 1) far = 1;
    if ((type == 1 && !d2c_call(v, 0x1c10, unit[1], 0, 2, 0, 0)) || !far) { d2c_call(v, 0xc670, 0, 0, 0, 0, 0, path, 0); return; }
    uint32_t req[7] = { (uint32_t)x, (uint32_t)y, 0, 0, 100, 15, 0 };
    uint32_t kind = 1;
    if (type == 0) {
        if ((uint32_t)(uintptr_t)unit != player) {
            if (flag) { d2c_call(v, 0x2a1b0, (uint32_t)x, 0, 0, 0, 0, (uint32_t)y, 2, 0, 0); return; }
            kind = 0x17;
        }
    } else if (type == 1) req[3] = 1;
    d2c_call(v, 0x2a750, 0, kind, (uint32_t)(uintptr_t)unit, 0, 0, (uint32_t)(uintptr_t)req, 0);
}
