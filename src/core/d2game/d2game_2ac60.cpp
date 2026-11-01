// 1.13c D2Game+0x2AC60 (72 B, client in EAX and ESI): the join init. The 8-byte join body goes out through 0x8C420 (client, flag 1), then a one-byte
// packet holding 0 goes through the packet appender 0x8A3E0, and the client state (+4) becomes 1. A null client is the 0x95F assert.
#include "d2game_gamelist.h"

struct D2G2AC60Env {
    void (*sendJoinBody)(uint8_t* pClient);                          // 0x8C420
    void (*sendPacket)(uint8_t* pClient, const uint8_t* pBuf, uint32_t nLen);   // 0x8A3E0
};

// 1.13c D2Game+0x2AC60
void D2Game_2AC60(uint8_t* pClient, const D2G2AC60Env* pEnv)
{
    pEnv->sendJoinBody(pClient);
    const uint8_t nHeader = 0;
    pEnv->sendPacket(pClient, &nHeader, 1);
    if (pClient == nullptr) d2gl::fatal(0x95F);
    std::memcpy(pClient + 4, "\x01\x00\x00\x00", 4);
}
