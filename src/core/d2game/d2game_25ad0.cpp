// D2Game missile side-split: when a flagged missile hits its tracked target, two copies of the missile class are created
// at the source point, offset perpendicular to the source->target direction. Over retail-layout (32-bit) blobs.
// Convention: ECX = game, EDX = missile, two stack words (the first unused, the second must be nonzero); ret 8.
// Callees are Env seams; the unit position getters and the monster-flag test are inlined.
#include <cstdint>
#include <cstring>

static inline uint32_t r32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline uint16_t r16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }

struct D2Game25AD0Env {
    const uint8_t* dt;                                       // sgptDataTables
    const uint8_t* bitMasks;                                 // D2Common gdwBitMasks
    void*    (*ptr)(uint32_t v);
    void*    (*serverUnit)(void* game, uint32_t type, uint32_t id);   // EAX = type, ECX = game, EDX = id
    void*    (*pathTarget)(void* game, void* unit);          // ECX = game, ESI = unit
    int32_t  (*pathX)(uint32_t path);                        // dynamic path x (stdcall 1)
    int32_t  (*pathY)(uint32_t path);
    int32_t  (*missX)(uint32_t path);                        // missile path x (stdcall 1)
    int32_t  (*missY)(uint32_t path);
    void     (*seedInit)(uint32_t* seed);                    // ECX = seed
    void     (*seedLow)(uint32_t* seed, uint32_t v);         // ECX = seed, EDX = value
    int32_t  (*getSkill)(void* missile);
    int32_t  (*getLevel)(void* missile);
    void     (*create)(void* game, uint32_t cls, const uint32_t* args8);   // ECX = game, EDX = class, 8 stack words, ret 0x20
};

static int32_t unitXY(const D2Game25AD0Env* e, void* u, bool y) {
    const uint32_t t = r32(u, 0), path = r32(u, 0x2c);
    if (t == 2 || t == 4 || t == 5) return (int32_t)r32(e->ptr(path), y ? 0x10 : 0xc);
    if (!path) return 0;
    return y ? e->pathY(path) : e->pathX(path);
}

static int32_t sgn(int32_t v) { return v < 0 ? -1 : v > 0 ? 1 : 0; }

// 1.13c D2Game+0x25AD0
void D2Game_25AD0(const D2Game25AD0Env* e, void* game, void* missile, uint32_t, uint32_t enable) {
    if (!enable || !missile || r32(missile, 0) != 3) return;
    const int32_t cls = (int32_t)r32(missile, 4);
    if (!((r32(missile, 0xc8) >> 10) & 1)) return;
    uint8_t* tgt = (uint8_t*)e->serverUnit(game, r32(missile, 0x94), r32(missile, 0x98));
    if (!tgt || cls < 0 || cls >= (int32_t)r32(e->dt, 0xb6c)) return;
    const uint8_t* rec = (const uint8_t*)e->ptr(r32(e->dt, 0xb64) + (uint32_t)cls * 0x1a4);
    if (!rec || (rec[5] & e->bitMasks[0x10])) return;
    if (r32(tgt, 0) == 1 && r32(tgt, 0x14) && (r16(e->ptr(r32(tgt, 0x14)), 0x16) & 0x80)) return;
    const uint32_t mpath = r32(missile, 0x2c);
    int32_t sx, sy;
    if (void* src = e->pathTarget(game, tgt)) { sx = unitXY(e, src, false); sy = unitXY(e, src, true); }
    else { sx = e->missX(mpath); sy = e->missY(mpath); }
    uint32_t seed[2];
    e->seedInit(seed);
    e->seedLow(seed, 0x53454953);
    seed[1] = r32(tgt, 0xc);
    int32_t dx = sgn(unitXY(e, tgt, false) - sx);
    int32_t dy = sgn(unitXY(e, tgt, true) - sy);
    if (cls >= 0x3f && cls <= 0x42) { dx = 0; dy = 0; }
    const bool mon = r32(tgt, 0) == 1 && r32(tgt, 0x14);
    if (mon) ((uint8_t*)e->ptr(r32(tgt, 0x14)))[0x16] |= 0x80;
    {
        const int32_t lvl = e->getLevel(missile), sk = e->getSkill(missile);
        const uint32_t a[8] = { (uint32_t)(uintptr_t)tgt, (uint32_t)sk, (uint32_t)lvl, 0, 0, (uint32_t)(sx - dy), (uint32_t)(sy + dx), 0 };
        e->create(game, (uint32_t)cls, a);
    }
    {
        const int32_t lvl = e->getLevel(missile), sk = e->getSkill(missile);
        const uint32_t a[8] = { (uint32_t)(uintptr_t)tgt, (uint32_t)sk, (uint32_t)lvl, 0, 0, (uint32_t)(sx + dy), (uint32_t)(sy - dx), 0 };
        e->create(game, (uint32_t)cls, a);
    }
    if (r32(tgt, 0) == 1 && r32(tgt, 0x14)) ((uint8_t*)e->ptr(r32(tgt, 0x14)))[0x16] &= 0x7f;
}
