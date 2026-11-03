// 1.13c D2Game+0x2D4B0 (ordinal 10038, D2Game_10046) -- the module init. stdcall, no arguments, returns 1.
// Order: SRegLoadValue("Diablo II", "PlayerPos", 0, &PlayerPos) [Fog #423 through thunk 0xA29E], clear the 0x400-dword game GUID
// table, InitializeCriticalSection(game-list lock), CLIENTS_Initialize (0x4A110), D2Net #10009 registers the client-kick callback
// (retail 0x2D2F0; the vendored twin is sub_6FC36B20) through thunk 0xA394, SUNITPROXY_FillGlobalItemCache (0x8F850).
#include <cstdint>
#include <cstring>
#include "d2game_2d4b0.h"

int32_t d2game_port_2d4b0(const D2Game2D4B0Env* e, int32_t* guids, uint32_t* playerPos)
{
    e->loadPlayerPos(playerPos);
    std::memset(guids, 0, 0x400 * sizeof(int32_t));
    e->initGameListLock();
    e->clientsInitialize();
    e->registerKickCallback();
    e->fillGlobalItemCache();
    return 1;
}
