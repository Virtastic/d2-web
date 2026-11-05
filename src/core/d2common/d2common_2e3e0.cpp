// 1.13c D2Common+0x2E3E0 (ordinal 10316, DRLG allocation for one act). Raw offsets on
// 32-bit addresses; every callee is a seam (D2Common2E3E0Env) so the FN probe logs retail's call records.
#include <cstdint>
#include <cstring>

namespace {
inline uint32_t R32(uint32_t a, uint32_t off) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + off), 4); return v; }
inline void W32(uint32_t a, uint32_t off, uint32_t v) { std::memcpy((void*)(uintptr_t)(a + off), &v, 4); }
inline void W8(uint32_t a, uint32_t off, uint8_t v) { *(uint8_t*)(uintptr_t)(a + off) = v; }
constexpr uint32_t kSeedMul = 0x6AC690C5u;
}

struct D2Common2E3E0Env {
    uint32_t (*alloc)(uint32_t pool, uint32_t size, uint32_t line);  // Fog 10045 (thunk 0x9222): ECX pool, EDX size, stack (file, line, 0)
    void (*rooms2D4E0)(uint32_t drlg);                                // 0x2D4E0: EAX drlg
    void (*links7B8D0)(uint32_t drlg);                                // 0x7B8D0: EAX drlg
    void (*act)(uint32_t idx, uint32_t eax);                         // 0x17AE0/0x17AA0/0x17A00/0x17990/0x17930 by act 0..4
    uint32_t (*level2D9B0)(uint32_t drlg, uint32_t id);              // 0x2D9B0: ECX drlg, EDX level id
    void (*init2E360)(uint32_t level);                                // 0x2E360: stack (level), ret 4
};

// ECX act record, DL act number; stack a1 (+0x464), seed, town level id, flags (+0x8C), game (+0x9C), difficulty byte
// (+0x450), automap callback (+0x454), town automap callback (+0x488); ret 0x20. Returns the new 0x48C-byte record.
extern "C" uint32_t D2Common_2E3E0With(uint32_t actRec, uint32_t edx, uint32_t a1, uint32_t seed, uint32_t townId,
    uint32_t flags, uint32_t game, uint32_t diff, uint32_t automap, uint32_t townAutomap, const D2Common2E3E0Env* e) {
    const uint8_t act = (uint8_t)edx;
    const uint32_t pool = R32(actRec, 0x5C);
    const uint32_t d = e->alloc(pool, 0x48C, 0x3A);
    std::memset((void*)(uintptr_t)d, 0, 0x48C);
    W32(d, 0x478, pool);
    W32(d, 0x464, a1);
    W32(d, 0x8C, flags);
    W32(d, 0x9C, game);
    W8(d, 0x450, (uint8_t)diff);
    W32(d, 0x454, automap);
    W8(d, 0x480, act);
    W32(d, 0x46C, actRec);
    uint64_t st = (uint64_t)seed * kSeedMul + 0x29A;   // the low-seed init and its first roll
    W32(d, 0x00, (uint32_t)st); W32(d, 0x04, (uint32_t)(st >> 32));
    W32(d, 0x470, (uint32_t)st);
    W32(d, 0x458, seed);
    W32(d, 0x488, townAutomap);
    if (act == 1) {
        uint32_t a, b;
        do {
            st = (uint64_t)(uint32_t)st * kSeedMul + (st >> 32); a = (uint32_t)st % 7;
            st = (uint64_t)(uint32_t)st * kSeedMul + (st >> 32); b = (uint32_t)st % 7;
        } while (a == b);
        W32(d, 0x94, a + 0x42);
        W32(d, 0x00, (uint32_t)st); W32(d, 0x04, (uint32_t)(st >> 32));
        W32(d, 0x484, b + 0x42);
    } else if (act == 2) {
        st = (uint64_t)(uint32_t)st * kSeedMul + (st >> 32);
        W32(d, 0x00, (uint32_t)st); W32(d, 0x04, (uint32_t)(st >> 32));
        W32(d, 0x474, (uint32_t)st & 1);
    }
    e->rooms2D4E0(d);
    e->links7B8D0(d);
    if (act <= 4) e->act(act, act == 2 ? 2u : d);   // act 2's callee is entered with EAX = 2 (the switch index), not the record
    if (townId) {
        uint32_t lv = R32(d, 0x47C);
        while (lv && R32(lv, 0x1D0) != townId) lv = R32(lv, 0x1AC);
        if (!lv) lv = e->level2D9B0(d, townId);
        e->init2E360(lv);
    }
    return d;
}
