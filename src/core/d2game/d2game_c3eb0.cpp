// 1.13c D2Game+0xC3EB0 -- ACT2Q2_Callback10_PlayerLeavesGame on retail's 32-bit layout (the vendored typed form is in
// d2moo/src/D2Game/QUESTS/ACT2/A2Q2.cpp). The 14-byte body is push ebx; push esi; mov ebx,edx; mov esi,ecx; call 0x340C0:
// fastcall ECX = the unit (ESI), EDX = the quest argument (EBX), both forwarded unchanged to QUESTS_RemovePlayerGuid. No 1.10f delta.
#include <cstdint>

struct D2G340C0Env {
    uint32_t (*records)(uint32_t p);
    uint32_t (*flag)(uint32_t rec, uint32_t id, uint32_t bit);
    void (*removeGuid)(uint32_t guid, uint32_t list);
};
void QUESTS_RemovePlayerGuid(uint32_t ctx, uint32_t unit, const D2G340C0Env& env);

void ACT2Q2_Callback10_PlayerLeavesGame32(uint32_t ecx, uint32_t edx, const D2G340C0Env& env)
{
    QUESTS_RemovePlayerGuid(edx, ecx, env);
}
