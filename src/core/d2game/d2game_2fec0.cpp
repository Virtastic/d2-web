// OBJECTS_SpawnBarrel (1.13c D2Game+0x2FEC0): ECX = game, EDX = room, stack = pDrlgCoords (ret 4). Retail-layout (32-bit offsets) rule; every engine callee is an Env member.
// coords: +0 x, +4 y, +8 width, +0xC height. game: +0x1C pool, +0x70 flag, +0x98 unit id counter, +0xBC + act*4 per-act word, +0xD0 block, +0x10F0 object control (seed lo/hi at +0/+4).
#include "d2game_2e790.h"
using namespace d2g32;

struct D2G2FEC0Env {
    uint32_t (*objTxt)(uint32_t cls);                                                          // thunk 0xA520 (#10688)
    uint32_t (*placeTest)(uint32_t coords, const uint32_t* xy, uint32_t room, uint32_t sx, uint32_t sy);   // 0x2E5D0: EAX coords, ESI &{x,y}, stack room, sizeX, sizeY
    uint32_t (*allocUnit)(uint32_t pool, uint32_t type);                                       // thunk 0xA6B8
    uint32_t (*roomLevel)(uint32_t room);                                                      // thunk 0xA556
    uint32_t (*levelAct)(uint32_t level);                                                      // thunk 0xA56E; the low byte is used
    void (*initUnit)(uint32_t unit, uint32_t gameBlock);                                       // 0xE0110: EDI unit, ESI game+0xD0
    uint32_t (*alloc)(uint32_t pool, uint32_t size, uint32_t line);                            // thunk 0xA2E6 (Fog pool alloc, file Objects.cpp)
    void (*setUnitState)(uint32_t unit);                                                       // thunk 0xA724
    void (*place)(uint32_t unit, uint32_t id, uint32_t room, uint32_t x, uint32_t y);          // 0x2F300 (ESI unit, stack id, room, x, y)
    void (*attach)(uint32_t room, uint32_t y, uint32_t unit, uint32_t x, uint32_t game, uint32_t one);   // 0xE0F40: EAX room, ECX y, EDX unit, stack x, game, 1
    void (*fatal)();                                                                           // room == 0 assert: _exit(-1)
};

// 1.13c D2Game+0x2FEC0
uint32_t OBJECTS_SpawnBarrel(uint32_t game, uint32_t room, uint32_t coords, const D2G2FEC0Env& env)
{
    const uint32_t xEnd = rd32(coords + 8) + rd32(coords) + 1;
    const uint32_t yEnd = rd32(coords + 0xC) + rd32(coords + 4) + 1;
    const uint32_t txt = env.objTxt(7);
    const uint32_t ctl = rd32(game + 0x10F0);
    uint32_t counter = 0;
    for (uint32_t x = rd32(coords); x < xEnd; x += 2) {
        for (uint32_t y = rd32(coords + 4); y < yEnd; y += 2) {
            const uint64_t v = (uint64_t)rd32(ctl) * 0x6AC690C5u + rd32(ctl + 4);
            wr32(ctl, (uint32_t)v); wr32(ctl + 4, (uint32_t)(v >> 32));
            const uint32_t cls = ((uint32_t)v & 3) == 0 ? 11 : 7;
            const uint32_t xy[2] = { x, y };
            if (env.placeTest(coords, xy, room, rd32(txt + 0xD0), rd32(txt + 0xD4)) != 1) continue;
            if (!room) { env.fatal(); return 1; }
            const uint32_t u = env.allocUnit(rd32(game + 0x1C), 2);
            if (u) {
                wr32(u, 2); wr32(u + 4, cls); wr32(u + 8, rd32(game + 0x1C)); wr32(u + 0x80, game);
                uint32_t f = rd32(u + 0xC8);
                f = rd32(game + 0x70) ? (f | 0x2000000u) : (f & 0xFDFFFFFFu);
                wr32(u + 0xC8, f | 0x4000000u);
                const uint32_t act = env.levelAct(env.roomLevel(room)) & 0xFF;
                wr8(u + 0x18, act);
                wr32(u + 0x1C, rd32(game + 0xBC + act * 4));
                wr32(u + 0x74, 0);
                env.initUnit(u, game + 0xD0);
                wr32(u + 0xC4, rd32(u + 0xC4) | 0x10);
                wr32(u + 0xD0, 0xB);
                uint32_t id = rd32(game + 0x98) + 1;
                if (id == 0xFFFFFFFFu) id = 1;
                wr32(game + 0x98, id);
                const uint32_t d = env.alloc(rd32(game + 0x1C), 0x38, 0x265);
                wr32(u + 0x14, d);
                for (uint32_t o = 0; o < 0x38; o += 4) wr32(d + o, 0);
                wr32(d, env.objTxt(cls));
                wr32(u + 0x10, 0);
                env.setUnitState(u);
                env.place(u, id, room, x, y);
                env.attach(room, y, u, x, game, 1);
            }
            if (++counter >= 0x20) return 1;
        }
    }
    return 1;
}
