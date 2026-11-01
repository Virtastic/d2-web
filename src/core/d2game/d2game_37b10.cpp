// D2Game: a unit found by id is moved to a position and its timers re-armed, in the retail 32-bit layout (addresses are the low 32 bits of a pointer).
#include <cstdint>
#include <cstring>
#include <unistd.h>

static uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static void wr32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }

struct D2Game37B10Env {
    void (*check)(uint32_t src, uint32_t pos, uint32_t two, uint32_t line, uint32_t four);            // 0xAB8C thunk (stdcall 5)
    void (*place)(uint32_t room, uint32_t unit, uint32_t src, uint32_t x, uint32_t y);               // 0xB2A6 thunk (stdcall 5)
    void (*setFlag)(uint32_t unit, uint32_t flag);                                                   // 0xA4A2 thunk (stdcall 2)
    void (*setUnitFlag)(uint32_t unit, uint32_t flag, uint32_t on);                                  // 0xA3CA thunk (stdcall 3)
    void (*rearm)(uint32_t node, uint32_t game);                                                     // 0x8E6F0: EAX = node, EDI = game; plain ret
    uint32_t (*invOwner)(uint32_t inv);                                                              // 0xAF34 thunk (stdcall 1)
    void (*broadcast)(uint32_t fn, uint32_t game, const uint8_t* pkt);                               // 0xDFC10: EBX = fn, EDI = game; stack packet (ret 4)
    uint32_t sendFn;                                                                                 // module address of D2Game+0x8C530
};

// 1.13c D2Game+0x37B10
// ECX = src, EAX = pos (two dwords); stack game, id; ret 8. A null src or game is a Fog assert (exit). The unit with
// this id is looked up in the game's bucket table (+0x1120 + (id & 0x7F) * 4, chained through +0xE4); none returns.
// It is placed at pos in its room (+0x2C), flag 0x11 set, flags +0xC4 |= 2, &= ~8, unit flag 7 set, |= 0x10, then
// the timer nodes (+0xDC, chained through +0x24) of types 3, 4 and 6 are re-armed in that order. When the
// inventory owner (+0x60) is not -1, packet 0x8E {1, owner, id} goes to every client.
void UNITS_PlaceById_37B10(uint32_t src, uint32_t pos, uint32_t game, uint32_t id, const D2Game37B10Env& env)
{
    if (src == 0) _exit(0x21E);
    env.check(src, pos, 2, 0x9C19, 4);
    if (game == 0) _exit(0x2D3);
    uint32_t u = rd32(game + 0x1120 + (id & 0x7F) * 4);
    for (; u != 0 && rd32(u + 0xC) != id; u = rd32(u + 0xE4)) {}
    if (u == 0) return;
    env.place(rd32(u + 0x2C), u, src, rd32(pos), rd32(pos + 4));
    wr32(u + 0xC4, rd32(u + 0xC4) | 2);
    env.setFlag(u, 0x11);
    wr32(u + 0xC4, rd32(u + 0xC4) & ~8u);
    env.setUnitFlag(u, 7, 1);
    wr32(u + 0xC4, rd32(u + 0xC4) | 0x10);
    static const uint8_t kTypes[3] = { 3, 4, 6 };
    for (uint8_t t : kTypes)
        for (uint32_t n = rd32(u + 0xDC); n != 0;) {
            const uint32_t next = rd32(n + 0x24);
            if (*(const uint8_t*)(uintptr_t)n == t) env.rearm(n, game);
            n = next;
        }
    const uint32_t owner = env.invOwner(rd32(u + 0x60));
    if (owner == 0xFFFFFFFFu) return;
    uint8_t pkt[12] = { 0x8E, 1 };
    std::memcpy(pkt + 2, &owner, 4);
    std::memcpy(pkt + 6, &id, 4);
    env.broadcast(env.sendFn, game, pkt);
}
