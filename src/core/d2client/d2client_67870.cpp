// List panel: clear every line's six text heads. Derived from the cached 1.13c decompile and disassembly.
#include <cstdint>
#include <cstring>
namespace d2client_listpanel {
// 1.13c D2Client+0x67870  (fastcall ECX = panel). For each line i < count (u16 @0x180748), the first u16 of each of the
// six texts (line base 0x300e*i, text k at +8+0x800*k) is zeroed; then the count is zeroed.
void clear_67870(uint8_t* p) {
    uint16_t n; std::memcpy(&n, p + 0x180748, 2);
    for (uint32_t i = 0; i < n; ++i)
        for (uint32_t k = 0; k < 6; ++k) { const uint16_t z = 0; std::memcpy(p + 8 + 0x300eu * i + 0x800u * k, &z, 2); }
    const uint16_t z = 0; std::memcpy(p + 0x180748, &z, 2);
}
}
