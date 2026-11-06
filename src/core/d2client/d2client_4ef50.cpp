// 1.13c D2Client+0x4EF50: monster-mode spawn-drops step. Retail: stack (ret 8) = a0 unit, a1 packet {u16 id; +4 key}.
// Every callee is an injected seam; register arguments are named parameters. Rewritten 2026-11-05 from the asm: the old batch port passed `id` to the
// record lookup (0xCD60 takes buf[0], buf[1] and the caller's EBX), and 0x10003/0x10971 were really 0xD1EC(id or 0x2B0D) and 0xC22C(unit, result).
#include <cstdint>
#include <cstring>
namespace d2client_4ef50 {
struct Env {
    uint32_t (*cd8a)(uint32_t ecx, uint32_t edx, uint32_t id);
    uint32_t (*cd7e)(uint32_t ecx, uint32_t edx, uint32_t key, uint32_t r, uint32_t c390, uint32_t* buf);
    uint32_t (*cd60)(uint32_t g, uint32_t b0, uint32_t b1);
    void (*c550)(uint32_t unit, uint32_t v, uint32_t k, uint32_t zero, uint32_t str, uint32_t line);
    uint32_t (*c838)(uint32_t unit, uint32_t v, uint32_t c);
    void (*cdc0)(uint32_t x);
    uint32_t (*d1ec)(uint32_t ecx);
    void (*c22c)(uint32_t unit, uint32_t r);
    void (*f82fe0)(uint32_t unit);
};
struct G { uint32_t g9854, c390, player, rows; };   // 0x119854, byte 0x11C390, 0x11BBFC, *(*0xCE474)+0xBA0
static uint32_t rd(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; }
void sub_4EF50(const Env& e, const G& g, uint32_t ebx, uint32_t unit, const uint8_t* q) {
    if (!unit || !q) return;
    { uint32_t v = rd(unit, 0xC4) | 0x202; std::memcpy((void*)(uintptr_t)(unit + 0xC4), &v, 4); }
    uint16_t id16; std::memcpy(&id16, q, 2); const uint32_t id = id16;
    uint32_t key; std::memcpy(&key, q + 4, 4);
    const uint32_t r = e.cd8a(g.g9854, 0, id);
    uint32_t buf[9] = {};
    if (!e.cd7e(g.g9854, g.player, key, r, g.c390 & 0xff, buf)) return;
    const uint32_t b0 = buf[0], b1 = buf[1];
    const uint32_t rec = e.cd60(g.g9854, b0, b1);
    if (!rec) return;
    int32_t d = (int32_t)(b1 - rd(rec, 0x1C)); if (d < 0) d = 0;
    for (uint32_t i = 0; i < 6; ++i) {
        const int32_t v = (int32_t)rd(rec, 0x78 + 4 * i);
        if (v <= 0 || v >= (int32_t)g.rows) break;
        const int32_t m = *(const int8_t*)(uintptr_t)(rec + 0xCC + i);
        int32_t k = ((m * d) >> 5) + *(const int8_t*)(uintptr_t)(rec + 0xC6 + i);
        if (k >= 0x20) k = 0x20;
        const int32_t c = *(const int8_t*)(uintptr_t)(rec + 0xC0 + i);
        e.c550(unit, (uint32_t)v, (uint32_t)k, 0, 0x6fb86030u, 0x9c7);
        e.cdc0(e.c838(unit, (uint32_t)v, (uint32_t)c));
    }
    e.c22c(unit, e.d1ec(id == 0x421 ? 0x2b0d : id));
    e.f82fe0(unit);
}
}
