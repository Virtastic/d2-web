// 1.13c D2Common+0x33C20 and +0x34000
#include <cstddef>
#include <cstdint>
// Retail's 32-bit images (the guest pointers are 4 bytes; ours are 8), kept only to pin the dereferenced offsets.
struct Owner32 { uint8_t pad[0x5C]; uint32_t pRec; };
struct Rec32 { uint8_t pad0[0x10]; int32_t flag; uint8_t pad1[0x58 - 0x14]; uint32_t pBits; };
static_assert(offsetof(Owner32, pRec) == 0x5C, "33c20 owner->rec");
static_assert(offsetof(Rec32, flag) == 0x10 && offsetof(Rec32, pBits) == 0x58, "33c20 rec flag/bits");
// The test over already-resolved fields: 1 when the owner and its rec exist, rec flag has the sign bit, the bit array exists
// and any of nWords words of mask AND bits is non-zero. nWords = ([sgptDataTables+0xC4] + 31) / 32 (signed), 6 in 1.13c.
uint32_t D2Common_33C20_BitmaskHit(bool hasOwner, bool hasRec, int32_t recFlag, const uint32_t* bits, const uint32_t* mask, int32_t nWords)
{
    if (!hasOwner || !hasRec || recFlag >= 0 || !bits) return 0;
    for (int32_t i = 0; i < nWords; ++i) if (mask[i] & bits[i]) return 1;
    return 0;
}
// 0x34000: the same test with mask = [sgptDataTables+0xE0]; the caller supplies that table mask.
uint32_t D2Common_34000_BitmaskHitTable(bool hasOwner, bool hasRec, int32_t recFlag, const uint32_t* bits, const uint32_t* tableMask, int32_t nWords)
{
    return D2Common_33C20_BitmaskHit(hasOwner, hasRec, recFlag, bits, tableMask, nWords);
}
