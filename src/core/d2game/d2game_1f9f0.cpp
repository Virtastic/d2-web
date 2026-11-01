// Decompile-derived, 1.13c-only (convention and frame read off the disassembly). Seams: the record decoder (#11105, D2Game+0xB360),
// the unit-from-record builder (D2Game+0xEAE0), the inventory get/append (#10387 +0xA448, #10405 +0xA9B8), the per-item place
// (D2Game+0xD47C0) and the fatal path.
#include <cstdint>
#include <cstring>

struct D2Game1f9f0Env {
    void (*decode)(uint32_t buf, uint32_t len, uint32_t one, uint32_t* rec6);            // #11105: fills a 6-dword header, +0x14 = item count
    uint32_t (*build)(uint32_t game, uint32_t first, uint32_t buf, uint32_t len, uint32_t one,
                      int32_t* consumed, uint32_t flags, uint32_t* rec6);              // 0xEAE0: EDI = game, ESI = &header, 6 stack words (ret 0x18)
    uint32_t (*inventory)(uint32_t zero, uint32_t unit);                                 // #10387
    void (*append)(uint32_t inv, uint32_t item, uint32_t one);                           // #10405
    void (*place)(uint32_t unit, uint32_t item, uint32_t zero);                          // 0xD47C0: ESI = unit, EDI = item, 1 stack word
    void (*fatal)(uint32_t id);                                                          // Fog #10265 / #10024 / exit(-1)
};
static uint32_t r32(uint32_t b, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(b + o), 4); return v; }
static void w32(uint32_t b, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(b + o), &v, 4); }

// 1.13c D2Game+0x1F9F0
// stdcall (game, first, node), ret 0xC. The node's stream (length word +0xC, bytes from +0xE) rebuilds the unit; its path block
// (+0x14) +0x24 takes node+0x04, raised to game+0xA8 + 15000 when nonzero and below it. Then header-count items are rebuilt from the
// rest of the stream, each appended to the unit's inventory (fetched once, #10387 when +0x60 is empty) and placed. Returns the unit or 0.
extern "C" uint32_t d2game_restore_inactive_1f9f0(uint32_t game, uint32_t first, uint32_t node, const D2Game1f9f0Env* env)
{
    uint32_t len = *(const uint16_t*)(uintptr_t)(node + 0xc);
    uint32_t buf = node + 0xe;
    uint32_t hdr[6] = {};
    int32_t consumed = (int32_t)first;
    env->decode(buf, len, 1, hdr);
    const uint32_t unit = env->build(game, first, buf, len, 1, &consumed, 0x60, hdr);
    if (!unit) return 0;
    buf += (uint32_t)consumed; len -= (uint32_t)consumed;
    const int32_t t = (int32_t)r32(node, 4);
    const int32_t lim = (int32_t)(r32(game, 0xa8) + 15000u);
    w32(r32(unit, 0x14), 0x24, (uint32_t)((t != 0 && t < lim) ? lim : t));
    const int32_t count = (int32_t)hdr[5];
    if (!count) return unit;
    uint32_t inv = r32(unit, 0x60);
    if (!inv) inv = env->inventory(0, unit);
    for (int32_t i = 0; i < count; ++i) {
        env->decode(buf, len, 1, hdr);
        const uint32_t item = env->build(game, 0, buf, len, 1, &consumed, 0x60, hdr);
        if (!item) { env->fatal(0x10d); return unit; }
        buf += (uint32_t)consumed; len -= (uint32_t)consumed;
        env->append(inv, item, 1);
        env->place(unit, item, 0);
    }
    return unit;
}
