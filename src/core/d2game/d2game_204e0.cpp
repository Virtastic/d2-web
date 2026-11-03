// SUNITINACTIVE_CompressInactiveUnit (1.13c D2Game+0x204E0): EAX = unit, stack = game (ret 4). Retail-layout (32-bit offsets) rule; every engine callee is an Env member.
// game+0x1C pool, +0xA8 frame, +0xD8 + act*4 = head of the per-act list of 0x18-byte position nodes {x, y, ..., +0x10 record list, +0x14 next}.
// Records are 0x34 bytes: +0 x, +4 y, +8 type, +0xC class, +0x10 mode, +0x14 frame, +0x18/+0x1C unit flags, +0x20/+0x24/+0x28 per-type data, +0x2C object flagEx, +0x30 next.
#include "d2game_2e790.h"
using namespace d2g32;

struct D2G204E0Env {
    void (*monsterCompress)(uint32_t unit, uint32_t game);                 // 0x1F600 (EAX unit, stack game)
    uint32_t (*room)(uint32_t unit);                                       // thunk 0xA478 (#10331)
    uint32_t (*roomLevel)(uint32_t room);                                  // thunk 0xA556 (#10826)
    uint32_t (*levelAct)(uint32_t level);                                  // thunk 0xA56E (#10459); the low byte is used
    void (*roomOrigin)(uint32_t room, uint32_t* xy);                       // thunk 0xA6D0: out pair
    uint32_t (*alloc)(uint32_t pool, uint32_t size, uint32_t line);        // thunk 0xA2E6 (Fog pool alloc, file SUnitInactive.cpp)
    void (*compressItem)(uint32_t game, uint32_t node);                    // 0x203D0 (stdcall game, node, 0)
    uint32_t (*pathX)(uint32_t path);                                      // thunk 0xA454
    uint32_t (*pathY)(uint32_t path);                                      // thunk 0xA3B8
    uint32_t (*objTxt)(uint32_t cls);                                      // thunk 0xA520 (#10688)
    uint32_t (*isShrine)(uint32_t unit);                                   // thunk 0xB186
    uint32_t (*shrineWord)(uint32_t unit);                                 // 0x8E490 (EAX unit)
};

// 1.13c D2Game+0x204E0
void SUNITINACTIVE_CompressInactiveUnit(uint32_t unit, uint32_t game, const D2G204E0Env& env)
{
    if (rd32(unit) == 1) { env.monsterCompress(unit, game); return; }
    const uint32_t room = env.room(unit);
    const uint32_t act = env.levelAct(env.roomLevel(room)) & 0xFF;
    uint32_t xy[2];
    env.roomOrigin(room, xy);
    const uint32_t headSlot = game + 0xD8 + act * 4;
    uint32_t node = rd32(headSlot), prev = 0;
    bool found = false;
    while (node) {
        const int32_t nx = (int32_t)rd32(node);
        if ((int32_t)xy[0] == nx) { if (xy[1] == rd32(node + 4)) { found = true; break; } }
        else if ((int32_t)xy[0] > nx) break;
        prev = node; node = rd32(node + 0x14);
    }
    if (!found) {
        const uint32_t n = env.alloc(rd32(game + 0x1C), 0x18, 0x22F);
        for (uint32_t o = 0; o < 0x18; o += 4) wr32(n + o, 0);
        wr32(n, xy[0]); wr32(n + 4, xy[1]); wr32(n + 0x14, node);
        if (prev) wr32(prev + 0x14, n); else wr32(headSlot, n);
        node = n;
    }
    const uint32_t type = rd32(unit);
    if (type == 4) { env.compressItem(game, node); return; }
    const uint32_t rec = env.alloc(rd32(game + 0x1C), 0x34, 0x245);
    for (uint32_t o = 0; o < 0x34; o += 4) wr32(rec + o, 0);
    const uint32_t path = rd32(unit + 0x2C);
    const bool staticPath = type == 2 || (type > 3 && type <= 5);
    wr32(rec, staticPath ? rd32(path + 0xC) : (path ? env.pathX(path) : 0));
    wr32(rec + 4, staticPath ? rd32(path + 0x10) : (path ? env.pathY(path) : 0));
    wr32(rec + 8, type);
    wr32(rec + 0xC, rd32(unit + 4));
    if (type == 0) {
        wr32(rec + 0x20, rd32(unit + 0xC));
        wr32(rec + 0x10, rd32(unit + 0x10));
    } else if (type != 2) {
        wr32(rec + 0x10, rd32(unit + 0x10));
    } else {
        const uint32_t txt = env.objTxt(rd32(unit + 4));
        if (rd32(unit + 0x10) == 1 && rd8(txt + 0x109) == 0 && rd8(txt + 0x141) != 0) wr32(unit + 0x10, 2);
        wr32(rec + 0x10, rd32(unit + 0x10));
        if (env.isShrine(unit)) {
            wr32(rec + 0x20, env.shrineWord(unit) >> 16);
            wr32(rec + 0x24, env.shrineWord(unit) & 0xFFFF);
        } else {
            const uint32_t cls = rd32(unit + 4);
            if (cls == 0x3B || cls == 0x3C || cls == 0x64) wr32(rec + 0x20, rd32(unit + 0xC));
            else wr32(rec + 0x24, rd8(unit + 0x78));
        }
        wr32(rec + 0x2C, rd32(unit + 0xB8));
        wr8(rec + 0x28, rd8(rd32(unit + 0x14) + 4));
    }
    wr32(rec + 0x14, rd32(game + 0xA8));
    wr32(rec + 0x18, rd32(unit + 0xC4));
    wr32(rec + 0x1C, rd32(unit + 0xC8));
    wr32(rec + 0x30, rd32(node + 0x10));
    wr32(node + 0x10, rec);
}
