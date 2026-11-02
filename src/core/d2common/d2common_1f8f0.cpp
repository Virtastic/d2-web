// 1.13c D2Common+0x1F8F0 in retail guest layout (32-bit unit/item-data pointers): the FN probe and the
// replay hand it raw retail addresses; the native D2Common_1F8F0_UnlinkInventory is the same rule over wasm64 structs.
#include <cstdint>
#include <cstring>

namespace {
inline uint32_t Rd(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
inline void Wr(uint32_t a, uint32_t v) { std::memcpy(reinterpret_cast<void*>(static_cast<uintptr_t>(a)), &v, 4); }
// item data of a type-4 unit whose +0x14 is neither null nor the -0x5C sentinel; 0 otherwise
inline uint32_t Data(uint32_t unit) {
    if (!unit || Rd(unit) != 4) return 0;
    const uint32_t d = Rd(unit + 0x14);
    return (d && d != 0xFFFFFFA4u) ? d : 0;
}
}

// EDX = item, ESI = endpoint pair {head, tail}; links in the item data: previous +0x60, next +0x64.
extern "C" void D2Common_1F8F0_UnlinkGuest(uint32_t item, uint32_t endpoints)
{
    const uint32_t d = Data(item);
    if (!d) return;
    const uint32_t prev = Rd(d + 0x60), next = Rd(d + 0x64);
    if (!prev) {
        if (Rd(endpoints) == item) Wr(endpoints, next);
    } else if (const uint32_t pd = Data(prev)) {
        Wr(pd + 0x64, next);
    }
    if (!next) {
        if (Rd(endpoints + 4) == item) {
            Wr(endpoints + 4, prev);
            Wr(d + 0x64, 0);
            Wr(d + 0x60, 0);
            return;
        }
    } else if (const uint32_t nd = Data(next)) {
        Wr(nd + 0x60, prev);
    }
    Wr(d + 0x64, 0);
    Wr(d + 0x60, 0);
}
