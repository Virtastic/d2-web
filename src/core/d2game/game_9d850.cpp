// 1.13c D2Game+0x9D850 (EBX = game): per act 0..4 with its pending flag set, run the queued units of every room, then clear the flag;
// finally clear bit 0x400 of the arena control's flags. Retail 32-bit layout; the room/unit walk is a seam.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>
#include <cstdlib>
#include "pending_unit_updates.h"

#pragma pack(push, 1)
struct D2G9D850Game { uint8_t pad[0xBC]; uint32_t act[5]; uint8_t pad2[0x1D28 - 0xD0]; uint32_t arena; };
struct D2G9D850Arena { uint32_t pad0, pad4, flags; };
#pragma pack(pop)
static_assert(offsetof(D2G9D850Game, act) == 0xBC && offsetof(D2G9D850Game, arena) == 0x1D28 && offsetof(D2G9D850Arena, flags) == 8, "game fields");

// FN currently has no complete 0xE0860 binding. Do not turn a pending
// act into an apparent success by clearing its flag without visiting its units.
struct PendingReplay {
    D2G9D850Game* game;
    uint32_t act(int i) { return game->act[i]; }
    bool pending(uint32_t a) { return *(uint32_t*)(uintptr_t)a != 0; }
    uint32_t firstRoom(uint32_t) { std::abort(); }
    uint32_t nextRoom(uint32_t) { std::abort(); }
    uint32_t firstUnit(uint32_t) { std::abort(); }
    uint32_t nextUnit(uint32_t) { std::abort(); }
    void finishUnit(uint32_t) { std::abort(); }
    void clearQueue(uint32_t) { std::abort(); }
    void clearPending(uint32_t a) { *(uint32_t*)(uintptr_t)a = 0; }
    void clearArenaFlag() {
        if (game->arena) ((D2G9D850Arena*)(uintptr_t)game->arena)->flags &= 0xFFFFFBFFu;
    }
};
static void GAME_9D850(D2G9D850Game* game)
{
    PendingReplay env{game};
    d2game::updateQueuedUnits(env);
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_9d850(uint32_t* r)
{
    GAME_9D850((D2G9D850Game*)(uintptr_t)r[0]);
    r[4] = 0;
}
