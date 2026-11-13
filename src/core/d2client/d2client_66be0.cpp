// Button widget: mouse-up handler. Derived from the cached 1.13c decompile and disassembly.
#include <cstdint>
#include <cstring>
namespace d2client_btn {
// 1.13c D2Client+0x66BE0  (thiscall btn, [esp+4] = ev, ret 4). Pressed flag +0x20; rect x +8, y +0xc (bottom), w +0x1c, h +0x18;
// cb = the click callback at +0x28. A release inside clears the flag and clicks; outside clears +0x2c and the flag.
uint32_t up_66be0(uint8_t* btn, const uint8_t* ev, uint32_t (*cb)(uint8_t*)) {
    auto rd = [&](uint32_t o) { int32_t v; std::memcpy(&v, btn + o, 4); return v; };
    auto wr = [&](uint32_t o, uint32_t v) { std::memcpy(btn + o, &v, 4); };
    if (rd(0x20) == 0) return 0;
    uint16_t ux, uy; std::memcpy(&ux, ev + 0xc, 2); std::memcpy(&uy, ev + 0xe, 2);
    const int32_t x = ux, y = uy, l = rd(8), b = rd(0xc);
    if (x >= l && x <= rd(0x1c) + l && y >= b - rd(0x18) && y <= b) { wr(0x20, 0); return cb(btn); }
    wr(0x2c, 0); wr(0x20, 0);
    return 0;
}
}
