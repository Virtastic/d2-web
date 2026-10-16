// 1.13c D2Common+0x241D0
#include <cstddef>
#include <cstdint>
// Retail's 32-bit images (guest pointers are 4 bytes; ours are 8), kept only to pin the dereferenced offsets.
struct Unit32 { uint32_t type, id; uint8_t pad[0x14 - 8]; uint32_t pItemData; };
struct ItemData32 { uint8_t pad[0x18]; uint32_t flags; };
static_assert(offsetof(Unit32, type) == 0x00 && offsetof(Unit32, id) == 0x04 && offsetof(Unit32, pItemData) == 0x14, "241d0 unit type/id/item data");
static_assert(offsetof(ItemData32, flags) == 0x18, "241d0 item flags");
uint32_t D2Common_22690_ItemPlain(uint32_t nItemId);
// Over the resolved fields: 0 for a type-4 unit whose item data exists with flag 0x1000; otherwise 22690(id).
// Retail maps a null unit to id -1 (fatal there; the corpus has none).
uint32_t D2Common_241D0_ItemPlainUnit(uint32_t type, uint32_t id, bool hasData, uint32_t flags)
{
    if (type == 4 && hasData && (flags & 0x1000)) return 0;
    return D2Common_22690_ItemPlain(id) != 0;
}
