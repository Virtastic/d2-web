// 1.13c D2Game+0x34EA0: run the current act's quest "player entered" handler. EDI = unit (must be a player, type 0), stack = (game, pArgs) (ret 8), pArgs[0] must be 1.
// Flow: the 0x15D0 flag test (EAX = pArgs[1], EDX = 9) gates everything (0 returns 0); the game's quest control (game+0x10F4) needs a non-empty list (ctl+8);
// the act byte comes from 0x49DB0 (client word = playerData(unit)+0x9C); walk the nodes (ctl+0, chain +0xF4) whose act byte (+8) matches; the handler word
// (+0xEC) must be set, callable, and the unit's player data and its flags (pd+0x10+4*game[0x6D]) non-null (each failure is a fatal _exit, not ported);
// handler(ECX = node, EDX = pArgs[1] (the stack slot the 0x15D0 gate reads); stack unit, flags, pArgs) != 0 sends packet {0x8A, 1, pArgs[3] u32} through 0x8A770 and returns 1; no match returns 0.
// Port seam: r[0]=unit, r[1]=game, r[2]=pArgs, r[5]=D2GameBatch20Env.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "d2game_batch20env.h"

static inline uint32_t rd(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }

extern "C" void D2Game_34ea0(uint32_t* r) {
    const D2GameBatch20Env& env = *(const D2GameBatch20Env*)(uintptr_t)r[5];
    const uint32_t unit = r[0], game = r[1], args = r[2];
    r[4] = 0;
    if (!unit || rd(unit, 0) != 0) std::abort();            // 0x7EC
    if (!args || rd(args, 0) != 1) std::abort();            // 0x7ED
    if (!env.checkFlag(rd(args, 4), 9)) return;
    const uint32_t ctl = rd(game, 0x10F4);
    if (!rd(ctl, 8)) std::abort();                          // 0x7F4
    const uint32_t pd0 = (uint32_t)(uintptr_t)env.getPlayerData((void*)(uintptr_t)unit);
    const uint8_t act = (uint8_t)env.actOfClient(rd(pd0, 0x9C));
    for (uint32_t node = rd(ctl, 0); node; node = rd(node, 0xF4)) {
        if (act != *(const uint8_t*)(uintptr_t)(node + 8)) continue;
        const uint32_t fn = rd(node, 0xEC);
        if (!fn || env.isBadCodePtr((void*)(uintptr_t)fn)) std::abort();       // 0x813, 0x802
        if (!env.getPlayerData((void*)(uintptr_t)unit)) std::abort();            // 0x803
        const uint32_t pd = (uint32_t)(uintptr_t)env.getPlayerData((void*)(uintptr_t)unit);
        const uint32_t flags = rd(pd, 0x10 + 4u * *(const uint8_t*)(uintptr_t)(game + 0x6D));
        if (!flags) std::abort();                                                // 0x805
        if ((uint8_t)env.callQuestHandler(fn, (void*)(uintptr_t)node, unit, flags, args)) {
            uint8_t packet[6] = { 0x8A, 1 };
            const uint32_t id = rd(args, 0xC); std::memcpy(packet + 2, &id, 4);
            uint32_t client = 0;
            if (rd(unit, 0) == 0) client = rd((uint32_t)(uintptr_t)env.getPlayerData((void*)(uintptr_t)unit), 0x9C);
            env.sendQueued(client, packet);
            r[4] = 1;
            return;
        }
    }
}
