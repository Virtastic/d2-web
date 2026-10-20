// Implementation batch 62a30: quest/stone helpers, written from the cached 1.13c decompiles.
// Callees are declared by retail RVA (rNNNN) / ordinal (oNNNNN); unaff_* registers are explicit
// parameters. Not yet linked or vectored (RE-DONE only).
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>
using i32 = int32_t; using u32 = uint32_t; using u8 = uint8_t; using u16 = uint16_t;
extern "C" {
i32 o10920(void*); i32 o10174(i32, i32, i32); void o10003(i32, i32, i32); i32 o10265(i32); void o10024(const void*, i32);
i32 o10158(i32); i32 o10331(i32); void o10383(i32, i32*, i32*);
void r_8A3E0(void*, i32); void r_354F0(void*, i32); void r_34AA0(i32); void r_34C00(); void r_DFCC0(void*, i32);
void r_8B470(void*); i32 r_9BEB0(); void r_9C160(void*);
}
extern "C" i32 DAT_6fd31c20;
static inline i32 rd(i32 p, i32 o) { return *(i32*)(intptr_t)(p + o); }

// 1.13c D2Game+0x62A30
void D2GAME_62A30(i32* unit)
{
    struct { u32 a; u16 b; } pkt = { 0x1045d, 0 };
    if (unit && *unit == 0) o10920(unit);
    r_8A3E0(&pkt, 6);
}

static i32 stoneSlot(i32 game, void* unit) { return rd(o10920(unit), 0x10 + *(u8*)(intptr_t)(game + 0x6d) * 4); }

static i32 stoneActive(i32 game, void* unit, i32 q)
{
    i32 s = stoneSlot(game, unit);
    if ((o10174(s, q, 0) != 0 || o10174(s, q, 0xf) != 0) && o10174(s, q, 0xd) == 0 && o10174(s, q, 0xe) == 0) return 0;
    r_354F0(unit, game);
    return 0;
}

// 1.13c D2Game+0x62A80
i32 D2GAME_62A80(i32 game, void* unit) { return stoneActive(game, unit, 4); }
// 1.13c D2Game+0x62AE0
i32 D2GAME_62AE0(i32 game, void* unit) { return stoneActive(game, unit, 3); }
// 1.13c D2Game+0x62C00
i32 D2GAME_62C00(i32 game, void* unit) { return stoneActive(game, unit, 2); }
// 1.13c D2Game+0x62D30
i32 D2GAME_62D30(i32 game, void* unit) { return stoneActive(game, unit, 1); }

// 1.13c D2Game+0x62CD0
i32 D2GAME_62CD0(i32 game, void* unit)
{
    i32 s = stoneSlot(game, unit);
    if (o10174(s, 1, 0) != 1 && o10174(s, 1, 1) != 1) {
        o10003(s, 1, 0xe);
        r_34AA0(1);
        r_34C00();
    }
    return 0;
}

// 1.13c D2Game+0x63270 (ebx = game)
void D2GAME_63270(i32 ids, i32 quest, i32 flag, i32 game)
{
    for (i32 n = 0; n < (i32)*(u16*)(intptr_t)(ids + 0x80); n++) {
        u32 id = *(u32*)(intptr_t)(ids + n * 4);
        if (game == 0) { o10024((void*)0, o10265(0x2d3)); std::_Exit(-1); }
        for (i32 u = rd(game, 0x1120 + (id & 0x7f) * 4); u != 0; u = rd(u, 0xe4)) {
            if ((u32)rd(u, 0xc) != id) continue;
            i32 s = rd(o10920((void*)(intptr_t)u), 0x10 + *(u8*)(intptr_t)(game + 0x6d) * 4);
            if (o10174(s, quest, 0) != 1) {
                o10003(s, quest, 0xd);
                o10003(s, quest, 0);
                if (flag != 0) {
                    *(u16*)(intptr_t)(u + 0x6e) = (u16)flag;
                    *(i32*)(intptr_t)(u + 0x70) = u;
                    o10158(u);
                    *(u32*)(intptr_t)(u + 0xc4) |= 0x400;
                }
            }
            break;
        }
    }
}

// 1.13c D2Game+0x63350 (esi = unit)
void D2GAME_63350(void* p1, i32 unit)
{
    r_DFCC0(p1, 0);
    i32 q = rd(rd(unit, 4), 0x10f4);
    if (rd(q, 8) == 0) { o10024((void*)0, o10265(0x820)); std::_Exit(-1); }
    o10003(rd(q, 0xc), 5, 0xd);
    if (*(int16_t*)(intptr_t)(rd(unit, 0x18) + 100) == 0) { *(u8*)(intptr_t)(unit + 10) = 0; return; }
    extern void LAB_6fc811b0(); // 1.13c D2Game+0x611B0 (state callback)
    *(void**)(intptr_t)(unit + 200) = (void*)&LAB_6fc811b0;
}

// 1.13c D2Game+0x633C0 (caller 0x664F0): shuffle the five stone classes into slots, optionally log each
i32 D2GAME_633C0(i32 unit)
{
    u16* slots = (u16*)(intptr_t)rd(unit, 0x18);
    *(u32*)slots = 0; *((u32*)slots + 1) = 0; slots[4] = 0; slots[6] = 0;
    i32 game = rd(unit, 4);
    i32 seed = rd(game, 0x10f4);
    const i32 cls[5] = { 0x11, 0x12, 0x13, 0x14, 0x15 };
    i32 n = 0, q = 0;
    if (seed == 0) { o10024((void*)0, o10265(0x1fb)); std::_Exit(-1); }
    do {
        uint64_t v = (uint64_t)*(u32*)(intptr_t)(seed + 0x18) * 0x6ac690c5ull + *(u32*)(intptr_t)(seed + 0x1c);
        *(uint64_t*)(intptr_t)(seed + 0x18) = v;
        u32 lo = (u32)v;
        q = (i32)(lo / 5);
        i32 slot = (i32)(lo % 5);
        if (slots[slot] == 0) {
            char txt[84];
            snprintf(txt, sizeof txt, "stone %d is class %d", slot, cls[n]);
            if (DAT_6fd31c20 != 0) {
                u8 pk[0x11a] = {};
                pk[0] = 0x26; pk[1] = 1; pk[3] = 2; pk[8] = 4;
                memcpy(pk + 0x1a, txt, 0x100 < sizeof txt ? sizeof txt : 0x100); // retail copies 0x100 bytes of the stack buffer
                if (game == 0) { o10024((void*)0, o10265(0x1525)); std::_Exit(-1); }
                // IsBadCodePtr(callback) check at 0x1526 omitted: always valid here
                for (i32 c = rd(game, 0x88); c != 0; c = rd(c, 0x4a8))
                    if (rd(c, 4) == 4) r_8B470(pk);
            }
            slots[slot] = (u16)cls[n];
            n++;
        }
    } while (n <= 4);
    return q;
}

// 1.13c D2Game+0x63930 (ebx = count, ebp = unit)
i32 D2GAME_63930(i32 game, i32 list, i32 unit, u32 count)
{
    i32 p1 = o10331(list);
    i32 head = rd(game, 0x10f4);
    i32 target = 0;
    u32 id = 0;
    bool found = false;
    if (head) {
        for (i32 it = rd(head, 0); it != 0; it = rd(it, 0xf4)) {
            if (rd(it, 0) == 2) {
                i32 d = rd(it, 0x18);
                if (*(u8*)(intptr_t)(d + 2) == 0) return 0;
                id = (u32)rd(d, 8);
                found = true;
                break;
            }
        }
    }
    if (!found) { o10024((void*)0, o10265(0x133)); std::_Exit(-1); }
    for (target = rd(game, 0x1320 + (id & 0x7f) * 4); target != 0; target = rd(target, 0xe4))
        if ((u32)rd(target, 0xc) == id) break;
    i32 t = o10331(target);
    if (p1 != 0) {
        bool hit = false;
        if (p1 == t) hit = true;
        else {
            i32 u = o10331(unit);
            if (u == 0) { o10024((void*)0, o10265(0x14e)); std::_Exit(-1); }
            i32 x, y; o10383(u, &x, &y);
            for (u32 i = 0; i < count; i++) if (rd(list, i * 4) == t) { hit = true; break; }
        }
        i32 s = rd(o10920((void*)(intptr_t)unit), 0x10 + *(u8*)(intptr_t)(game + 0x6d) * 4);
        if (o10174(s, 2, 0) == 0 && o10174(s, 2, 1) == 0 && hit) { o10003(s, 2, 0xd); o10003(s, 2, 1); }
    }
    return 0;
}

// 1.13c D2Game+0x63D70
i32 D2GAME_63D70(i32 game, i32* unit)
{
    i32 s = rd(o10920(unit), 0x10 + *(u8*)(intptr_t)(game + 0x6d) * 4);
    if (o10174(s, 6, 0xd) != 0 && unit && unit[0x20] != 0 && *unit == 0 && (int16_t)r_9BEB0() != -1) {
        extern void LAB_6fc83000(); // 1.13c D2Game+0x63000
        r_9C160((void*)&LAB_6fc83000);
    }
    return 0;
}
