// 1.13c D2Client+0x9CCE0: EAX = unit selector (passed straight to 0x9CAD0), stack word 0 = kind (ret 4).
// rec = 0x9CAD0(EAX); NULL returns 0; kind 2 returns rec[3]. Otherwise up to 0x14 rounds: v = rec[0]; kind 0 with rec[1]
// set steps the player seed (64-bit at player+0x20: lo = lo*0x6AC690C5 + hi) and takes rec[1] when the new low bit is 0;
// rec[2] set and (v == 0 or a second step has low32 % 3 == 0) takes rec[2] + (0, 1, 2) from Ordinal 10396(global 0x11C3B8)
// (1: 0, 2..3: 1, else 2); v = 0x5EE70(v); repeat while v == rec[4]. rec[4] = v, rec[5] = tick (global 0x1197F8).
// 0x9CAD0, 0x5EE70 and the Ordinal 10396 thunk (0xC7E4, one word popped by the callee) are injected.
#include <cstdint>
#include <cstring>

struct D2c0x9cce0Env {
    uint8_t* (*f9cad0)(uint32_t eax);
    int32_t (*thunk_c7e4)(uint32_t arg);
    int32_t (*f5ee70)(int32_t v);
};

static inline uint32_t rd32_9cce0(const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }
static inline void wr32_9cce0(uint8_t* p, uint32_t o, uint32_t v) { std::memcpy(p + o, &v, 4); }

extern "C" int32_t d2c_port_9cce0(uint32_t eax, int32_t kind, uint8_t* player, uint32_t g11c3b8, uint32_t g1197f8, const D2c0x9cce0Env& e) {
    uint8_t* rec = e.f9cad0(eax);
    if (!rec) return 0;
    if (kind == 2) return (int32_t)rd32_9cce0(rec, 12);
    auto step = [&]() {
        uint64_t s = (uint64_t)rd32_9cce0(player, 0x20) * 0x6AC690C5ull + rd32_9cce0(player, 0x24);
        wr32_9cce0(player, 0x20, (uint32_t)s);
        wr32_9cce0(player, 0x24, (uint32_t)(s >> 32));
        return (uint32_t)s;
    };
    int32_t v;
    uint32_t n = 0;
    do {
        v = (int32_t)rd32_9cce0(rec, 0);
        if (kind == 0 && rd32_9cce0(rec, 4) != 0 && (step() & 1) == 0) v = (int32_t)rd32_9cce0(rec, 4);
        if (rd32_9cce0(rec, 8) != 0 && (v == 0 || step() % 3 == 0)) {
            int32_t t = e.thunk_c7e4(g11c3b8);
            v = (int32_t)rd32_9cce0(rec, 8) + (t == 1 ? 0 : (t < 2 || t > 3) ? 2 : 1);
        }
        v = e.f5ee70(v);
    } while (v == (int32_t)rd32_9cce0(rec, 16) && ++n < 0x14);
    wr32_9cce0(rec, 16, (uint32_t)v);
    wr32_9cce0(rec, 20, g1197f8);
    return v;
}
