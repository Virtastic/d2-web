// 1.13c D2Game+0x8CA20 -- send packet 0xAC (assign monster) for a monster unit.
// EAX = unit, stack = client (ret 4). Monsters only (type 1). Header: 0xAC, GUID, class, x, y (path 0xA454 / 0xA3B8),
// life byte (0x77060, EDI = unit), size byte; then the bit stream (D2Common #10126 init / #10128 write / #10127 size through the
// thunks 0xA2B6 / 0xA2CE / 0xA2E0) at packet+13, capacity 0xF4: mode nibble, the component block (0xAF610 / 0xADA2) or the
// champion block (type flags, boss id, mods, name word, pet owner via 0xE03A0 / 0x96600), alignment, the sent stats
// (0xAFBE / 0xA4C6, ItemStatCost +4 & gdwBitMasks[0], +8 bits, +9 param bits), and 0x8A3E0 sends size = bits + 13.
// The D2MOO 1.10f counterpart is sub_6FC3FC80 (stubbed empty in compat/game_net_stubs.cpp).
#include <cstdint>
#include <cstring>

struct D2Game8CA20Env {
    uint32_t (*pathX)(uint32_t path);                               // 0xA454 (stdcall 1)
    uint32_t (*pathY)(uint32_t path);                               // 0xA3B8 (stdcall 1)
    uint32_t (*life)(uint32_t unit);                                // 0x77060 (EDI = unit), the byte in AL
    void (*bwInit)(void* bw, void* buf, uint32_t cap);              // 0xA2B6 (stdcall 3)
    void (*bwWrite)(void* bw, uint32_t v, uint32_t bits);           // 0xA2CE (stdcall 3)
    uint32_t (*bwSize)(void* bw);                                   // 0xA2E0 (stdcall 1)
    uint32_t (*hasComp)(uint32_t unit);                             // 0xAF610 (EAX = unit)
    uint32_t (*compCount)(uint32_t unit, uint32_t i);               // 0xADA2 (stdcall 2)
    uint32_t (*serverUnit)(uint32_t game, uint32_t guid, uint32_t type);  // 0xE03A0 (ECX = game, EDX = guid, EAX = type)
    uint32_t (*petType)(uint32_t unit);                             // 0x96600 (EAX = unit)
    uint32_t (*statList)(uint32_t unit, uint32_t a, uint32_t flag); // 0xAFBE (stdcall 3)
    uint32_t (*baseStats)(uint32_t list, void* out, uint32_t max);  // 0xA4C6 (stdcall 3), out = {u16 layer, u16 stat, i32 value}[max]
    void (*send)(uint32_t client, void* buf, uint32_t size);        // 0x8A3E0 (EAX = client, stdcall 2)
};

namespace {
inline uint32_t r32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
inline uint32_t r16(uint32_t a, uint32_t o) { uint16_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 2); return v; }
inline uint32_t r8(uint32_t a, uint32_t o) { return *(const uint8_t*)(uintptr_t)(a + o); }
inline void w16(uint8_t* p, uint32_t v) { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); }
}

// 1.13c D2Game+0x8CA20
// dt = *sgptDataTables (+0xBCC ItemStatCost records 0x144 each, +0xBD4 count); bm = gdwBitMasks.
void D2Game_8CA20(uint32_t unit, uint32_t client, uint32_t dt, uint32_t bm, const D2Game8CA20Env& e)
{
    if (unit == 0 || r32(unit, 0) != 1) return;
    uint8_t pkt[0x101];
    uint32_t bw[4];
    pkt[0] = 0xAC;
    std::memcpy(pkt + 1, (const void*)(uintptr_t)(unit + 0xC), 4);
    w16(pkt + 5, r32(unit, 4));
    const uint32_t type = r32(unit, 0);
    const bool direct = type == 2 || (type > 3 && type < 6);
    const uint32_t path = r32(unit, 0x2C);
    w16(pkt + 7, direct ? r32(path, 0xC) : (path ? e.pathX(path) : 0));
    w16(pkt + 9, direct ? r32(path, 0x10) : (path ? e.pathY(path) : 0));
    pkt[11] = (uint8_t)e.life(unit);
    e.bwInit(bw, pkt + 13, 0xF4);

    const uint32_t mode = r32(unit, 0x10);
    e.bwWrite(bw, (mode == 8 || mode == 9 || mode == 0xC || mode == 0) ? mode : 1, 4);

    auto md = [&]() -> uint32_t { return r32(unit, 0) == 1 ? r32(unit, 0x14) : 0; };
    auto tf = [&](int b) -> uint32_t { const uint32_t m = md(); return m ? (r8(m, 0x16) >> b & 1) : 0; };

    bool champ = false;
    if (e.hasComp(unit)) {
        e.bwWrite(bw, 1, 1);
        if (r32(unit, 0) != 1) goto noMd;
        if (const uint32_t m = r32(unit, 0x14); m != 0 && m + 4 != 0) {
            for (uint32_t i = 0; i < 16; i++) {
                const uint32_t b = r8(m + 4, i);
                const int32_t n = (int32_t)e.compCount(unit, i);
                uint32_t bits;
                if (n < 3) bits = 1;
                else {
                    const uint32_t v = (uint32_t)n - 1;
                    bits = 0;
                    if (v) { int k = 31; while (!(v >> k)) k--; bits = (uint32_t)k + 1; }
                }
                e.bwWrite(bw, b, bits);
            }
        }
    } else {
        e.bwWrite(bw, 0, 1);
    }
    if (r32(unit, 0) == 1) {
        const uint32_t m = r32(unit, 0x14);
        if (m && r8(m, 0x1C) != 0) champ = true;
    }
noMd:
    if (!champ && (r32(unit, 0xC4) >> 9 & 1)) champ = true;
    if (!champ) {
        e.bwWrite(bw, 0, 1);
    } else {
        e.bwWrite(bw, 1, 1);
        e.bwWrite(bw, tf(2), 1);
        e.bwWrite(bw, tf(3), 1);
        e.bwWrite(bw, tf(1), 1);
        e.bwWrite(bw, tf(4), 1);
        e.bwWrite(bw, tf(6), 1);
        if (const uint32_t m = md(); m && (r8(m, 0x16) & 2))
            e.bwWrite(bw, (uint32_t)(int32_t)(int16_t)r16(m, 0x26), 0x10);
        if (const uint32_t m = md(); m && m + 0x1C != 0) {
            for (uint32_t i = 0; i < 9; i++) {
                const uint32_t b = r8(m + 0x1C, i);
                if (!b) break;
                e.bwWrite(bw, b, 8);
            }
        }
        e.bwWrite(bw, 0, 8);
        { const uint32_t m = md(); e.bwWrite(bw, m ? r16(m, 0x14) : 0x1506, 0x10); }
        bool pet = false;
        if (const uint32_t m = md(); m) {
            const uint32_t ai = r32(m, 0x28);
            if (ai && r32(ai, 0x28)) {
                const uint32_t o = e.serverUnit(r32(ai, 0x28), r32(ai, 0x2C), r32(ai, 0x30));
                if (o && r32(o, 0) == 0 && e.petType(o) == 7) {
                    e.bwWrite(bw, 1, 1);
                    e.bwWrite(bw, r32(o, 0xC), 0x20);
                    pet = true;
                }
            }
        }
        if (!pet) e.bwWrite(bw, 0, 1);
    }

    if ((r32(unit, 0xC8) >> 10 & 1) && r32(unit, 0x94) == 0) {
        e.bwWrite(bw, 1, 1);
        e.bwWrite(bw, r32(unit, 0x98) & 0x8FFFFFFFu, 0x1F);
    } else {
        e.bwWrite(bw, 0, 1);
    }

    uint32_t sent = 0;
    if (const uint32_t list = e.statList(unit, 0, 0x40); list != 0) {
        uint8_t st[16 * 8];
        const int32_t n = (int32_t)e.baseStats(list, st, 0x10);
        if (n <= 0) {
            e.bwWrite(bw, 0, 1);
        } else {
            for (int32_t i = 0; i < n; i++) {
                uint16_t layer, stat; int32_t val;
                std::memcpy(&layer, st + i * 8, 2); std::memcpy(&stat, st + i * 8 + 2, 2); std::memcpy(&val, st + i * 8 + 4, 4);
                if ((int32_t)stat >= (int32_t)r32(dt, 0xBD4)) continue;
                const uint32_t rec = r32(dt, 0xBCC) + stat * 0x144u;
                if (rec == 0 || !(r8(bm, 0) & r8(rec, 4)) || r8(rec, 8) == 0) continue;
                if (!sent) { e.bwWrite(bw, 1, 1); sent = 1; }
                e.bwWrite(bw, stat, 9);
                if (r8(rec, 9)) e.bwWrite(bw, layer, r8(rec, 9));
                e.bwWrite(bw, (uint32_t)val, r8(rec, 8));
            }
            if (sent) e.bwWrite(bw, 0x1FF, 9);
            else e.bwWrite(bw, 0, 1);
        }
    }
    if (!sent) e.bwWrite(bw, 0, 1);
    const uint32_t size = e.bwSize(bw) + 0xD;
    pkt[12] = (uint8_t)size;
    e.send(client, pkt, size);
}
