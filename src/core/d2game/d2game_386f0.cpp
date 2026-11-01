// D2Game: a matched pair of units is unlinked and removed, in the retail 32-bit layout (addresses are the low 32 bits of a pointer).
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

static uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

struct D2Game386F0Env {
    uint32_t (*record)(uint32_t x);                                  // 0xA4C0 thunk (stdcall 1): its +0x48 is the unit id
    void (*detach)(uint32_t unit, uint32_t game);                    // 0x56740: EAX = unit; stack game (ret 4)
    uint32_t (*partner)(uint32_t game, uint32_t unit);               // 0xE1570 (stdcall 2)
    uint32_t (*room)(uint32_t unit);                                 // 0xA478 thunk (stdcall 1)
    void (*roomEvent)(uint32_t room, uint32_t kind, uint32_t id);    // 0xA44E thunk (stdcall 3)
    void (*remove)(uint32_t game, uint32_t unit);                    // 0xE20A0: ECX = game, EDX = unit
    void (*roomFlag)(uint32_t room, uint32_t on);                    // 0xA51A thunk (stdcall 2)
};

static void unlinkOne(uint32_t game, uint32_t u, const D2Game386F0Env& env)
{
    env.detach(u, game);
}

static void removeOne(uint32_t game, uint32_t u, const D2Game386F0Env& env)
{
    const uint32_t r = env.room(u);
    if (r != 0) env.roomEvent(r, 2, rd32(u + 0xC));
    env.remove(game, u);
    env.roomFlag(r, 1);
}

// 1.13c D2Game+0x386F0
// EAX = x, EBX = game; plain ret. A null x, a null record from 0xA4C0 or a null game is a Fog assert (exit). The unit
// whose id is the record's +0x48 is looked up in the game's 128 object buckets (+0x1520, chain +0xE4). Only a unit of
// class 0x3B goes on: detached by 0x56740, its partner taken from 0xE1570, then removed (room event 2 with its id when
// it has a room, 0xE20A0, room flag 1); a non-null partner is detached and removed the same way.
void GAME_UnlinkMatchedPair_386F0(uint32_t x, uint32_t game, const D2Game386F0Env& env)
{
    if (x == 0) _exit(0x259);
    const uint32_t rec = env.record(x);
    if (rec == 0) _exit(0x25C);
    const uint32_t id = rd32(rec + 0x48);
    if (game == 0) _exit(0x2D3);
    uint32_t u = rd32(game + (id & 0x7F) * 4 + 0x1520);
    while (u != 0 && rd32(u + 0xC) != id) u = rd32(u + 0xE4);
    if (u == 0 || rd32(u + 4) != 0x3B) return;
    unlinkOne(game, u, env);
    const uint32_t p = env.partner(game, u);
    removeOne(game, u, env);
    if (p == 0) return;
    unlinkOne(game, p, env);
    removeOne(game, p, env);
}
