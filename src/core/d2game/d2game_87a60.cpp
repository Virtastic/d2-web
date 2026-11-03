// AI think for the sand maggot queen: a three-state cycle held in the AI control's param words.
// State 2 (word +0x1C) idles in neutral for delay*25 frames and clears itself; state 1 (+0x18) asks
// D2Common for the minion spawn point, resolves the room there, spawns the minion with no XP
// (flag 0x4000000), bumps the spawn count (+0x14), idles for the delay and moves to state 2; state 0
// walks one step on the spot and arms state 1 while the count is under the per-difficulty maximum.
// Retail: ECX = game, EDX = unit, stack pAiTickParam (ret 4); the callees come in as seams so the
// FN probe can record them.
#include <cstdint>
#include <cstring>

struct D2G87A60Seams {
    void (*idleNeutral)(uint32_t game, uint32_t unit, int32_t frames);                       // 0xF22C0
    void (*minionInfo)(uint32_t unit, int32_t* id, int32_t* x, int32_t* y, int32_t* mode);  // D2Common #10751 via 0xAC64
    uint32_t (*unitRoom)(uint32_t unit);                                                     // 0xA478
    uint32_t (*roomAt)(uint32_t room, int32_t x, int32_t y);                                 // 0x1280
    uint32_t (*spawn)(uint32_t game, uint32_t room, int32_t x, int32_t y, int32_t id, int32_t mode); // 0xEF650, (.., 2, 0x42)
    int32_t (*unitX)(uint32_t unit);                                                         // 0x1100
    int32_t (*unitY)(uint32_t unit);                                                         // 0x1130
    void (*stepTo)(uint32_t game, uint32_t unit, int32_t x, int32_t y, int32_t mode);        // 0xF1FF0
    void (*idle)(uint32_t game, uint32_t unit, int32_t frames);                              // 0xF20C0
};

static inline uint32_t rd(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void wr(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
static inline int32_t rs16(uint32_t a) { int16_t v; memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }

// 1.13c D2Game+0x87A60
extern "C" void d2game_87a60_port(uint32_t pGame, uint32_t pUnit, uint32_t pAiTickParam, const D2G87A60Seams* s)
{
    const uint32_t pMonStats = rd(pAiTickParam + 0x1C);
    const uint32_t pCtl = rd(pAiTickParam);
    const uint32_t nDiff = *(const uint8_t*)(uintptr_t)(pGame + 0x6D);
    const uint32_t delay = pMonStats + 0x5C + nDiff * 2;   // aip2[difficulty]

    if (rd(pCtl + 0x1C))
    {
        s->idleNeutral(pGame, pUnit, rs16(delay) * 25);
        wr(pCtl + 0x1C, 0);
        return;
    }
    if (!rd(pCtl + 0x18))
    {
        if ((int32_t)rd(pCtl + 0x14) < rs16(pMonStats + 0x56 + nDiff * 2))   // aip1[difficulty]: the spawn cap
        {
            const int32_t y = s->unitY(pUnit);
            const int32_t x = s->unitX(pUnit);
            s->stepTo(pGame, pUnit, x, y, 8);
            s->idle(pGame, pUnit, rs16(delay));
            wr(pCtl + 0x18, 1);
        }
        return;
    }

    int32_t nId = 0, nX = 0, nY = 0, nMode = 0;
    s->minionInfo(pUnit, &nId, &nX, &nY, &nMode);
    const uint32_t pRoom = s->roomAt(s->unitRoom(pUnit), nX, nY);
    if (pRoom)
    {
        if (const uint32_t pMinion = s->spawn(pGame, pRoom, nX, nY, nId, nMode))
        {
            wr(pMinion + 0xC4, rd(pMinion + 0xC4) | 0x4000000);
            wr(pCtl + 0x14, rd(pCtl + 0x14) + 1);
        }
    }
    s->idle(pGame, pUnit, rs16(delay));
    wr(pCtl + 0x18, 0);
    wr(pCtl + 0x1C, 1);
}
