// Decompile-derived, 1.13c-only. Seam: Fog pool allocator (D2Game+0xA2E6).
#include <cstdint>
#include <cstring>

struct D2Game20cf0Env { uint32_t (*alloc)(uint32_t pool, uint32_t size, const char* file, int line); };
static uint32_t r32(uint32_t base, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(base + o), 4); return v; }
static void w32(uint32_t base, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(base + o), &v, 4); }

// 1.13c D2Game+0x20CF0
// EAX = unit, EDI = source object (id at +0xC) or 0, stack arg = pGame (ret 4). For a player unit (type 1) pushes a new 8-byte link
// (id or -1, next) on the front of the list at pData(+0x14)->+0x28 block +0x34; the link comes from the Fog allocator (AiGeneral.cpp:0x1F3).
extern "C" void d2game_push_ai_link_20cf0(uint32_t unit, uint32_t src, uint32_t game, const D2Game20cf0Env* env)
{
    if (!unit || r32(unit, 0) != 1) return;
    const uint32_t pd = r32(unit, 0x14);
    const uint32_t blk = pd ? r32(pd, 0x28) : 0;
    const uint32_t p = env->alloc(r32(game, 0x1c), 8, "..\\Source\\D2Game\\Ai\\AiGeneral.cpp", 0x1f3);
    w32(p, 0, 0); w32(p, 4, 0);
    w32(p, 0, src ? r32(src, 0xc) : 0xffffffffu);
    if (r32(blk, 0x34)) w32(p, 4, r32(blk, 0x34));
    w32(blk, 0x34, p);
}
