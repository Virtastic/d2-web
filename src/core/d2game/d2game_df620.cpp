// 1.13c D2Game+0xDF620, injectable callees. ESI = unit, stack word 0 = game (ret 4). Walks the game's bucket
// list (game+0xBC + 4*(byte of 0xA56E) = head, next at +0x7C) for the node 0xA6DC returned for unit+0x2C;
// returns it when found, else releases the reference (0xA646) and returns 0. Every callee takes one stack word:
// 0xA6DC(g2c), 0xA478(unit), 0xA556(a478 result), 0xA56E(a556 result), 0xA5C8(bucket head), 0xA646(g2c).
#include <cstdint>
struct GDF620Env {
    uint32_t (*a6dc)(uint32_t g2c);
    uint32_t (*a478)(uint32_t unit);
    uint32_t (*a556)(uint32_t v);
    uint32_t (*a56e)(uint32_t v);
    uint32_t (*a5c8)(uint32_t head);
    void     (*a646)(uint32_t g2c);
};
// 1.13c D2Game+0xDF620
uint32_t g_df620(uint32_t unit, uint32_t game, const GDF620Env& env)
{
    const uint32_t g2c = *(const uint32_t*)(uintptr_t)(unit + 0x2C);
    const uint32_t want = env.a6dc(g2c);
    const uint32_t h = env.a556(env.a478(unit));
    if (h) {
        const uint32_t idx = env.a56e(h) & 0xFF;
        uint32_t n = env.a5c8(*(const uint32_t*)(uintptr_t)(game + 0xBC + idx * 4));
        while (n) {
            if (n == want) return want;
            n = *(const uint32_t*)(uintptr_t)(n + 0x7C);
        }
    }
    env.a646(g2c);
    return 0;
}
