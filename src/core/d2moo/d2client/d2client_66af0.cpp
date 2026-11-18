// Button widget: key handler. Derived from the cached 1.13c decompile and disassembly.
#include <cstdint>
#include <cstring>
namespace d2client_btn {
// 1.13c D2Client+0x66AF0  (thiscall btn, [esp+4] = ev, ret 4). cb = the click callback at btn+0x28 (ECX = btn).
uint32_t key_66af0(uint8_t* btn, const uint8_t* ev, uint32_t (*cb)(uint8_t*)) {
    uint32_t en, f, code;
    std::memcpy(&en, btn + 0x24, 4); std::memcpy(&f, ev + 0xc, 4); std::memcpy(&code, ev + 8, 4);
    if (en != 0 && !(f & 0x40000000u) && code == 0xd) return cb(btn);
    return 0;
}
}
