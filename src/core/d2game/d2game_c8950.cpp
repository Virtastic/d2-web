// 1.13c D2Game+0xC8950: EBX = block {game, player, esi}; every type-5 unit in the room list (room+0x74, next +0xE8) goes to 0xE1C00.
#include <cstdint>
struct GC8950Env { uint32_t (*fa478)(uint32_t s0); void (*fe1c00)(uint32_t eax, uint32_t esi, uint32_t s0); };
void g_c8950(uint32_t blk, const GC8950Env& env) {
    const uint32_t* b = (const uint32_t*)(uintptr_t)blk;
    uint32_t room = env.fa478(b[1]);
    for (uint32_t u = *(const uint32_t*)(uintptr_t)(room + 0x74); u; u = *(const uint32_t*)(uintptr_t)(u + 0xE8))
        if (*(const uint32_t*)(uintptr_t)u == 5) env.fe1c00(u, b[2], b[0]);
}
