// 1.13c D2Common+0x50610 (caller 0x50F00): EAX = hand index, stack = (item A, item B, skills.txt record), ret 0xC. The skill's
// item-type check for one hand: the record's two excluded types for that hand (+0x24 + hand*4) and three required types
// (+0x18 + hand*6) are tested against item A with 0x24430; with no item A only the empty-hand rule (types 0x2D/0x2E/0x43 at
// +0x18, +0x1E) applies. A matched weapon then passes unless its items.txt-indexed record (0x230A0, sgptDataTables +0xBF8,
// stride 0xE4) marks it and item B fails the follow-up checks. 32-bit retail layout through raw reads.
#include "d2common_50610.h"
#include <cstring>

namespace {
uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(uintptr_t(a)), 4); return v; }
int32_t rd16s(uint32_t a) { int16_t v; std::memcpy(&v, reinterpret_cast<const void*>(uintptr_t(a)), 2); return v; }
uint32_t rd8(uint32_t a) { return *reinterpret_cast<const uint8_t*>(uintptr_t(a)); }
}

// 1.13c D2Common+0x50610
extern "C" uint32_t D2Common_50610_HandTypeOk(uint32_t hand, uint32_t a, uint32_t b, uint32_t txt, const D2Common50610Env* e)
{
    const uint32_t et = txt + 0x24 + hand * 4, it = txt + 0x18 + hand * 6;
    if (!a) {
        const bool any = rd16s(et) > 0 || rd16s(it) > 0;
        if (!any) return 1;
        const int32_t w = rd16s(txt + 0x18);
        const bool twoHandOk = (w != 0x2D && w != 0x2E && w != 0x43) || rd16s(txt + 0x1E) > 0 ||
            e->isType(0, 0x2D) || e->isType(b, 0x2D);
        if (!twoHandOk) return 1;
        const bool bowOk = w != 0x43 || rd16s(txt + 0x1E) > 0 || e->isType(0, 0x2D) || e->isType(b, 0x2D);
        return bowOk ? 0 : 1;
    }
    for (int32_t j = 0; j < 2; ++j) {
        const int32_t t = rd16s(et + 2 * j);
        if (t < 1) break;
        if (e->isType(a, uint32_t(t))) return 0;
    }
    for (int32_t k = 0;; ++k) {
        if (k > 2) return 0;
        const int32_t t = rd16s(it + 2 * k);
        if (t < 1) return k == 0;
        if (e->isType(a, uint32_t(t))) break;
    }
    if (rd32(a) != 4) return 1;
    const uint32_t d = rd32(a + 0x14);
    if ((d && (rd32(d + 0x18) & 0x4000)) || (d && (rd32(d + 0x18) & 0x100))) return rd16s(txt) == 0;
    const uint32_t dt = e->dataTables;
    const int32_t r = e->recIndex(rd32(a + 4));
    if (r < 0 || r >= static_cast<int32_t>(rd32(dt + 0xBFC))) return 1;
    const uint32_t rec = uint32_t(r) * 0xE4 + rd32(dt + 0xBF8);
    if (!rec || rd16s(rec + 0xC) == 0 || (rd8(txt + 6) & 1)) return 1;   // gdwBitMasks[0] = 1
    if (rd16s(txt) == 0) {
        const uint32_t sl = rd32(a + 0x5C);
        const uint32_t costRec = rd32(dt + 0xBCC) + 0xC6B4;
        if (sl && static_cast<int32_t>(rd32(dt + 0xBD4)) >= 0x9E && costRec && e->hasStat(sl, 0x9D0000, costRec)) return 1;
    }
    if (!b) return 0;
    if (!e->isType(b, e->itemType(a))) return 0;
    if ((rd16s(txt) != 0 || !e->getStat(b, 0x9D, 0)) && e->check(b) && e->baseStat(b, 0x46, 0) < 1) return 0;
    return 1;
}
