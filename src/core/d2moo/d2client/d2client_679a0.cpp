// List panel: mouse handler. Derived from the cached 1.13c decompile and disassembly.
#include <cstdint>
#include <cstring>
namespace d2client_listpanel {
int32_t hit_67160(const uint8_t*, uint8_t*);
// 1.13c D2Client+0x679A0  (thiscall panel, [esp+4] = ev, ret 4). Inside the panel rect (strict: x +0x180708/+0x180710 wide,
// y +0x18070c/+0x180714 tall; ev x u16 +0xc, y u16 +0xe) the line under the point is selected through 0x67160. Returns 0.
uint32_t mouse_679a0(uint8_t* p, const uint8_t* ev) {
    auto rd = [&](uint32_t o) { int32_t v; std::memcpy(&v, p + o, 4); return v; };
    uint16_t ux, uy; std::memcpy(&ux, ev + 0xc, 2); std::memcpy(&uy, ev + 0xe, 2);
    const int32_t x = ux, y = uy, l = rd(0x180708), t = rd(0x18070c);
    if (x > l && x < rd(0x180710) + l && y > t && y < rd(0x180714) + t) hit_67160(ev, p);
    return 0;
}
}
