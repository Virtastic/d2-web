// Remove every loose item (1.13c D2Game+0x13180; 1.10f ITEMS_RemoveAll). EDI is the game; plain ret. The 128
// item-unit buckets at game+0x1720 are walked; an item in mode 6 (socketed) is skipped along its +0xE4 list link,
// any other is removed through 0xE20A0 (ECX = game, EDX = unit) and the bucket is re-read from its head.
#include <cstdint>
#include <cstring>

struct D2G13180Env {
    void (*removeUnit)(void* game, void* unit);   // 0xE20A0, fastcall
};

static inline void* rdp32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return (void*)(uintptr_t)v; }

// 1.13c D2Game+0x13180
extern "C" void d2g_13180(void* game, const D2G13180Env* env)
{
    for (uint32_t i = 0; i < 128; ++i) {
        const uint32_t slot = 0x1720 + i * 4;
        void* unit = rdp32(game, slot);
        while (unit != nullptr) {
            uint32_t mode; std::memcpy(&mode, (const uint8_t*)unit + 0x10, 4);
            if (mode == 6) {
                unit = rdp32(unit, 0xE4);
            } else {
                env->removeUnit(game, unit);
                unit = rdp32(game, slot);
            }
        }
    }
}
