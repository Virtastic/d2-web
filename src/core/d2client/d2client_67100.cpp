// List panel: key handler. Derived from the cached 1.13c decompile and disassembly.
#include <cstdint>
#include <cstring>
namespace d2client_listpanel {
// 1.13c D2Client+0x67100  (thiscall panel, [esp+4] = ev, ret 4). Needs the callback at +0x18074a; marks ev handled
// (+0x18 = 1, +0x1c = 0), passes it to Storm#511 (o511), then, with the mode word at +0x180772 >= 2, a selection
// (+0x180738 != -1) and ev code (+8) 0xd, calls the callback. Returns 1 once the callback slot is set and mode >= 2.
uint32_t key_67100(uint8_t* p, uint8_t* ev, void (*o511)(uint8_t*), void (*cb)()) {
    auto rd = [&](uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; };
    auto wre = [&](uint32_t o, uint32_t v) { std::memcpy(ev + o, &v, 4); };
    if (rd(0x18074a) == 0) return 0;
    wre(0x18, 1); wre(0x1c, 0);
    o511(ev);
    if (rd(0x180772) < 2) return 0;
    uint32_t code; std::memcpy(&code, ev + 8, 4);
    if (rd(0x180738) != 0xffffffffu && code == 0xd) cb();
    return 1;
}
}
