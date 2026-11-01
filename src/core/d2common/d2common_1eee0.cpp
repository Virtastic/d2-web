// 1.13c D2Common+0x1EEE0 (retail-layout, 32-bit image): does the belt hold a potion that 0x22CD0 pairs with this item?
// stdcall(inv, item), ret 8. The inventory needs magic 0x01020304; the item must be type 4 whose items.txt row (0x719A0: class
// below the count, row = base + class*0x1A8) has a code (+0x80) other than 'isc ' / 'tsc '. Grid index 1 sized by the constant
// pair {16, 1} (global 0x9D390) is read; each of its first four cell words that is non-null is handed to 0x22CD0 with the item,
// and the first non-zero answer gives 1. No grid, or no match: 0.
#include <cstdint>
#include "inv_place.h"
struct D2Common1EEE0Env { uint32_t (*compare)(uint32_t item, uint32_t cell); const InvEnv* inv; };
extern "C" uint32_t D2Common_1EEE0_BeltHasPotion(const D2Common1EEE0Env* e, uint32_t inv, uint32_t item)
{
    auto P = [](uint32_t a) { return (uint8_t*)(uintptr_t)a; };
    if (!inv || *(const uint32_t*)P(inv) != 0x1020304) return 0;
    if (!item || *(const uint32_t*)P(item) != 4) return 0;
    uint32_t cls = *(const uint32_t*)(P(item) + 4);
    if (cls >= e->inv->itemCount || !e->inv->items) return 0;
    uint32_t code = *(const uint32_t*)(e->inv->items + cls * 0x1A8 + 0x80);
    if (code == 0x20637369u || code == 0x20637374u) return 0;
    static const uint8_t kInfo[2] = { 16, 1 };
    uint32_t g = D2Common_1E1B0_GetGrid(*e->inv, inv, 1, (uint32_t)(uintptr_t)kInfo);
    if (!g) return 0;
    for (uint32_t o = 0; o < 0x10; o += 4) {
        uint32_t c = *(const uint32_t*)(P(*(const uint32_t*)(P(g) + 0xC)) + o);
        if (c && e->compare(item, c)) return 1;
    }
    return 0;
}
