// D2Game missile hit handler 20 (Lightning Fury), over retail-layout (32-bit) blobs.
// Convention: ECX = game, EDX = missile, one stack word (the hit unit, unused); ret 4. Returns 1, or 3 when the aura
// sweep ran. Callees are Env seams; the unit position getters are inlined with their dynamic-path calls as seams.
#include <cstdint>
#include <cstring>

static inline uint32_t r32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline int16_t r16(const void* p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }

struct D2Game3DD60Env {
    const uint8_t* dt;                                    // sgptDataTables
    void*    (*ptr)(uint32_t v);                          // a 32-bit struct field -> host pointer
    int32_t  (*getSkill)(void* missile);                  // D2Common missile skill (stdcall 1)
    int32_t  (*getLevel)(void* missile);                  // D2Common missile level (stdcall 1)
    void*    (*getOwner)(void* game, void* missile);      // EAX = missile, stack game, ret 4
    int32_t  (*pathX)(uint32_t path);                     // dynamic path x (D2Common, stdcall 1)
    int32_t  (*pathY)(uint32_t path);                     // dynamic path y (D2Common, stdcall 1)
    int32_t  (*evalFormula)(void* owner, uint32_t calc, int32_t skill, int32_t level);   // D2Common 10786
    void     (*auraSweep)(void* game, uint32_t* args8);   // EAX = game, 8 stack words, ret 0x20
    uint32_t callback;                                    // the per-target callback's address
};

// unit x / y: unit types 2, 4 and 5 read the static path (+0xC / +0x10), the rest ask the dynamic path (0 when null)
static int32_t unitXY(const D2Game3DD60Env* e, void* u, bool y) {
    const uint32_t t = r32(u, 0), path = r32(u, 0x2c);
    if (t == 2 || t == 4 || t == 5) return (int32_t)r32(e->ptr(path), y ? 0x10 : 0xc);
    if (!path) return 0;
    return y ? e->pathY(path) : e->pathX(path);
}

// 1.13c D2Game+0x3DD60
uint32_t D2Game_3DD60(const D2Game3DD60Env* e, void* game, void* missile) {
    if (!missile) return 1;
    const int32_t cls = (int32_t)r32(missile, 4);
    if (cls < 0 || cls >= (int32_t)r32(e->dt, 0xb6c)) return 1;
    const uint8_t* rec = (const uint8_t*)e->ptr(r32(e->dt, 0xb64) + (uint32_t)cls * 0x1a4);
    if (!rec || r16(rec, 0x24) < 0) return 1;
    const int32_t skill = e->getSkill(missile);
    const int32_t level = e->getLevel(missile);
    if (skill < 0 || skill >= (int32_t)r32(e->dt, 0xba0)) return 1;
    const uint8_t* sk = (const uint8_t*)e->ptr(r32(e->dt, 0xb98) + (uint32_t)skill * 0x23c);
    if (!sk) return 1;
    void* owner = e->getOwner(game, missile);
    if (!owner) return 1;
    const int32_t x = unitXY(e, missile, false);
    const int32_t y = unitXY(e, missile, true);
    int32_t range = (int32_t)r32(rec, 0x4c);
    if (range <= 0) { range = e->evalFormula(owner, r32(sk, 0x64), skill, level); if (range < 1) range = 1; }
    int32_t count = (int32_t)r32(rec, 0x50);
    if (count <= 0) { count = e->evalFormula(owner, r32(sk, 0x138), skill, level); if (count < 1) count = 1; }
    uint32_t filter = r32(sk, 0x50);
    if (!filter) filter = 0xa783;
    uint32_t blk[6] = { (uint32_t)(uintptr_t)owner, (uint32_t)(uintptr_t)missile, (uint32_t)skill, (uint32_t)level,
                        (uint32_t)count, (uint32_t)(int32_t)r16(rec, 0x24) };
    uint32_t a[8] = { (uint32_t)(uintptr_t)owner, (uint32_t)x, (uint32_t)y, (uint32_t)range, filter, e->callback,
                      (uint32_t)(uintptr_t)blk, 1 };
    e->auraSweep(game, a);
    return 3;
}
