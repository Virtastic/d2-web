// Seam for D2Game+0x2D4B0: the callees of the module init.
#pragma once
#include <cstdint>
struct D2Game2D4B0Env {
    void (*loadPlayerPos)(uint32_t* out);   // SRegLoadValue("Diablo II", "PlayerPos", 0, out)
    void (*initGameListLock)();             // InitializeCriticalSection(0x6FD30BE8)
    void (*clientsInitialize)();            // 0x4A110
    void (*registerKickCallback)();         // D2Net #10009 (thunk 0xA394) with the client-kick callback
    void (*fillGlobalItemCache)();          // 0x8F850
};
int32_t d2game_port_2d4b0(const D2Game2D4B0Env* e, int32_t* guids, uint32_t* playerPos);
