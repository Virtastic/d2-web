// 1.13c D2Common+0x39CC0 (#10379): stdcall(a0, a1, a2), ret 0xC. Retail-layout (32-bit) reader. Loads EAX = *(a0 + 0x5C) (the unit's
// stat list), ESI = a1 (stat id), pushes a2 (layer) and calls 0x39380 (register EAX/ESI + one stack word, ret 4); its result is returned.
// The callee is a seam. Prior art: none (D2MOO has no wrapper at this site).
#include <cstdint>
#include <cstring>

struct D2Common39CC0Env { int32_t (*statDelta)(uint32_t list, int32_t id, uint32_t layer); };

extern "C" int32_t D2Common_39CC0_Raw(uint32_t a0, int32_t a1, uint32_t a2, const D2Common39CC0Env* env)
{
    uint32_t list; memcpy(&list, reinterpret_cast<const void*>(static_cast<uintptr_t>(a0 + 0x5C)), 4);
    return env->statDelta(list, a1, a2);
}
