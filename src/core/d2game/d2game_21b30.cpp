// Decompile-derived, 1.13c-only. Seams: D2Common ordinals injected.
#include <cstdint>
#include <cstring>

struct D2Game21b30Env {
    int  (*getStat)(const void* unit, int stat, int layer);          // Ordinal 10973
    void*(*getStatList)(const void* unit, int flag);                 // Ordinal 10195
    void (*setStat)(void* list, int stat, int value, int layer);     // Ordinal 10188
};

// 1.13c D2Game+0x21B30
// fastcall(ecx unused, edx = unit). Folds stats 0x17/0x18 into 0x15/0x16 on the unit's stat list; skips type 0x10f.
extern "C" void d2_fn_d2game_21B30(uint32_t /*ecx*/, const void* unit, const D2Game21b30Env* env)
{
    int a = env->getStat(unit, 0x15, 0);
    int b = env->getStat(unit, 0x16, 0);
    uint32_t t = 0; if (unit) std::memcpy(&t, (const uint8_t*)unit + 4, 4);
    if (b > 0 && (unit == nullptr || t != 0x10f)) {
        int x = a - 1 + env->getStat(unit, 0x17, 0);
        int y = b - 1 + env->getStat(unit, 0x18, 0);
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        void* l = env->getStatList(unit, 1);
        env->setStat(l, 0x15, x, 0);
        env->setStat(l, 0x16, y, 0);
    }
}
