// stand-alone 32-bit layout twin of D2Common 0x39530 (the proven row runs sub_6FDB5830; callers 0x39CE0, 0x39B30, 0x39B60, itself): ECX = stat list, EDX = layer | stat << 16, plain ret.
// A stat's value with its ItemStatCost op chain applied: the base value (0x38CD0) plus, for a record flagged at +0x52, up to
// sixteen op entries at +0xDE (stride 6: u16 stat, u16 target, u8 op, u8 param), each adding a term built from the target's
// own value (this function, recursing). Ops 1-9, 11 and 13 have cases; op 0 ends the chain. 32-bit retail layout through raw reads.
#include "d2common_39530.h"
#include <cstring>

namespace {
uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(uintptr_t(a)), 4); return v; }
uint32_t rd16(uint32_t a) { uint16_t v; std::memcpy(&v, reinterpret_cast<const void*>(uintptr_t(a)), 2); return v; }
uint32_t rd8(uint32_t a) { return *reinterpret_cast<const uint8_t*>(uintptr_t(a)); }
int32_t mul(int32_t a, int32_t b) { return static_cast<int32_t>(uint32_t(a) * uint32_t(b)); }
int32_t sar(int32_t v, uint32_t s) { return v >> (s & 31); }

// the value dword of entry i of the array behind header hdr (0x382B0's index; a miss is 0)
int32_t valueAt(uint32_t hdr, uint32_t key, const D2Common39530Env* e)
{
    const int32_t i = e->find(hdr, key);
    if (i < 0) return 0;
    const uint32_t ent = rd32(hdr) + uint32_t(i) * 8;
    return ent ? static_cast<int32_t>(rd32(ent + 4)) : 0;
}
}

// stand-alone twin of D2Common 0x39530 (the proven row runs sub_6FDB5830)
extern "C" int32_t D2Common_39530_StatWithOps(uint32_t S, uint32_t key, const D2Common39530Env* e)
{
    const uint32_t id = (key >> 16) & 0xFFFF, dt = e->dataTables;
    if (static_cast<int32_t>(rd32(dt + 0xBD4)) <= static_cast<int32_t>(id)) return 0;
    const uint32_t rec = id * 0x144 + rd32(dt + 0xBCC);
    if (!rec) return 0;
    int32_t acc = e->base(S, key, rec);   // EAX still holds the whole incoming key here (mov eax, edx at entry)
    if (!rd8(rec + 0x52)) return acc;
    int32_t base = acc;
    for (uint32_t k = 0; k < 16; ++k) {
        const uint32_t ent = rec + k * 6;
        const uint32_t op = rd8(ent + 0xE2), stat = rd16(ent + 0xDE), target = rd16(ent + 0xE0), param = rd8(ent + 0xE3);
        if (!op) return acc;
        const uint32_t o = rd32(S + 0x44), kind = rd32(S + 8);
        switch (op) {
        case 1:
            if (!o) break;
            if (rd32(o) == 4) base = e->ownerStat(o, id, 0);
            if (base) { const int32_t R = D2Common_39530_StatWithOps(S, target << 16, e); if (R) acc += e->pct(R, base); }
            break;
        case 2: case 3: {
            if (kind > 1 || stat == 0xFFFF) break;
            const uint32_t p = e->statRec(stat);
            if (!p) break;
            const uint32_t hdr = static_cast<int32_t>(rd32(S + 0x10)) < 0 ? S + 0x48 : S + 0x24;
            const int32_t v = sar(valueAt(hdr, stat << 16, e), rd8(p + 0x18));
            if (v <= 0) break;
            const int32_t R = D2Common_39530_StatWithOps(S, target << 16, e);
            if (!R) break;
            const int32_t t = sar(mul(R, v), param);
            acc += op == 2 ? t : e->pct(t, base);
            break;
        }
        case 4: case 5: {
            const uint32_t u = rd32(S + 4);
            if (!u || rd32(u) > 1 || stat == 0xFFFF) break;
            const uint32_t p = e->statRec(stat);
            if (!p) break;
            const int32_t v = sar(e->unitStat(u, stat, 0), rd8(p + 0x18));
            if (v <= 0) break;
            const int32_t R = D2Common_39530_StatWithOps(S, target << 16, e);
            if (!R) break;
            const int32_t t = sar(mul(R, v), param);
            acc += op == 4 ? t : e->pct(t, base);
            break;
        }
        case 6: case 7: {
            if (!o) break;
            const uint32_t g = rd32(o + 0x1C);
            if (!g) break;
            const int32_t R = D2Common_39530_StatWithOps(S, target << 16, e);
            if (!R) break;
            uint32_t t = 0;
            const uint32_t a = e->skillOf(g, &t);
            const int32_t x = e->eval(uint32_t(R), a, t);
            acc += op == 6 ? x : e->pct(x, acc);
            break;
        }
        case 8: case 9: {
            if (!o || rd32(o) != 0) break;
            const uint32_t c = e->charRec(rd32(o + 4));
            if (!c) break;
            const int32_t b = valueAt(S + 0x24, target << 16, e);
            const int32_t d = D2Common_39530_StatWithOps(S, target << 16, e) - b;
            if (!d) break;
            const uint32_t f = op == 8 ? rd8(c + 0x48) : rd8(c + (id == 0xB ? 0x47 : 0x46));
            acc += static_cast<int32_t>(f * uint32_t(d) * 0x40u);
            break;
        }
        case 11:
            if (!base || !o || rd32(o) > 1) break;
            { const int32_t R = D2Common_39530_StatWithOps(S, target << 16, e); if (R) acc += e->pct(R, base); }
            break;
        case 13:
            if (!o || rd32(o) != 4) break;
            base = e->ownerStat(o, id, 0);
            if (!base) break;
            { const int32_t R = D2Common_39530_StatWithOps(S, target << 16, e); if (R) acc += e->pct(R, base); }
            break;
        default: break;
        }
    }
    return acc;
}
