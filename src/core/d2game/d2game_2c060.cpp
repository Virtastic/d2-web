// 1.13c D2Game+0x2C060 (133 B, register convention EAX = pClient): when the client has a +0x1B8 block and +0x1B0 >= 10, and the table
// 0x4AD10(0) is present, derives a count n = (499 - block[0]) / 9 (signed). Non-positive: nothing. 1..0x37: 0x4AD80(n) (ECX = ?, EBX = pClient,
// [esp+4] = n, ret 4), then 0x8A2A0 (EDX = pClient) is tail-called. n above 0x37 is the 0xECE assert; a non-zero first dword of the table the 0xEC4 assert.
#include <cstdint>
#include <cstring>

struct D2Game2C060Env {
    int32_t* (*fn4AD10)(uint32_t nArg);               // 0x4AD10 (stack arg 0, ret 4)
    void (*fn4AD80)(uint32_t nCount, void* pClient);  // 0x4AD80 (EBX = client, [esp+4] = count, ret 4)
    void (*fn8A2A0)(void* pClient);                   // 0x8A2A0 (EDX = client, plain ret)
    void (*fatal)(uint32_t nLine);
};

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, static_cast<const uint8_t*>(p) + o, 4); return v; }

// 1.13c D2Game+0x2C060
void D2Game_2C060(void* pClient, const D2Game2C060Env* pEnv)
{
    const int32_t* pBlock = reinterpret_cast<const int32_t*>(static_cast<uintptr_t>(rd32(pClient, 0x1B8)));
    if (pBlock == nullptr) return;
    const int32_t* pTable = pEnv->fn4AD10(0);
    if (pTable == nullptr || static_cast<int32_t>(rd32(pClient, 0x1B0)) < 10) return;
    if (*pTable != 0) { pEnv->fatal(0xEC4); return; }
    const int32_t nCount = (499 - *pBlock) / 9;
    if (nCount < 1) return;
    if (static_cast<uint32_t>(nCount) >= 0x38) { pEnv->fatal(0xECE); return; }
    pEnv->fn4AD80(static_cast<uint32_t>(nCount), pClient);
    pEnv->fn8A2A0(pClient);
}
