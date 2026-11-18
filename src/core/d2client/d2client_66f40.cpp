// Button widget: mouse-down handler. Derived from the cached 1.13c decompile and disassembly.
#include <cstdint>
#include <cstring>
namespace d2client_btn {
// 1.13c D2Client+0x66F40  (thiscall btn, [esp+4] = ev, ret 4). A press inside the rect sets the pressed flag (+0x20) and
// calls D2Client+0x88A70(0, 0, 0, 0) with EBX = 4 (a sound/feedback helper, injected as snd); returns 1, else 0.
uint32_t down_66f40(uint8_t* btn, const uint8_t* ev, void (*snd)()) {
    auto rd = [&](uint32_t o) { int32_t v; std::memcpy(&v, btn + o, 4); return v; };
    uint16_t ux, uy; std::memcpy(&ux, ev + 0xc, 2); std::memcpy(&uy, ev + 0xe, 2);
    const int32_t x = ux, y = uy, l = rd(8), b = rd(0xc);
    if (x >= l && x <= rd(0x1c) + l && y >= b - rd(0x18) && y <= b) {
        const uint32_t one = 1; std::memcpy(btn + 0x20, &one, 4);
        snd();
        return 1;
    }
    return 0;
}
}
