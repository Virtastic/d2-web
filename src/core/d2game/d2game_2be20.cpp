// 1.13c D2Game+0x2BE20 (168 B, register convention ESI = pClient): re-evaluates the client's tracked unit. The unit is client+0x174
// directly when bit 0 of client+0x3D4 is set, else (non-zero +0x174) looked up through 0xE03A0 (EAX = +0x16C, EDX = +0x170, ECX = +0x1A8)
// and +0x174 cleared when it is gone. A live unit with a non-zero first dword is the 0xEB0 assert. The "urgent" flag for 0xCC030 is set
// when the client has a +0x1B8 block and +0x1B0 >= 10, or +0x1B0 >= 20; a non-zero 0xCC030 result resets +0x1B0.
#include <cstdint>
#include <cstring>

struct D2Game2BE20Env {
    void* (*getUnit)(uint32_t nEax, uint32_t nEdx, uint32_t nEcx);   // 0xE03A0 (plain ret)
    int32_t (*fnCC030)(void* pUnit, uint32_t nFlag);                  // 0xCC030 (ECX = unit, [esp+4] = flag, ret 4)
    void (*fatal)(uint32_t nLine);
};

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, static_cast<const uint8_t*>(p) + o, 4); return v; }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy(static_cast<uint8_t*>(p) + o, &v, 4); }

// 1.13c D2Game+0x2BE20
void D2Game_2BE20(void* pClient, const D2Game2BE20Env* pEnv)
{
    void* pUnit;
    if ((static_cast<const uint8_t*>(pClient)[0x3D4] & 1) == 0)
    {
        if (rd32(pClient, 0x174) == 0) return;
        pUnit = pEnv->getUnit(rd32(pClient, 0x16C), rd32(pClient, 0x170), rd32(pClient, 0x1A8));
        if (pUnit == nullptr) wr32(pClient, 0x174, 0);
    }
    else
    {
        pUnit = reinterpret_cast<void*>(static_cast<uintptr_t>(rd32(pClient, 0x174)));
    }
    if (pUnit == nullptr) return;

    const int32_t nCount = static_cast<int32_t>(rd32(pClient, 0x1B0));
    uint32_t nFlag = 0;
    if (rd32(pClient, 0x1B8) != 0 && nCount >= 10) nFlag = 1;
    if (nCount >= 20) nFlag = 1;
    if (rd32(pUnit, 0) != 0) { pEnv->fatal(0xEB0); return; }
    if (pEnv->fnCC030(pUnit, nFlag) != 0) wr32(pClient, 0x1B0, 0);
}
