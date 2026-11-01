// 1.13c D2Game+0x2CD10 (27 B, register convention EAX / ESI): locks the object in ESI through 0x29820 (EAX = ESI; it takes
// the 0x2A9F0 callback in EDI), then tail-jumps 0xE20A0 with ECX = ESI and EDX = the incoming EAX, whose value is the result.
#include <cstdint>

struct D2Game2CD10Env {
    void (*fn29820)(uint32_t eax);                              // 0x29820 (plain ret)
    uint32_t (*fnE20A0)(uint32_t ecx, uint32_t edx);            // 0xE20A0 (plain ret)
};

// 1.13c D2Game+0x2CD10
uint32_t D2Game_2CD10(uint32_t nEax, uint32_t nEsi, const D2Game2CD10Env* pEnv)
{
    pEnv->fn29820(nEsi);
    return pEnv->fnE20A0(nEsi, nEax);
}
