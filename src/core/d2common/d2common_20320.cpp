// Left-hand weapon lookup on a 32-bit-image inventory (item in the left or right arm slot whose id is the
// inventory's left-hand item id and whose type is a weapon). The native-layout twin in d2common_batch_1fb80.cpp
// stays for the play path's native callers.
#include <cstdint>
#include <cstring>
#include "inv_place.h"

struct D2UnitStrc;
uint32_t D2Common_24430_CheckItemTypeId(const D2UnitStrc* pItem, int nItemType);

namespace {
inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
inline uint32_t W(const uint8_t* p, uint32_t off) { uint32_t v; memcpy(&v, p + off, 4); return v; }
constexpr uint32_t kSignature = 0x1020304;
constexpr int kWeaponType = 0x2D;
}

// 1.13c D2Common+0x20320
uint32_t D2Common_20320_GetLeftHandWeaponImg(const InvEnv& e, uint32_t inv, uint32_t info)
{
    if (!inv || W(P(inv), 0) != kSignature) return 0;
    const uint32_t id = W(P(inv), 0x1C);
    if (id == 0xFFFFFFFFu) return 0;
    static const uint32_t kSlots[2] = {0x14u, 0x10u};  // left arm, then right arm
    for (uint32_t slot : kSlots) {
        if (W(P(inv), 0) != kSignature) return 0;    // retail re-checks before the second fetch
        const uint32_t g = D2Common_1E1B0_GetGrid(e, inv, 0, info);
        if (!g) continue;
        const uint32_t item = W(P(W(P(g), 0xC)), slot);
        if (item && D2Common_24430_CheckItemTypeId(reinterpret_cast<const D2UnitStrc*>(P(item)), kWeaponType)
            && W(P(item), 0xC) == id)
            return item;
    }
    return 0;
}
