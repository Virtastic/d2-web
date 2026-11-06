// 1.13c D2Game+0xD1A30 (size 26): ECX = unit, EAX also in play. The 0xA8C8 import is called with
// push eax, push ecx, so its first stack word is ECX and its second is EAX (ret 8).
// Result 0 when the import returns 2 or 7, else 1.
#include <cstdint>
struct GD1a30Env { int32_t (*o10787)(uint32_t s0, uint32_t s1); };
uint32_t g_d1a30(uint32_t eax, uint32_t ecx, const GD1a30Env& env) {
    int32_t r = env.o10787(ecx, eax);
    if (r != 2 && r != 7) return 1;
    return 0;
}
