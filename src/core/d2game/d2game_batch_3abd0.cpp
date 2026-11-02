// src/core/d2game/d2game_batch_3abd0.cpp -- 1.13c D2Game.dll missile-hit / missile-do callbacks
// 0x3ABD0..0x3C260, written from the retail decompile (register arguments read off ours' 1.10f
// crosswalk, D2MOO MissMode.cpp). Retail records are opaque host objects read at retail byte offsets;
// every callee src/ does not own is a seam in retail argument order. The default env is inert.
#include <cstdint>
#include <cstring>

static inline uint16_t b3_rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t b3_rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void* b3_ptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }

struct D2GameBatch3Env {
    const void* (*dataTables)();                                         // *sgptDataTables
    void* (*getUnit)(void* pGame, uint32_t guid, uint32_t type);         // 0xE03A0
    int32_t (*ord10240)(void* unit);                                     // skill id
    int32_t (*ord11029)(void* unit);                                     // skill level
    void* (*ord10331)(void* unit);                                       // room
    int32_t (*ord10750)(void* p);
    int32_t (*ord10867)(void* p);
    int32_t (*chk0370)(void* owner);                                     // 0xA0370
    void (*ord10786)(void* owner, uint32_t rec60, int32_t skill, int32_t level);
    void* (*get00B0)();                                                  // 0xA00B0
    void (*fn448E0)(int32_t zero, void* p, int32_t skill, int32_t level);// 0x448E0
    uint32_t (*unitX)(void* unit);                                       // 0x1100
    uint32_t (*unitY)(void* unit);                                       // 0x1130
    uint32_t (*roll)(void* pSeed);                                       // 0x1180 (range register unresolved)
    int32_t (*ord10983)(void* p);
    int32_t (*ord10764)(void* p);
    void (*ord10614)(void* owner);
    void (*createMissile)(void* pGame, uint32_t* params23);              // 0x6F930, D2MissileStrc-shaped
    void (*spawnRandomMonster)(void* pGame, void* room, int32_t x, int32_t y); // 0xEF6B0
};
static const D2GameBatch3Env g_batch3Env{};

static inline bool b3_flag10(const void* u) { return (b3_rd32(u, 0xC8) >> 10) & 1; }
static inline void* b3_owner(const D2GameBatch3Env* e, void* pGame, const void* u)
{
    return e->getUnit(pGame, b3_rd32(u, 0x98), b3_rd32(u, 0x94));
}

// 1.13c D2Game+0x3ABD0
// (ecx=pGame, edx=missile, stack target) -> 1 unless the owner skill helper ran (0)
int32_t D2Game_3ABD0(void* pGame, void* missile, void* target, const D2GameBatch3Env* e = &g_batch3Env)
{
    void* owner;
    if (!missile || !b3_flag10(missile) || !(owner = b3_owner(e, pGame, missile)) || !target) return 1;
    const int32_t id = e->ord10240(missile);
    const int32_t lvl = e->ord11029(missile);
    const void* dt = e->dataTables();
    if (id < 0 || id >= (int32_t)b3_rd32(dt, 0xBA0)) return 1;
    const uint8_t* rec = (const uint8_t*)b3_ptr(dt, 0xB98) + (uint32_t)id * 0x23C;
    const int16_t st = (int16_t)b3_rd16(rec, 0x82);
    if (st < 0 || st > (int32_t)b3_rd32(dt, 0xC4)) return 1;        // note: <= count (1.10f used <)
    if (!e->chk0370(owner)) return 1;
    e->ord10786(owner, b3_rd32(rec, 0x60), id, lvl);
    if (void* x = e->get00B0()) e->fn448E0(0, x, id, lvl);
    return 0;
}

static inline bool b3_petlike(uint32_t t) { return t == 2 || (t > 3 && t < 6); }
// The (y, x) pair read off the missile's path/pet record (+0x2C): 0x10 / 0xC for pet-like types, else #10750 / #10867.
static void b3_pos(const D2GameBatch3Env* e, const void* u, int32_t* y, int32_t* x)
{
    void* sub = b3_ptr(u, 0x2C);
    if (b3_petlike(b3_rd32(u, 0))) { *y = (int32_t)b3_rd32(sub, 0x10); *x = (int32_t)b3_rd32(sub, 0xC); return; }
    *y = sub ? e->ord10750(sub) : 0;
    *x = sub ? e->ord10867(sub) : 0;
}

// 1.13c D2Game+0x3BC00
// (ecx=pGame, edx=missile) -> 1: spawn a random monster for the level at the missile's position
int32_t D2Game_3BC00(void* pGame, void* missile, const D2GameBatch3Env* e = &g_batch3Env)
{
    int32_t y, x;
    b3_pos(e, missile, &y, &x);
    e->spawnRandomMonster(pGame, e->ord10331(missile), x, y);
    return 1;
}

// D2MissileStrc dword indices (Units/Missile.h)
enum { MS_FLAGS = 0, MS_OWNER = 1, MS_ORIGIN = 2, MS_MISSILE = 4, MS_X = 5, MS_Y = 6, MS_SKILL = 11, MS_LEVEL = 12 };

// 1.13c D2Game+0x3BED0
// (ecx=pGame, edx=missile) -> 1: create the record's +0x24 sub-missile at a random offset around the missile
int32_t D2Game_3BED0(void* pGame, void* missile, const D2GameBatch3Env* e = &g_batch3Env)
{
    if (!missile) return 1;
    const void* dt = e->dataTables();
    const int32_t cls = (int32_t)b3_rd32(missile, 4);
    if (cls < 0 || cls >= (int32_t)b3_rd32(dt, 0xB6C)) return 1;
    const uint8_t* rec = (const uint8_t*)b3_ptr(dt, 0xB64) + (uint32_t)cls * 0x1A4;
    if ((int16_t)b3_rd16(rec, 0x24) < 0 || !b3_flag10(missile)) return 1;
    void* owner = b3_owner(e, pGame, missile);
    if (!owner) return 1;
    e->unitX(missile);
    e->ord10614(owner);
    int32_t r = 1;
    if ((int32_t)b3_rd32(rec, 0x4C) > 0) r = (int32_t)b3_rd32(rec, 0x4C);
    const uint32_t x = e->unitX(missile), y = e->unitY(missile);
    void* seed = (uint8_t*)missile + 0x20;
    const uint32_t r1 = e->roll(seed), r2 = e->roll(seed);
    uint32_t p[23] = {};
    p[MS_FLAGS] = 0x20;
    p[MS_OWNER] = (uint32_t)(uintptr_t)owner;
    p[MS_MISSILE] = (uint16_t)b3_rd16(rec, 0x24);
    p[MS_SKILL] = (uint32_t)e->ord10240(missile);
    p[MS_LEVEL] = (uint32_t)e->ord11029(missile);
    p[MS_X] = r1 + (x - (uint32_t)r);
    p[MS_Y] = r2 + (y - (uint32_t)r);
    e->createMissile(pGame, p);
    return 1;
}

// 1.13c D2Game+0x3C160
// (ecx=pGame, edx=missile) -> 1: create the three hit sub-missiles (row words +0x24, +0x26, +0x28)
int32_t D2Game_3C160(void* pGame, void* missile, const D2GameBatch3Env* e = &g_batch3Env)
{
    if (!missile) return 1;
    const void* dt = e->dataTables();
    const int32_t cls = (int32_t)b3_rd32(missile, 4);
    if (cls < 0 || cls >= (int32_t)b3_rd32(dt, 0xB6C)) return 1;
    const uint8_t* rec = (const uint8_t*)b3_ptr(dt, 0xB64) + (uint32_t)cls * 0x1A4;
    if ((int16_t)b3_rd16(rec, 0x24) < 0) return 1;
    uint32_t p[23] = {};
    p[MS_ORIGIN] = (uint32_t)(uintptr_t)missile;
    p[MS_OWNER] = b3_flag10(missile) ? (uint32_t)(uintptr_t)b3_owner(e, pGame, missile) : 0;
    p[MS_SKILL] = (uint32_t)e->ord10240(missile);
    p[MS_LEVEL] = (uint32_t)e->ord11029(missile);
    for (uint32_t o = 0x24; o <= 0x28; o += 2) {
        const int16_t m = (int16_t)b3_rd16(rec, o);
        if (o != 0x24 && m < 0) continue;
        p[MS_MISSILE] = (uint16_t)m;
        e->createMissile(pGame, p);
    }
    return 1;
}

// 1.13c D2Game+0x3C260
// (ecx=pGame, edx=missile) -> 1: create flag-0x21 sub-missile (row word +0x24) at the missile's position,
// targeting the path record's #10983 / #10764 pair
int32_t D2Game_3C260(void* pGame, void* missile, const D2GameBatch3Env* e = &g_batch3Env)
{
    if (!missile) return 1;
    const void* dt = e->dataTables();
    const int32_t cls = (int32_t)b3_rd32(missile, 4);
    if (cls < 0 || cls >= (int32_t)b3_rd32(dt, 0xB6C)) return 1;
    const uint8_t* rec = (const uint8_t*)b3_ptr(dt, 0xB64) + (uint32_t)cls * 0x1A4;
    if ((int16_t)b3_rd16(rec, 0x24) < 0) return 1;
    int32_t y, x;
    b3_pos(e, missile, &y, &x);
    void* sub = b3_ptr(missile, 0x2C);
    uint32_t p[23] = {};
    p[MS_FLAGS] = 0x21;
    p[MS_OWNER] = b3_flag10(missile) ? (uint32_t)(uintptr_t)b3_owner(e, pGame, missile) : 0;
    p[MS_X] = (uint32_t)x;
    p[MS_Y] = (uint32_t)y;
    p[MS_SKILL] = (uint32_t)e->ord10240(missile);
    p[MS_LEVEL] = (uint32_t)e->ord11029(missile);
    p[MS_MISSILE] = (uint16_t)b3_rd16(rec, 0x24);
    p[7] = (uint32_t)e->ord10983(sub);   // nTargetX
    p[8] = (uint32_t)e->ord10764(sub);   // nTargetY
    e->createMissile(pGame, p);
    return 1;
}

// function probe seam (src/core/fn/d2game_3bc00.cpp): cfg dwords: room (A478), y (2A3B8), x (2A454).
#include "../fn/hookenv.h"
static const uint32_t* g_p3e_cfg; static uint32_t* g_p3e_lg;
static void p3e_log(uint32_t rva, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3)
{
    uint32_t* w = g_p3e_lg + 1 + g_p3e_lg[0]++ * 10;
    w[0] = rva; w[1] = 0; w[2] = 0; w[3] = 0; w[4] = 0; w[5] = 0; w[6] = s0; w[7] = s1; w[8] = s2; w[9] = s3;
    for (int i = 1; i < 10; i++) if (HookEnv::isStk(w[i])) w[i] = 0xB3000000u;
}
static inline uint32_t p3e32(const void* p) { return (uint32_t)(uintptr_t)p; }
int32_t D2Game_3BC00_probe(void* pGame, void* missile, uint32_t* lg, const uint32_t* cfg)
{
    g_p3e_cfg = cfg; g_p3e_lg = lg; lg[0] = 0;
    D2GameBatch3Env e{};
    e.ord10750 = [](void* p) -> int32_t { p3e_log(0xA3B8, p3e32(p), 0, 0, 0); return (int32_t)g_p3e_cfg[1]; };
    e.ord10867 = [](void* p) -> int32_t { p3e_log(0xA454, p3e32(p), 0, 0, 0); return (int32_t)g_p3e_cfg[2]; };
    e.ord10331 = [](void* u) -> void* { p3e_log(0xA478, p3e32(u), 0, 0, 0); return (void*)(uintptr_t)g_p3e_cfg[0]; };
    e.spawnRandomMonster = [](void* g, void* room, int32_t x, int32_t y) { p3e_log(0xEF6B0, p3e32(g), p3e32(room), (uint32_t)x, (uint32_t)y); };
    return D2Game_3BC00(pGame, missile, &e);
}

// function probe seam (src/core/fn/d2game_3c160.cpp): callees recorded like the capture's `hook` lines; sc = (rva, value) pairs.
#include "../fn/d2game_seam_probe.h"
static D2ProbeSeam* g_p3c; static const void* g_p3cDt;
static uint32_t p3c_u32(const void* p) { return (uint32_t)(uintptr_t)p; }
uint32_t D2Game_3C160_probe(void* pGame, void* missile, const uint32_t* r, uint32_t* lg, const uint32_t* sc)
{
    static const uint32_t size[7] = { 2048, 4096, 4, 4, 0x120, 1024, 64 };
    static const uint32_t rv[3] = { 0xE03A0, 0xB26A, 0xB0FC };
    uint32_t v[3] = {};
    for (int i = 0; i < 3; i++) for (const uint32_t* p = sc; p[0]; p += 2) if (p[0] == rv[i]) { v[i] = p[1]; break; }
    const D2ProbeHook hooks[4] = { { 0xE03A0, "acd", 0, v[0] }, { 0xB26A, "", 1, v[1] }, { 0xB0FC, "", 1, v[2] }, { 0x6F930, "cd", 0, 0 } };
    D2ProbeSeam ps{ r, size, 7, lg, hooks, 4 };
    lg[0] = 0; g_p3c = &ps; g_p3cDt = (const void*)(uintptr_t)r[3];
    D2GameBatch3Env e{};
    e.dataTables = []() -> const void* { return g_p3cDt; };
    e.getUnit = [](void* g, uint32_t guid, uint32_t type) -> void* { return (void*)(uintptr_t)D2ProbeSeam::thunk(g_p3c, 0xE03A0, type, p3c_u32(g), guid, 0, 0, 0, 0, 0, 0); };
    e.ord10240 = [](void* u) -> int32_t { return (int32_t)D2ProbeSeam::thunk(g_p3c, 0xB26A, 0, 0, 0, 0, 0, p3c_u32(u), 0, 0, 0); };
    e.ord11029 = [](void* u) -> int32_t { return (int32_t)D2ProbeSeam::thunk(g_p3c, 0xB0FC, 0, 0, 0, 0, 0, p3c_u32(u), 0, 0, 0); };
    e.createMissile = [](void* g, uint32_t* p) {
        D2ProbeSeam::thunk(g_p3c, 0x6F930, 0, p3c_u32(g), p3c_u32(p), 0, 0, 0, 0, 0, 0);
        for (uint32_t o = 0; o < 23; o += 9) {
            uint32_t* w = g_p3c->lg + 1 + g_p3c->lg[0]++ * 10;
            w[0] = 0x6F930u | 0x40000000u;
            for (uint32_t j = 0; j < 9; j++) w[1 + j] = o + j < 23 ? g_p3c->tag(p[o + j]) : 0;
        } };
    return (uint32_t)D2Game_3C160(pGame, missile, &e);
}
