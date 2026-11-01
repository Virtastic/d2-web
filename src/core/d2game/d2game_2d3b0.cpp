// 1.13c D2Game+0x2D3B0 (251 B, stdcall ret 0x20, 8 args: id, p2, w3, w4, flag5, p6, pPos, nExpect): records a client's position after a check.
// w4 != 0: 0x4A190 (EAX = id; stack p2, w3, w4, flag5 == 0) returning 0 ends with 1. Then flag5 != 0 -> report 0x2D2F0 (line 0x3EC) and 0;
// w4 == 0 -> report (line 0x3F9) and 0. Otherwise the current game (0x2AB70) and the client (0x49FE0, ECX = id) are fetched; a missing
// game or client = 0 (the game left through 0x29BA0 when it exists); client+0x60 != nExpect = leave + report (line 0x451) + 0; else
// client+0x190/+0x194 := pPos[0], pPos[1], leave the game, 0x8C490 (AL = 2, stack client) and the result is w4.
#include "d2game_gamelist.h"

struct D2Game2D3B0Env {
    uint32_t (*check)(uint32_t nId, uint32_t p2, uint32_t w3, uint32_t w4, uint32_t nFlag);   // 0x4A190
    void (*report)(uint32_t nId, uint32_t nLine);                                             // 0x2D2F0 with the Game.cpp file name
    void* (*getGame)();                                                                       // 0x2AB70
    void* (*getClient)(uint32_t nId);                                                         // 0x49FE0
    void (*unlockGame)(void* pGame);                                                          // 0x29BA0
    void (*notify)(void* pClient);                                                            // 0x8C490
};

// 1.13c D2Game+0x2D3B0
uint32_t D2Game_2D3B0(uint32_t nId, uint32_t p2, uint16_t w3, uint16_t w4, uint32_t nFlag5, uint32_t p6, const void* pPos, uint32_t nExpect, const D2Game2D3B0Env* pEnv)
{
    (void)p6;
    if (w4 != 0 && pEnv->check(nId, p2, w3, w4, nFlag5 == 0) == 0) return 1;
    if (nFlag5 != 0) { pEnv->report(nId, 0x3EC); return 0; }
    if (w4 == 0) { pEnv->report(nId, 0x3F9); return 0; }
    void* pGame = pEnv->getGame();
    if (pGame == nullptr) return 0;
    uint8_t* pClient = static_cast<uint8_t*>(pEnv->getClient(nId));
    if (pClient == nullptr) { pEnv->unlockGame(pGame); return 0; }
    if (d2gl::rd32(pClient, 0x60) != nExpect) { pEnv->unlockGame(pGame); pEnv->report(nId, 0x451); return 0; }
    std::memcpy(pClient + 0x190, pPos, 8);
    pEnv->unlockGame(pGame);
    pEnv->notify(pClient);
    return w4;
}
