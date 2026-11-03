// 1.13c D2Game+0x73AA0 stop all player interactions (retail layout). EAX = game, EDI = the player's unit (interact info at
// +0x64 target id, +0x68 kind, +0x6C active byte). If it is active with kind 0 and the target (type 0, id) resolves, the
// info resets (id -1, kind 6, active 0). If the target is active toward this unit's id (+0xC): trade state 9 on both
// (player data +0x50), the update event (0xD, frame+1) for each, both trades are released, the 0x77 packet goes to the
// target's client (+0x9C of its player data, 0 when the target unit has a type) and its info resets. The callees ride in Env.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2G73AA0Env {
    uint32_t (*getUnit)(uint32_t game, uint32_t type, uint32_t id);                  // 0xE03A0 (EAX type, ECX game, EDX id)
    uint32_t (*plrData)(uint32_t unit);                                              // 0xA4C0 (ord 10920)
    void     (*event)(uint32_t game, uint32_t unit, uint32_t eax, uint32_t frame);   // 0x8EA80 (ECX game, EDX unit, 0xD, frame, 0, 0, 0; EAX at the call)
    void     (*detach)(uint32_t game, uint32_t unit);                                // 0x716D0
    void     (*freeTrade)(uint32_t unit);                                            // 0x700C0 (EAX unit)
    void     (*send77)(uint32_t client);                                             // 0x8B340 (AL 0xC, one stack word)
};

static inline uint32_t rd_73aa0(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void wr_73aa0(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }

extern "C" EMSCRIPTEN_KEEPALIVE void D2Game_73AA0(uint32_t game, uint32_t pl, const D2G73AA0Env* e)
{
    if ((uint8_t)rd_73aa0(pl + 0x6C) == 0 || rd_73aa0(pl + 0x68) != 0) return;
    const uint32_t tg = e->getUnit(game, rd_73aa0(pl + 0x68), rd_73aa0(pl + 0x64));
    if (!tg) return;
    wr_73aa0(pl + 0x64, 0xFFFFFFFF); wr_73aa0(pl + 0x68, 6); *(uint8_t*)(uintptr_t)(pl + 0x6C) = 0;
    if ((uint8_t)rd_73aa0(tg + 0x6C) == 0 || rd_73aa0(tg + 0x68) != 0 || rd_73aa0(tg + 0x64) != rd_73aa0(pl + 0xC)) return;
    wr_73aa0(e->plrData(pl) + 0x50, 9);
    e->event(game, pl, rd_73aa0(game + 0xA8) + 1, rd_73aa0(game + 0xA8) + 1);
    e->detach(game, pl);
    e->detach(game, tg);
    e->freeTrade(pl);
    e->freeTrade(tg);
    e->send77(rd_73aa0(tg) == 0 ? rd_73aa0(e->plrData(tg) + 0x9C) : 0);
    wr_73aa0(tg + 0x64, 0xFFFFFFFF); wr_73aa0(tg + 0x68, 6); *(uint8_t*)(uintptr_t)(tg + 0x6C) = 0;
    const uint32_t pd = e->plrData(tg);
    wr_73aa0(pd + 0x50, 9);
    e->event(game, tg, pd, rd_73aa0(game + 0xA8) + 1);
}
