// 1.13c D2Common+0x85990 (sub_85990): walk the unit's data's state list (+0x264) for the entry whose
// state-def (+0x00) has id 0x28 at +0x1D0; if found, call the +0x42A70 helper with a direction
// pair: entry kind (+0x04) == 3 -> (0, h-1, 0x16b), else (w-1, 0, 0x16a). Caller: +0x85E20.
// Local opaque layouts; field names are guesses, offsets are the retail 32-bit ones (pointers widen on wasm64).
#include <cstdint>

struct B85990_Def { uint8_t pad[0x1D0]; int32_t nId; };
struct B85990_Node { B85990_Def* pDef; int32_t nKind; int32_t pad[3]; B85990_Node* pNext; };
struct B85990_Data { uint8_t pad0[0x5C]; int32_t nW; int32_t nH; uint8_t pad1[0x264 - 0x64]; B85990_Node* pList; };
struct B85990_Unit { uint8_t pad[0x14]; B85990_Data* pData; };

void __fastcall D2Common_42A70(B85990_Unit* pUnit, int32_t a, int32_t b, int32_t c, int32_t d, int32_t e);

void __fastcall D2Common_85990(B85990_Unit* pUnit)
{
    B85990_Data* d = pUnit->pData;
    for (B85990_Node* n = d->pList; n; n = n->pNext) {
        if (n->pDef->nId != 0x28) continue;
        if (n->nKind == 3) D2Common_42A70(pUnit, 0, d->nH - 1, 0x16b, -1, 0);
        else D2Common_42A70(pUnit, d->nW - 1, 0, 0x16a, -1, 0);
        return;
    }
}
