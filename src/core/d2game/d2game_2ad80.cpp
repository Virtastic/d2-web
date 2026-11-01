// 1.13c D2Game+0x2AD80 (stdcall ret 8: game id word, message): the game-list lock is taken for the slot read and left again; 0x2AAE0 finds the game
// (null is the 0x1511 assert). The message is cut at 0xFF; one that starts with the 8 characters "<ABEND> " (CRT strncmp 0x3520) stores its ninth
// character's low nibble at game+0x1DC8. The message goes on to 0xCBE10 (EDX = message, game, 1) and the game lock is left (null is 0x100F).
#include "d2game_gamelist.h"

struct D2G2AD80Env {
    void (*enterList)();                                              // 0xA328, ECX = 0x110BE8, EDX = 0x125
    void (*leaveList)();                                              // LeaveCriticalSection, imported
    uint8_t* (*lockGame)(uint32_t nGuid);                             // 0x2AAE0
    int32_t (*strncmpCrt)(const char* a, const char* b, uint32_t n);  // 0x3520
    void (*logMessage)(uint8_t* pMsg, uint8_t* pGame, uint32_t nFlag);   // 0xCBE10
    void (*leaveGame)(void* pLock);
};

// 1.13c D2Game+0x2AD80
void D2Game_2AD80(uint32_t nId, char* pMsg, const D2G2AD80Env* pEnv)
{
    pEnv->enterList();
    const uint32_t nGuid = d2gl::usableGuid(d2gl::slotAt(nId & 0xFFFF));
    pEnv->leaveList();
    uint8_t* pGame = pEnv->lockGame(nGuid);
    if (pGame == nullptr) d2gl::fatal(0x1511);
    pMsg[0xFF] = 0;
    if (pEnv->strncmpCrt("<ABEND> ", pMsg, 8) == 0)
    {
        const uint32_t nCode = (uint8_t)pMsg[8] & 0xF;
        std::memcpy(pGame + 0x1DC8, &nCode, 4);
    }
    pEnv->logMessage((uint8_t*)pMsg, pGame, 1);
    void* pLock = d2gl::rdptr(pGame, 0x18);
    if (pLock == nullptr) d2gl::fatal(0x100F);
    pEnv->leaveGame(pLock);
}
