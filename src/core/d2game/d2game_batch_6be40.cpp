// 1.13c D2Game+0x6BE40 AITHINK_SiblingTypeTwoCheck. Written from the 1.13c disassembly.
// Register contract: ecx = think context (ctx[1] = unit, ctx[2] = target). Plain return, always 0 (eax cleared).
// Loop bound resolved: ordinal 10383(owner, &array, &count) returns the count through the third argument.
#include <cstdint>

namespace ordns_6be40 {
__attribute__((weak, import_module("env"), import_name("Ordinal_10331"))) uint32_t Ordinal_10331(uint32_t);
__attribute__((weak, import_module("env"), import_name("Ordinal_10383"))) void Ordinal_10383(uint32_t, uint32_t*, uint32_t*);
}
void D2GAME_6BD60(int* esi, int edi);  // d2game_batch_6ae60.cpp

namespace {
inline uint32_t rd32(uintptr_t b, uint32_t o) { return *reinterpret_cast<uint32_t*>(b + o); }
// Scan the unit list at head+0x74 (link +0xE8) for a type-2 node, not `self`, whose +4 equals `id`.
inline bool hasSibling(uintptr_t holder, uint32_t self, uint32_t id) {
    for (uint32_t n = rd32(holder, 0x74); n; n = rd32(n, 0xE8))
        if (rd32(n, 0) == 2 && n != self && rd32(n, 4) == id) return true;
    return false;
}
}

// 1.13c D2Game+0x6BE40
extern "C" int D2GAME_AI_SiblingTypeTwoCheck_6BE40(int32_t* ctx)
{
    uint32_t unit = static_cast<uint32_t>(ctx[1]);
    uint32_t owner = ordns_6be40::Ordinal_10331(unit);
    if (!owner) return 0;
    uint32_t id = unit ? rd32(unit, 4) : 0xFFFFFFFFu;
    if (hasSibling(owner, unit, id)) { D2GAME_6BD60(reinterpret_cast<int*>(static_cast<uintptr_t>(static_cast<uint32_t>(ctx[2]))), static_cast<int>(owner)); return 0; }
    uint32_t arr = 0, cnt = 0;
    ordns_6be40::Ordinal_10383(owner, &arr, &cnt);
    for (uint32_t i = 0; i < cnt; ++i) {
        uint32_t o = rd32(arr, i * 4);
        if (o == owner) continue;
        if (hasSibling(o, static_cast<uint32_t>(ctx[1]), id)) {
            D2GAME_6BD60(reinterpret_cast<int*>(static_cast<uintptr_t>(static_cast<uint32_t>(ctx[2]))), static_cast<int>(o));
            return 0;
        }
    }
    return 0;
}
