// 1.13c D2Common+0x353D0: collision pattern from the monster-type record. The data-table pointer, the bit-mask array pointer and the four-word pattern
// table (module globals 0x99E1C, 0x890B0, 0x8C5E4) are passed in so a replay probe can pin them; offsets are the retail 32-bit record layout.
#include <cstdint>
#include <cstring>
namespace {
inline uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
inline int16_t rd16s(uint32_t a) { int16_t v; memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 2); return v; }
inline uint8_t rd8(uint32_t a) { return *reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(a)); }
}
extern "C" uint32_t D2Common_353D0_Seam(uint32_t dt, uint32_t bitMasks, const uint32_t* pattern, int32_t index)
{
    if (index < 0 || index >= static_cast<int32_t>(rd32(dt + 0xA80))) return 1;
    const int32_t sub = rd16s(rd32(dt + 0xA78) + 0x18 + static_cast<uint32_t>(index) * 0x1A8);
    if (sub < 0 || sub >= static_cast<int32_t>(rd32(dt + 0xA98))) return 1;
    const uint32_t rec = static_cast<uint32_t>(sub) * 0x134 + rd32(dt + 0xA90);
    if (!rec) return 1;
    const int32_t kind = static_cast<int8_t>(rd8(rec + 8));
    if (kind < 0 || kind >= 4) return 1;
    const uint32_t flags = rd8(rec + 5);
    uint32_t v = pattern[kind];
    if (((rd32(bitMasks) & flags) != 0 || (rd32(bitMasks + 8) & flags) != 0) && (rd32(bitMasks + 4) & flags) == 0) {
        if (v == 1) v = 3;
        else if (v == 2) return 4;
    }
    return v;
}
