// 1.13c D2Common+0x217F0 (#10405)
#include <cstddef>
#include <cstdint>
// Operates on retail's 32-bit images (guest pointers are 4 bytes), reached through the low 32 bits of a wasm address;
// the offsets are the pins.
namespace {
inline uint8_t* P(uint32_t a) { return (uint8_t*)(uintptr_t)a; }
inline uint32_t& W(uint8_t* b, int o) { return *(uint32_t*)(b + o); }
}
// Append pItem (a type-4 unit) to the tail of pInv's item list. Returns 0 for a bad inventory/unit/item data, else 1.
// ponytail: the INVENTORY_RemoveItem step (0x21640, own row) is modelled only for an item whose parent-inventory link (item data +0x5c) is 0,
// which it leaves untouched; a linked item is outside this function's corpus.
uint32_t D2Common_217F0_AppendItem(uint32_t pInv, uint32_t pItem)
{
    uint8_t* inv = P(pInv);
    uint8_t* it = P(pItem);
    if (!inv || W(inv, 0) != 0x01020304u) return 0;
    if (!W(inv, 8)) return 1;
    if (!it || W(it, 0) != 4) return 0;
    if (W(P(W(inv, 8)), 0) != 4) return 1;
    uint8_t* idata = P(W(it, 0x14));
    if (!(W(it, 0) == 4 && idata)) return 0;      // 0x1E790: node = item data + 0x5c
    uint8_t* node = idata + 0x5c;
    if (W(node, 0)) return 0;                     // linked item: outside the corpus (see ponytail above)
    // 0x1F690: tail append
    uint32_t last = W(inv, 0x10);
    if (!last) W(inv, 0xc) = pItem;
    else {
        uint8_t* l = P(last);
        uint8_t* ld = W(l, 0) == 4 ? P(W(l, 0x14)) : nullptr;
        if (ld) W(ld, 0x5c + 8) = pItem;          // iVar4 + 100
    }
    W(node, 4) = W(inv, 0x10);                    // +0x60
    W(inv, 0x10) = pItem;
    W(node, 0) = pInv;
    // 0x2F2C0 / 0x2F270: path x := count, y := 0 (type-4 unit)
    uint8_t* path = P(W(it, 0x2c));
    W(path, 0xc) = W(inv, 0x28);
    W(path, 0x10) = 0;
    node[0xc] = 0;
    node[0xd] = 1;
    W(inv, 0x28)++;
    W(idata, 0xc) = 0xFFFFFFFFu;                  // 0x22510
    return 1;
}
