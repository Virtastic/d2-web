// D2Game+0x26A30: dual-wield test. EAX = unit; true when the unit's inventory holds composit items in
// slots 6 and 5 and both are item type 0x2E. Both slots are read before either type is checked.
// The D2Common calls (ordinal 10062 GetCompositItem, 10744 CheckItemType) arrive through the env.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game26A30Env {
    uint32_t (*getCompositItem)(uint32_t inventory, uint32_t slot);   // thunk 0xB2A0
    uint32_t (*checkItemType)(uint32_t item, uint32_t type);          // thunk 0xA430
};

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }

// 1.13c D2Game+0x26A30
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2game_26a30(uint32_t unit, const D2Game26A30Env* env)
{
    const uint32_t inv = rd32(unit, 0x60);
    if (!inv) return 0;
    const uint32_t first = env->getCompositItem(inv, 6);
    const uint32_t second = env->getCompositItem(inv, 5);
    if (!first || !env->checkItemType(first, 0x2e) || !second) return 0;
    return env->checkItemType(second, 0x2e) ? 1 : 0;
}
