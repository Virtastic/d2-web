// 1.13c D2Game+0x4AD80: CLIENTS_BuildUnitsUpdateList, EBX = client, [esp+4] = max count, ret 4. The client's room
// handle (+0x1B4) is required (assert 0x6F7) and D2Common #10383 (0xA55C: handle, &list, &count) must yield a room list (assert 0x6F9). The client's
// player comes from 0x4AD10(0) (ESI = client); without one nothing happens. Otherwise x = 0x1100(player) and y = 0x1130(player) (ECX = unit),
// client+0x3BC = 0, every unit on each room entry's +0x74 chain (next at +0xE8) goes through 0x4A460 (EAX = x, ESI = node array, stack: unit, y,
// &best, client, max) and 0x4A370 (ESI = client, stack: node array) writes the sorted list into client+0x1CC. The node array (12-byte entries) and
// best are stack locals; best starts 0, node 0 is zero. Pointers are retail's 32-bit words.
#include <cstdint>
#include <cstring>

struct D2Game4AD80Env {
    void (*roomList)(uint32_t nHandle, uint32_t* ppList, uint32_t* pCount);        // 0xA55C (#10383, stack 3; out pointers written)
    uint8_t* (*getPlayer)(uint8_t* pClient, uint32_t nIgnore);                     // 0x4AD10 (ESI = client, stack arg 0)
    uint32_t (*unitX)(uint8_t* pUnit);                                             // 0x1100 (ECX = unit)
    uint32_t (*unitY)(uint8_t* pUnit);                                             // 0x1130 (ECX = unit)
    void (*insert)(uint8_t* pUnit, uint32_t nY, uint32_t* pBest, uint8_t* pClient, uint32_t nMax, uint32_t* pNodes, uint32_t nX); // 0x4A460
    void (*flush)(uint32_t* pNodes, uint8_t* pClient);                             // 0x4A370
    void (*fatal)(uint32_t nLine);
};

static inline uint32_t rd32(const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }
static inline uint8_t* ptr32(const uint8_t* p, uint32_t o) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(rd32(p, o))); }

// 1.13c D2Game+0x4AD80
void D2Game_4AD80(uint8_t* pClient, uint32_t nMax, const D2Game4AD80Env* pEnv)
{
    if (rd32(pClient, 0x1B4) == 0) { pEnv->fatal(0x6F7); return; }
    uint32_t pList = 0, nCount = 0;
    pEnv->roomList(rd32(pClient, 0x1B4), &pList, &nCount);
    if (pList == 0) { pEnv->fatal(0x6F9); return; }
    uint8_t* pPlayer = pEnv->getPlayer(pClient, 0);
    if (pPlayer == nullptr) return;
    uint32_t aNodes[0x38 * 3 + 3];
    std::memset(aNodes, 0, sizeof aNodes);
    const uint32_t nX = pEnv->unitX(pPlayer);
    const uint32_t nY = pEnv->unitY(pPlayer);
    uint32_t nBest = 0;
    const uint32_t zero = 0;
    std::memcpy(pClient + 0x3BC, &zero, 4);
    for (uint32_t i = 0; i < nCount; ++i) {
        const uint8_t* pEntry = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(rd32(reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(pList)), i * 4)));
        for (uint8_t* pUnit = ptr32(pEntry, 0x74); pUnit != nullptr; pUnit = ptr32(pUnit, 0xE8))
            pEnv->insert(pUnit, nY, &nBest, pClient, nMax, aNodes, nX);
    }
    pEnv->flush(aNodes, pClient);
}
