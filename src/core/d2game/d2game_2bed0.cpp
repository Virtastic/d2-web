// 1.13c D2Game+0x2BED0 (178 B, EAX = pClient, [esp+4] = pGame, ret 4): tells every type-4 client of the game about the unit the given
// client tracks. The unit is resolved as in 0x2BE20 (client+0x174 when bit 0 of +0x3D4 is set, else 0xE03A0 with EAX = +0x16C,
// EDX = +0x170, ECX = +0x1A8, clearing +0x174 when it fails). For each client of game+0x88 (next at +0x4A8) whose +4 is 4, the 5-byte
// packet {0x5C, unit+0xC} goes out through 0x8A3E0. A null client is the 0xFB assert.
#include <cstdint>
#include <cstring>

struct D2Game2BED0Env {
    void* (*getUnit)(uint32_t nEax, uint32_t nEdx, uint32_t nEcx);                  // 0xE03A0
    void (*sendPacket)(void* pClient, const uint8_t* pBuf, uint32_t nLen);          // 0x8A3E0 (EAX = client, ret 8)
    void (*fatal)(uint32_t nLine);
};

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, static_cast<const uint8_t*>(p) + o, 4); return v; }

// 1.13c D2Game+0x2BED0
void D2Game_2BED0(void* pClient, void* pGame, const D2Game2BED0Env* pEnv)
{
    if (pClient == nullptr) { pEnv->fatal(0xFB); return; }
    void* pUnit;
    if ((static_cast<const uint8_t*>(pClient)[0x3D4] & 1) != 0)
    {
        pUnit = reinterpret_cast<void*>(static_cast<uintptr_t>(rd32(pClient, 0x174)));
    }
    else
    {
        if (rd32(pClient, 0x174) == 0) return;
        pUnit = pEnv->getUnit(rd32(pClient, 0x16C), rd32(pClient, 0x170), rd32(pClient, 0x1A8));
        if (pUnit == nullptr) std::memset(static_cast<uint8_t*>(pClient) + 0x174, 0, 4);
    }
    if (pUnit == nullptr) return;

    for (uintptr_t pPeer = rd32(pGame, 0x88); pPeer != 0; pPeer = rd32(reinterpret_cast<void*>(pPeer), 0x4A8))
    {
        if (rd32(reinterpret_cast<void*>(pPeer), 4) != 4) continue;
        uint8_t aBuf[5];
        aBuf[0] = 0x5C;
        const uint32_t nId = rd32(pUnit, 0xC);
        std::memcpy(aBuf + 1, &nId, 4);
        pEnv->sendPacket(reinterpret_cast<void*>(pPeer), aBuf, 5);
    }
}
