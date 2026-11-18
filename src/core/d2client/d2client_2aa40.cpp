// 1.13c D2Client+0x2AA40: queue an action on a target (EAX unit, stack p0, p1, p2; ret 0xC). When 0x29F30 allows it,
// a 7-dword request {p1, p2, 0, 0, 0, 0, 0} goes to 0x2A750 with kind 0x18 (globals 0x11C3E8 or 0x11C3EC set) or 0;
// on acceptance the 9-byte packet {4 or 2, p1, p2} is sent (0x143E0, EBX = 9) and a type-0 unit's pending record at
// unit[5]+0x150..0x15C is cleared. Finally a type-0 unit with unit[5] gets {1, p0, p1, p2} there (skipped only when
// the unit is null).
#include <cstring>
#include "d2client_callee.h"
// 1.13c D2Client+0x2AA40
extern "C" void d2c_2aa40(const D2cSvc& v, uint32_t* unit /*EAX*/, uint32_t p0, uint32_t p1, uint32_t p2, uint32_t g3e8, uint32_t g3ec) {
    const bool alt = g3e8 || g3ec;
    const uint32_t u = (uint32_t)(uintptr_t)unit;
    auto rec = [&]() -> uint32_t* { return (unit && unit[0] == 0 && unit[5]) ? (uint32_t*)(uintptr_t)unit[5] : nullptr; };
    if (d2c_call(v, 0x29f30, 0, 0, 0, 0, u)) {
        uint32_t req[7] = { p1, p2, 0, 0, 0, 0, 0 };
        if (d2c_call(v, 0x2a750, 0, alt ? 0x18u : 0u, u, 0, 0, (uint32_t)(uintptr_t)req, 0)) {
            uint8_t pkt[12] = {};
            pkt[0] = alt ? 4 : 2;
            std::memcpy(pkt + 1, &p1, 4);
            std::memcpy(pkt + 5, &p2, 4);
            d2c_call(v, 0x143e0, 0, 0, 0, 9, 0, (uint32_t)(uintptr_t)pkt);
        }
        if (!unit) return;
        if (uint32_t* r = rec()) { r[0x150 / 4] = 0; r[0x154 / 4] = 0; r[0x158 / 4] = 0; r[0x15c / 4] = 0; }
    }
    if (uint32_t* r = rec()) { r[0x150 / 4] = 1; r[0x154 / 4] = p0; r[0x158 / 4] = p1; r[0x15c / 4] = p2; }
}
