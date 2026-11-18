// List panel: line-count getter. Derived from the cached 1.13c decompile and disassembly.
#include <cstdint>
#include <cstring>
namespace d2client_listpanel {
// 1.13c D2Client+0x678D0  (fastcall ECX = panel). Returns the u16 line count at +0x180748.
uint32_t count_678d0(const uint8_t* p) {
    uint16_t v; std::memcpy(&v, p + 0x180748, 2); return v;
}
}
