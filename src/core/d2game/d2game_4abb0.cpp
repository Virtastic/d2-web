// D2Game+0x4ABB0 -- tell a client a unit left unless its room is still in the client's room list.
// ebx = unit, stack client ret 4. A null client is fatal (0x83f), a null client+0x1b4 is fatal (0x845).
// #a55c(client+0x1b4, &list, &count) yields the rooms; a null entry met before a match is fatal (0x84a).
// No match and unit type byte != 3: packet {0x0a, type, dword unit id} (6 bytes) via 0x8A3E0(eax client; &pkt, 6).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game4ABB0Env {
    void     (*fatal)(uint32_t line);
    uint32_t (*a478)(uint32_t unit);
    void     (*a55c)(uint32_t roomsOwner, uint32_t* list, uint32_t* count);
    void     (*sub8A3E0)(uint32_t client, uint32_t pkt, uint32_t len);
};

static inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

// 1.13c D2Game+0x4ABB0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_4abb0(uint32_t unit, uint32_t client, const D2Game4ABB0Env* env)
{
    if (client == 0) { env->fatal(0x83f); return; }
    const uint32_t room = env->a478(unit);
    const uint32_t owner = rd32(client + 0x1b4);
    if (owner == 0) { env->fatal(0x845); return; }
    uint32_t list = 0, count = 0;
    env->a55c(owner, &list, &count);
    for (uint32_t i = 0; i < count; ++i) {
        const uint32_t e = rd32(list + i * 4);
        if (e == 0) { env->fatal(0x84a); return; }
        if (e == room) return;
    }
    const uint8_t type = *(const uint8_t*)(uintptr_t)unit;
    if (type == 3) return;
    uint8_t pkt[6] = { 0x0a, type };
    const uint32_t id = rd32(unit + 0xc);
    std::memcpy(pkt + 2, &id, 4);
    env->sub8A3E0(client, (uint32_t)(uintptr_t)pkt, 6);
}
