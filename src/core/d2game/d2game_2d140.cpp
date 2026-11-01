// 1.13c D2Game+0x2D140 (339 B, EAX = pClient, [esp+4] = nArg, ret 4): streams the client's pending save blob (+0x17C, length +0x180, sent so far
// +0x188) in packets of at most 0xFF bytes: {0xB3, n, first, total(4), bytes}. Paced by the counter +0x184 (reloaded with 5); a failed send
// counts in +0x1C4, three in a row drop the blob (0x49630) and call 0x2D090. A null client is the 0xB8A assert.
#include <cstdint>
#include <cstring>

struct D2Game2D140Env {
    uint8_t* (*alloc)(uint32_t nPool, uint32_t nSize, uint32_t nLine);           // Fog #10045 (0xA2E6): ECX = pool, EDX = size
    int (*send)(uint32_t nClientId, const uint8_t* pPkt, uint32_t nLen);        // Fog #10002 (0xA35E)
    void (*free)(uint32_t nPool, uint8_t* pPkt, uint32_t nLine);                // Fog #10046 (0xA2EC)
    void (*dropBlob)(void* pClient);                                            // 0x49630 (ESI = client)
    void (*notify)(void* pClient, uint32_t nArg);                               // 0x2D090 (ESI = client, [esp+4] = arg, [esp+8] = 0)
    void (*fatal)(uint32_t nLine);
};

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, static_cast<const uint8_t*>(p) + o, 4); return v; }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy(static_cast<uint8_t*>(p) + o, &v, 4); }

// 1.13c D2Game+0x2D140
void D2Game_2D140(void* pClient, uint32_t nArg, const D2Game2D140Env* pEnv)
{
    if (pClient == nullptr) { pEnv->fatal(0xB8A); return; }
    uint8_t* c = static_cast<uint8_t*>(pClient);
    if ((c[0x3D4] & 0x10) == 0 || rd32(c, 0x17C) == 0) return;
    if (rd32(c, 0x184) != 0) { wr32(c, 0x184, rd32(c, 0x184) - 1); return; }
    int32_t n = static_cast<int32_t>(rd32(c, 0x180) - rd32(c, 0x188));
    wr32(c, 0x184, 5);
    const uint8_t* src = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(rd32(c, 0x188) + rd32(c, 0x17C)));
    if (n >= 0xFF) n = 0xFF;
    const uint32_t nPool = rd32(reinterpret_cast<void*>(static_cast<uintptr_t>(rd32(c, 0x1A8))), 0x1C);
    uint8_t* pkt = pEnv->alloc(nPool, 0x107, 0x8B2);
    pkt[0] = 0xB3;
    pkt[1] = static_cast<uint8_t>(n);
    pkt[2] = rd32(c, 0x188) == 0;
    wr32(pkt, 3, rd32(c, 0x180));
    for (int32_t i = 0; i < n; ++i) pkt[7 + i] = src[i];
    if (pEnv->send(rd32(c, 0), pkt, static_cast<uint32_t>(n + 7)) != 0)
    {
        const uint32_t nNew = rd32(c, 0x188) + static_cast<uint32_t>(n);
        wr32(c, 0x188, nNew);
        if (nNew == rd32(c, 0x180)) pEnv->dropBlob(pClient);
        wr32(c, 0x1C4, 0);
    }
    else wr32(c, 0x1C4, rd32(c, 0x1C4) + 1);
    pEnv->free(nPool, pkt, 0x8D8);
    if (static_cast<int32_t>(rd32(c, 0x1C4)) >= 3)
    {
        pEnv->dropBlob(pClient);
        pEnv->notify(pClient, nArg);
    }
}
