// 1.13c D2Common+0x44870 (sub_44870, __fastcall; sole caller +0x44B20): ordering predicate on two
// records. Field +4 is a kind; lower kind sorts first (returns 1). On equal kind, +0x10 points at a
// pair of ints and the kind picks the compare: 0 -> a[1] < b[1]; 1 -> b[0] < a[0]; 2 -> b[1] < a[1];
// 3 -> a[0] < b[0]. Anything else returns 0. Pointer widens under wasm64, so layout is by field.
#include <cstdint>

struct D2Common44870Rec {
    int32_t pad0;
    int32_t nKind;        // +0x04
    int32_t pad8[2];
    const int32_t* pKey;  // +0x10 (retail 32-bit)
};

// 1.13c D2Common+0x44870
int D2Common_44870_Less(const D2Common44870Rec* a, const D2Common44870Rec* b)
{
    if (a->nKind < b->nKind) return 1;
    if (a->nKind != b->nKind) return 0;
    switch (a->nKind) {
    case 0: return a->pKey[1] < b->pKey[1];
    case 1: return b->pKey[0] < a->pKey[0];
    case 2: return b->pKey[1] < a->pKey[1];
    case 3: return a->pKey[0] < b->pKey[0];
    }
    return 0;
}
