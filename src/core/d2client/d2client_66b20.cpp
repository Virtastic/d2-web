// List-panel button widget: set the click-callback enable word. Derived from the cached 1.13c decompile and disassembly.
#include <cstdint>
#include <cstring>
namespace d2client_btn {
// 1.13c D2Client+0x66B20  (thiscall btn, [esp+4] = v, ret 4). Stores v at +0x24; EAX = v.
uint32_t set_enable_66b20(uint8_t* btn, uint32_t v) {
    std::memcpy(btn + 0x24, &v, 4);
    return v;
}
}
