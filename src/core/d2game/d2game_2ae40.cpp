// 1.13c D2Game+0x2AE40 (stdcall ret 0xC: type, param, message): sends one message to every running game. The slot table is walked from index 1 for
// 1024 slots; a guid other than 0 and -1 is found and locked by 0x2AAE0 (null is the 0x100E assert). Type 1 sends the 40-byte event packet {0x5A, 0x0F,
// 0x04, param at +3, rest zero} through 0xCBDB0; any other type hands the message to 0xCBE10 (EDX = text, game, 1), a text of 256 or more characters
// first cut to its first 255 into a local buffer. The game lock is left at the end (null is the 0x100F assert).
#include "d2game_gamelist.h"

struct D2G2AE40Env {
    uint8_t* (*lockGame)(uint32_t nGuid);                              // 0x2AAE0
    void (*sendEvent)(const uint8_t* pPacket, uint8_t* pGame);         // 0xCBDB0, EAX = packet
    void (*logMessage)(const char* pMsg, uint8_t* pGame, uint32_t nFlag);   // 0xCBE10, EDX = text
    void (*leaveGame)(void* pLock);
};

// 1.13c D2Game+0x2AE40
void D2Game_2AE40(int32_t nType, uint32_t nParam, const char* pMsg, const D2G2AE40Env* pEnv)
{
    for (uint32_t i = 1; i <= 0x400; ++i)
    {
        const uint32_t nGuid = d2gl::slotAt(i);
        if (nGuid == 0 || nGuid == 0xFFFFFFFFu) continue;
        uint8_t* pGame = pEnv->lockGame(nGuid);
        if (pGame == nullptr) d2gl::fatal(0x100E);
        if (nType == 1)
        {
            uint8_t packet[0x28] = {};
            packet[0] = 0x5A;
            packet[1] = 0x0F;
            packet[2] = 0x04;
            std::memcpy(packet + 3, &nParam, 4);
            pEnv->sendEvent(packet, pGame);
        }
        else if (std::strlen(pMsg) > 0xFF)
        {
            char buf[0x100];
            std::strncpy(buf, pMsg, 0xFF);
            buf[0xFF] = 0;
            pEnv->logMessage(buf, pGame, 1);
        }
        else
        {
            pEnv->logMessage(pMsg, pGame, 1);
        }
        void* pLock = d2gl::rdptr(pGame, 0x18);
        if (pLock == nullptr) d2gl::fatal(0x100F);
        pEnv->leaveGame(pLock);
    }
}
