// High priest heal-target scan callback (1.13c D2Game+0x15660). ECX = game (unused), EDX = scanning unit, stack = candidate, stack = arg block
// { best target (+0), best life value (+4), max squared distance (+8) }; ret 8. A candidate is taken only when it is a monster (type 1), not dead,
// aligned with the scanner, evil (alignment 0), within the squared distance, with a life value (ordinal 11027) under 0x4C and under the best so far.
// Always returns 0 (the scan continues). Retail is 32-bit: the callees are seams in retail argument order, the engine-hosted copy is
// AITHINK_TargetCallback_HighPriest in AiThink.cpp.
#include <cstdint>
#include <cstring>

struct D2G15660Env {
    int32_t (*isDead)(void* target);                      // 0xA688 (D2Common 10648)
    int32_t (*aligned)(void* unit, void* target);         // 0xA6F4 (10666)
    int32_t (*alignment)(void* target);                   // 0xA514 (10830)
    int32_t (*sqDist)(void* unitEax, void* targetEbx);    // 0x1900, EAX = unit, EBX = target
    int32_t (*lifeValue)(void* target);                   // 0xA718 (11027)
};

// 1.13c D2Game+0x15660
extern "C" uint32_t d2g_15660(void* /*game*/, void* unit, void* target, uint8_t* arg, const D2G15660Env* env) {
    uint32_t type = 0;
    if (!target) return 0;
    std::memcpy(&type, target, 4);
    if (type != 1) return 0;
    if (env->isDead(target)) return 0;
    if (!env->aligned(unit, target)) return 0;
    if (env->alignment(target)) return 0;
    int32_t maxDist; std::memcpy(&maxDist, arg + 8, 4);
    if (env->sqDist(unit, target) > maxDist) return 0;
    const int32_t v = env->lifeValue(target);
    int32_t best; std::memcpy(&best, arg + 4, 4);
    if (v > 0x4B || v >= best) return 0;
    std::memcpy(arg + 4, &v, 4);
    const uint32_t t = (uint32_t)(uintptr_t)target;
    std::memcpy(arg, &t, 4);
    return 0;
}
