// 1.13c D2Game+0x2D500 (343 B, EAX = pClient, [esp+4] = pGame, [esp+8] = nFlush, ret 8): the client's outgoing packet pump. Null game / client are the 0xEE2 / 0xEE3 asserts.
// With nFlush and client state (+4) == 4 it first runs 0x2C060 (when the module switch 0x6FD31C38 is set) and 0x2BE20 (ESI = client); in a live game (byte +0x6A is 1 or 2) it
// runs 0x2D140 (EAX = client, [esp+4] = game). Then it pops nodes off the client queue (head +0x1B8, tail +0x1BC, node: +0 length, +4 data, +0x204 next) and sends each through
// 0xA35E(1, id, data, len): a send (non-zero) recycles the node onto the free list (+0x1C0) and clears the failure count (+0x1C4); a failure (0xA388 runs) counts it, puts the node
// back at the head and stops, and the third failure drops the client through 0x2D090 (ESI = client, [esp+4] = game, [esp+8] = 0). A live game stops after 3 sends.
#include <cstdint>
#include <cstring>

struct D2Game2D500Env {
    uint32_t (*switchOn)();                                        // module global 0x6FD31C38
    void (*fn2C060)(void* pClient);                                // 0x2C060 (EAX = client)
    void (*fn2BE20)(void* pClient);                                // 0x2BE20 (ESI = client)
    void (*fn2D140)(void* pClient, void* pGame);                   // 0x2D140
    void (*kick)(void* pClient, void* pGame, uint32_t nFlag);      // 0x2D090
    int (*send)(uint32_t nOne, uint32_t nId, const uint8_t* pData, uint32_t nLen);  // 0xA35E
    void (*sendFailed)();                                          // 0xA388
    void (*fatal)(uint32_t nLine);
};

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, static_cast<const uint8_t*>(p) + o, 4); return v; }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy(static_cast<uint8_t*>(p) + o, &v, 4); }

// 1.13c D2Game+0x2D500
void D2Game_2D500(void* pClient, void* pGame, uint32_t nFlush, const D2Game2D500Env* pEnv)
{
    if (pGame == nullptr) { pEnv->fatal(0xEE2); return; }
    if (pClient == nullptr) { pEnv->fatal(0xEE3); return; }
    uint8_t* c = static_cast<uint8_t*>(pClient);
    const uint8_t* g = static_cast<const uint8_t*>(pGame);
    if (nFlush != 0 && rd32(c, 4) == 4)
    {
        if (pEnv->switchOn() != 0) pEnv->fn2C060(pClient);
        pEnv->fn2BE20(pClient);
    }
    if (g[0x6A] == 1 || g[0x6A] == 2) pEnv->fn2D140(pClient, pGame);
    int nSent = 0;
    bool bFailed = false;
    for (;;)
    {
        const uint32_t nHead = rd32(c, 0x1B8);
        if (rd32(c, 0x1BC) == nHead) { wr32(c, 0x1BC, 0); wr32(c, 0x1B8, 0); }
        if (nHead == 0) return;
        uint8_t* pNode = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(nHead));
        wr32(c, 0x1B8, rd32(pNode, 0x204));
        const uint32_t nLen = rd32(pNode, 0);
        if (nLen == 0) return;
        if (pEnv->send(1, rd32(c, 0), pNode + 4, nLen) != 0)
        {
            wr32(pNode, 0, 0);
            wr32(pNode, 0x204, rd32(c, 0x1C0));
            ++nSent;
            wr32(c, 0x1C0, nHead);
            wr32(c, 0x1C4, 0);
        }
        else
        {
            pEnv->sendFailed();
            wr32(c, 0x1C4, rd32(c, 0x1C4) + 1);
            wr32(pNode, 0x204, 0);
            bFailed = true;
            if (rd32(c, 0x1B8) != 0) wr32(c, 0x1BC, nHead);
            wr32(c, 0x1B8, nHead);
            if (static_cast<int32_t>(rd32(c, 0x1C4)) >= 3) { pEnv->kick(pClient, pGame, 0); return; }
        }
        if ((g[0x6A] == 1 || g[0x6A] == 2) && nSent >= 3) return;
        if (bFailed) return;
    }
}
