// 1.13c D2Client+0x74350 (303 B): type-1 message for skilldesc record p2 on unit p1, then zero the spawned object's velocity.
// ECX = p1 (unit), EDX = p2 (skilldesc index), stack (ret 4): s0 = p3. tbl = *sgptDataTables (+0xB98 skilldesc rows of 0x23C,
// +0xBA0 count, +0xB6C missile count).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

namespace {
inline uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
inline void wr32(uint32_t a, uint32_t v) { *(uint32_t*)(uintptr_t)a = v; }
inline int32_t rs16(uint32_t a) { return *(const int16_t*)(uintptr_t)a; }
}

struct D2c74350Env {
    uint32_t (*a5f70)();                                   // 0xA5F70: no arguments, returns the player object
    uint32_t (*c1630)(uint32_t ecx);
    uint32_t (*c1660)(uint32_t ecx);
    uint32_t (*b30f0)(const uint32_t* msg);                // 0xB30F0, returns the spawned object
    void (*c7e30)(uint32_t ecx, uint32_t edx, uint32_t s0);   // 0xC7E30 (ret 4)
    void (*a01f0)(uint32_t s0);                            // 0x801F0 (ret 4)
    void (*b6b300)(uint32_t eax, uint32_t unit, uint32_t two);   // 0x6B300: EAX = skill id, 7 stack words (unit, 2, 0 x5)
};

extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_74350(uint32_t p1, int32_t p2, uint32_t p3, uint32_t tbl, const D2c74350Env* e) {
    if (p2 < 0 || p2 >= (int32_t)rd32(tbl + 0xba0)) return 0;
    uint32_t rec = (uint32_t)p2 * 0x23c + rd32(tbl + 0xb98);
    if (!rec) return 0;
    int32_t id = rs16(rec + 0xea);
    if (id < 0 || id >= (int32_t)rd32(tbl + 0xb6c)) return 0;
    uint32_t pl = e->a5f70();
    if (!pl || !p1) return 0;
    uint32_t m[23]; memset(m, 0, sizeof m);
    m[0] = 1; m[1] = p1;
    m[5] = e->c1630(pl);
    m[6] = e->c1660(pl);
    m[4] = (uint32_t)id; m[11] = (uint32_t)p2; m[12] = p3;
    uint32_t o = e->b30f0(m);
    if (o) {
        e->c7e30(0x118, 0, 0);
        uint32_t a = rd32(o + 0x54);
        if (a) {
            uint32_t b = rd32(a + 0x30);
            if (b) { wr32(b + 0x10, 0); wr32(b + 0x14, 0); wr32(b + 0x18, 0xfffec000u); }
        }
    }
    e->a01f0(0);
    int32_t sk = rs16(rec + 0x10c);
    if (sk > 0) e->b6b300((uint32_t)sk, pl, 2);
    return 1;
}
