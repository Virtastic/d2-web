// 1.13c D2Game+0x2A9F0 (197 B, fastcall: ECX = client, EDX = unit): a non-null unit whose room is on the client's adjacent-room list is announced to
// the client with the 6-byte packet {0x0A, unit type byte, unit id}, unless it is a type 3 unit. The room is D2Common 10331, the list D2Common 10383
// (room, &list, &count), the send 0x8A3E0. A null client room (0x156B) or a null list entry met before the match (0x1570) is an assert.
#include "d2game_gamelist.h"

struct D2G2A9F0Env {
    uint8_t* (*unitRoom)(uint8_t* pUnit);                                                   // 0xA478
    void (*adjacentRooms)(uint8_t* pRoom, const uint8_t** ppList, int32_t* pnCount);   // the list is an array of 32-bit room pointers             // 0xA55C
    void (*sendPacket)(uint8_t* pClient, const uint8_t* pBuf, uint32_t nLen);               // 0x8A3E0
};

// 1.13c D2Game+0x2A9F0
void D2Game_2A9F0(uint8_t* pClient, uint8_t* pUnit, const D2G2A9F0Env* pEnv)
{
    if (pUnit == nullptr) return;
    uint8_t* pRoom = pEnv->unitRoom(pUnit);
    uint8_t* pClientRoom = (uint8_t*)d2gl::rdptr(pClient, 0x1B4);
    if (pClientRoom == nullptr) d2gl::fatal(0x156B);
    const uint8_t* pList = nullptr;
    int32_t nCount = 0;
    pEnv->adjacentRooms(pClientRoom, &pList, &nCount);
    for (uint32_t i = 0; i < (uint32_t)nCount; ++i)
    {
        const uint32_t nEntry = d2gl::rd32(pList, 4 * i);
        if (nEntry == 0) d2gl::fatal(0x1570);
        if (nEntry != (uint32_t)(uintptr_t)pRoom) continue;
        const uint8_t nType = (uint8_t)d2gl::rd32(pUnit, 0);
        if (nType == 3) return;
        uint8_t buf[6];
        buf[0] = 0x0A;
        buf[1] = nType;
        const uint32_t nId = d2gl::rd32(pUnit, 0xC);
        std::memcpy(buf + 2, &nId, 4);
        pEnv->sendPacket(pClient, buf, 6);
        return;
    }
}
