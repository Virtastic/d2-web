// 1.13c D2Game+0x2CFD0 (191 B, stdcall ret 4): sends the client with id nId its 0x5A packet. nId 0 or no current game (0x2AB70) = nothing.
// The client comes from 0x49FE0 (ECX = id; null = 0x9D9 assert). A 40-byte packet is built: byte 0 = 0x5A, byte 2 = 4, the 16 bytes at
// client+0xD copied (Storm memcpy) to offset 8, byte 0x18 = 0; 0xCBDB0 (EAX = &packet, stack game) sends it, 0x4B7F0 (EAX = client id [client+0],
// stack game, 1) follows and 0x29BA0 (EAX = game) leaves the game.
#include "d2game_gamelist.h"

struct D2Game2CFD0Env {
    void* (*getGame)();                                                // 0x2AB70
    void* (*getClient)(uint32_t nId);                                  // 0x49FE0
    void (*copy)(void* pDst, const void* pSrc, uint32_t nLen);         // Storm ordinal 501
    void (*send)(const uint8_t* pPacket, void* pGame);                 // 0xCBDB0
    void (*dropClient)(uint32_t nClientId, void* pGame, uint32_t nFlag);   // 0x4B7F0
    void (*unlockGame)(void* pGame);                                   // 0x29BA0
    void (*fatal)(uint32_t nLine);
};

// 1.13c D2Game+0x2CFD0
void D2Game_2CFD0(uint32_t nId, const D2Game2CFD0Env* pEnv)
{
    if (nId == 0) return;
    void* pGame = pEnv->getGame();
    if (pGame == nullptr) return;
    const uint8_t* pClient = static_cast<const uint8_t*>(pEnv->getClient(nId));
    uint8_t packet[40] = {};
    packet[0] = 0x5A; packet[2] = 4;
    if (pClient == nullptr) { pEnv->fatal(0x9D9); return; }
    pEnv->copy(packet + 8, pClient + 0xD, 0x10);
    pEnv->send(packet, pGame);
    pEnv->dropClient(d2gl::rd32(pClient, 0), pGame, 1);
    pEnv->unlockGame(pGame);
}
