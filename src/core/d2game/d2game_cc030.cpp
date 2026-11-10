// 1.13c D2Game+0xCC030: the per-client life/mana/position resync. Disassembled convention: EAX = player (type 0, its player
// data's +0x9C client), ECX = the unit whose stats are sent, one stack word `flag`, ret 4. The 0x49240 record (EAX = client) holds
// the last sent values: +0 idle count, +4 life, +6 mana, +8 stat 10, +0xA / +0xB the 0xCB840 / 0xCB720 bytes, +0xC / +0xE x / y,
// +0x10 / +0x12 the path deltas, +0x14 gold, +0x18 experience. With flag 0 the life change must be >= 10% of max life (signed
// idiv), or life 0 against a non-zero record life. A changed byte pair sends 0x8B8C0, a changed life/mana 0x8B7F0, a changed
// stat 10 0x8B750; otherwise the idle count runs to 4 and then a >= 2 move of x or y sends 0x8B750. A send stores x, y and the
// two delta bytes (zero-extended) and clears the count. Gold and experience changes go to 0x8BE10 / 0x8BDA0 (EDX new, ESI old).
#include <cstdint>
#include <cstring>
#include "d2game_cc030.h"

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
static inline uint16_t rd16(uint32_t a, uint32_t o) { uint16_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 2); return v; }
static inline uint8_t rd8(uint32_t a, uint32_t o) { return *(const uint8_t*)(uintptr_t)(a + o); }
static inline void wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(a + o), &v, 4); }
static inline void wr16(uint32_t a, uint32_t o, uint16_t v) { std::memcpy((void*)(uintptr_t)(a + o), &v, 2); }
static inline void wr8(uint32_t a, uint32_t o, uint8_t v) { *(uint8_t*)(uintptr_t)(a + o) = v; }

uint32_t d2game_cc030_port(uint32_t player, uint32_t unit, uint32_t flag, const D2GameCC030Env* e) {
    if (!player || !unit || rd32(player, 0) != 0) return 0;
    const uint32_t client = rd32(e->o10920(player), 0x9c);
    if (!client) return 0;
    const uint32_t rec = e->f_49240(client);
    if (!rec) return 0;
    const int32_t hp = (int32_t)e->o10973(unit, 6, 0) >> 8;
    const int32_t max = (int32_t)e->o10913(unit) >> 8;
    if (max <= 0) return 0;
    const uint16_t recHp = rd16(rec, 4);
    int32_t d = (int32_t)((uint32_t)recHp - (uint32_t)hp);
    if (d < 0) d = (int32_t)(0u - (uint32_t)d);
    if (flag == 0) {
        if ((int32_t)((uint32_t)d * 100u) / max < 10) return 0;
        if (hp == 0 && recHp != 0) return 0;
    }
    const uint32_t x = e->f_1100(unit);
    const uint32_t y = e->f_1130(unit);
    uint8_t dx = 0, dy = 0;
    const uint32_t path = rd32(unit, 0x2c);
    if (path) {
        dx = (uint8_t)((uint8_t)x - (uint8_t)e->pathX(path));
        dy = (uint8_t)((uint8_t)y - (uint8_t)e->pathY(path));
    }
    const uint32_t mana = (uint32_t)((int32_t)e->o10973(unit, 8, 0) >> 8);
    const uint32_t st10 = (uint32_t)((int32_t)e->o10973(unit, 0xa, 0) >> 8);
    const uint32_t gold = e->o10973(unit, 0xe, 0);
    const uint32_t exp = e->o10973(unit, 0xd, 0);
    const uint8_t b840 = (uint8_t)e->f_cb840(unit);
    const uint8_t b720 = (uint8_t)e->f_cb720(unit);

    int send = 0;   // 3 = 0x8B8C0, 2 = 0x8B7F0, 1 = 0x8B750
    if (rd8(rec, 0xa) != b840 || rd8(rec, 0xb) != b720) send = 3;
    else if (recHp != (uint16_t)hp || rd16(rec, 6) != (uint16_t)mana) send = 2;
    else if (rd16(rec, 8) != (uint16_t)st10) send = 1;
    else {
        const uint32_t cnt = rd32(rec, 0);
        if ((int32_t)cnt <= 3) { wr32(rec, 0, cnt + 1); }
        else {
            int32_t mx = (int32_t)rd16(rec, 0xc) - (int32_t)(uint16_t)x; if (mx < 0) mx = -mx;
            int32_t my = (int32_t)rd16(rec, 0xe) - (int32_t)(uint16_t)y; if (my < 0) my = -my;
            if (mx >= 2 || my >= 2) send = 1;
            else wr32(rec, 0, cnt + 1);
        }
    }
    if (send) {
        if (send == 3) e->f_8b8c0(client, (uint32_t)hp, mana, st10, b840, b720, x, y, dx, dy);
        else if (send == 2) e->f_8b7f0(client, (uint32_t)hp, mana, st10, x, y, dx, dy);
        else e->f_8b750(client, st10, x, y, dx, dy);
        wr16(rec, 0xc, (uint16_t)x);
        wr16(rec, 0xe, (uint16_t)y);
        wr16(rec, 0x10, dx);
        wr16(rec, 0x12, dy);
        wr32(rec, 0, 0);
    }
    const uint32_t oldGold = rd32(rec, 0x14);
    if (gold != oldGold) { e->f_8be10(client, gold, oldGold); wr32(rec, 0x14, gold); }
    const uint32_t oldExp = rd32(rec, 0x18);
    if (exp != oldExp) { e->f_8bda0(client, exp, oldExp); wr32(rec, 0x18, exp); }
    wr16(rec, 8, (uint16_t)st10);
    wr16(rec, 6, (uint16_t)mana);
    wr8(rec, 0xa, b840);
    wr16(rec, 4, (uint16_t)hp);
    wr8(rec, 0xb, b720);
    return 1;
}
