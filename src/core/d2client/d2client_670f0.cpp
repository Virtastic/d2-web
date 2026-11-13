// List panel: selected-line getter. Derived from the cached 1.13c decompile and disassembly.
#include <cstdint>
#include <cstring>
namespace d2client_listpanel {
// 1.13c D2Client+0x670F0  (fastcall ECX = panel). Returns the selected line index (+0x180738).
uint32_t selected_670f0(const uint8_t* p) {
    uint32_t v; std::memcpy(&v, p + 0x180738, 4); return v;
}
}
