// 1.13c D2Game+0xE0A60: are two units hostile, after each is replaced by its owner (22 callers).
// EAX = pUnitB, ECX = pUnitA, stack = pGame, ret 4. A is replaced once (a monster's owner record, or a missile with flag
// bit 10); B follows its monster-owner chain until the lookup fails or hands B back. Equal: 0; two players: 0xF02A0's
// value; otherwise the D2Common thunk 0xA6F4(A, B) inverted.
#include <cstdint>
struct GE0A60Env {
    uint32_t (*find)(uint32_t type, uint32_t game, uint32_t id);   // 0xE03A0 (EAX type, ECX game, EDX id)
    uint32_t (*players)(uint32_t a, uint32_t b);                    // 0xF02A0 (EDI a, ESI b, stack 8)
    uint32_t (*thunk)(uint32_t a, uint32_t b);                      // D2Common 0xA6F4 (stdcall)
};
static uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
// 1.13c D2Game+0xE0A60
uint32_t g_e0a60(uint32_t b, uint32_t a, uint32_t game, const GE0A60Env& env)
{
    if (a != 0) {
        uint32_t type = 0, id = 0; bool look = false;
        if (rd32(a) == 1) {
            const uint32_t o = rd32(a + 0x14);
            const uint32_t q = o ? rd32(o + 0x28) : 0;
            if (q != 0 && rd32(q + 0x28) != 0) { game = rd32(q + 0x28); type = rd32(q + 0x30); id = rd32(q + 0x2C); look = true; }
        } else if (rd32(a) == 3 && ((rd32(a + 0xC8) >> 10) & 1) != 0) {
            type = rd32(a + 0x94); id = rd32(a + 0x98); look = true;
        }
        if (look) { const uint32_t r = env.find(type, game, id); if (r != 0) a = r; }
    }
    while (b != 0 && rd32(b) == 1) {
        const uint32_t o = rd32(b + 0x14);
        if (o == 0) break;
        const uint32_t q = rd32(o + 0x28);
        if (q == 0 || rd32(q + 0x28) == 0) break;
        const uint32_t r = env.find(rd32(q + 0x30), rd32(q + 0x28), rd32(q + 0x2C));
        if (r == 0 || r == b) break;
        b = r;
    }
    if (a == b) return 0;
    if (a != 0 && rd32(a) == 0 && b != 0 && rd32(b) == 0) return env.players(a, b);
    return env.thunk(a, b) == 0 ? 1 : 0;
}
