// src/core/d2game/d2game_batch_0cdc0.cpp -- twelve 1.13c D2Game.dll functions transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000.
//
// Same shape as d2game_batch_104f0.cpp: register-only arguments (in_EAX / unaff_E?? / the __fastcall ECX,
// EDX) are explicit parameters, callees that src/ does not implement are seams on D2GameBatch0CDC0Env in
// retail argument order. Callee register arguments the decompile elides are passed as the caller's live
// value and marked "reg". The "out[-1]" convention: 0x9FA30 writes out[0] and the dword below it.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline int16_t  rd16s(const void* p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }

struct D2GameBatch0CDC0Env {
    const void* tables;               // *sgptDataTables_exref: +0xa80 units, +0xb6c, +0xb98 skills, +0xba0, +0xbc0
    const int32_t* tabA3DC;           // 6fd1a3dc
    const uint32_t* tabD0B4;          // 6fd1d0b4[4]
    const uint32_t* tabD0C4;          // 6fd1d0c4[4]
    void* cb61F50;                    // LAB_6fc61f50
    // D2Common ordinals
    int32_t  (*o10511)(void* a);
    void     (*o10980)();
    uint32_t (*o10649)(int32_t a);
    uint32_t (*o10179)(int32_t a);
    uint32_t (*o10631)(int32_t a);
    void     (*o10331)(void* a);
    void     (*o10210)(void* a, uint32_t b);       // 1-arg and 0-arg uses pass nulls/0
    void     (*o10689)(void* a, int32_t b, int32_t c);
    void     (*o10945)();
    int32_t  (*o11027)();
    void     (*o10637)(void* a, int32_t b);
    void     (*o10019)(void* a, int32_t b);
    int32_t  (*o10786)(void* a, int32_t b, int32_t c, int32_t d);
    int32_t  (*o10494)(void* a, int32_t b);
    void     (*o10542)(void* a, int32_t b);
    uint32_t (*o10459)(int32_t a);
    int32_t  (*o11088)();
    int32_t  (*o10910)(void* a, int32_t b, int32_t c);   // reg: a/b/c elided on the bare calls
    int32_t  (*o10973)();
    void     (*o10887)();
    int32_t  (*o10049)();
    int32_t* (*o10535)();
    // D2Game internals
    int32_t  (*sub1280)();
    int32_t  (*sub18A0)(void* reg);
    void     (*sub18E0)();
    int32_t  (*sub1100)(void* a);
    int32_t  (*sub1130)(void* a);
    int32_t  (*sub11D0)(void* a);
    void*    (*subEF650)(uint32_t ebp, void* a2, int32_t unit, int32_t b, uint32_t c, uint32_t d, int32_t mode, int32_t z);
    void     (*subE900)(int32_t a);
    uint8_t* (*subE680)(void* unit);
    void     (*subE6F0)(void* reg);
    void     (*subEA80)(void* game, void* unit, int32_t a, int32_t b, int32_t c, int32_t d, int32_t e);
    void     (*subD230)(void* game, void* unit);
    void     (*subFEEE0)(uint32_t game, void* unit, void* z, int32_t one);
    int32_t  (*sub9FA30)(void* unit, int32_t* out);       // writes out[0] and out[-1]
    void*    (*subC28F0)(void* game, int32_t a, void* unit, int32_t skill, uint32_t d, int, int, int, int, int);
    void     (*sub8F930)(void* unit, const uint32_t* s23);
    void     (*sube490)(int32_t* xy);
    void     (*sube450)();
    int32_t  (*sub41260)(void* unit);
};

static const uint8_t* skillRec(const D2GameBatch0CDC0Env* e, int32_t id) {
    const uint8_t* t = (const uint8_t*)e->tables;
    if (id < 0 || (int32_t)rd32(t, 0xba0) <= id) return nullptr;
    const uint8_t* r = (const uint8_t*)(uintptr_t)rd32(t, 0xb98);
    return r ? r + (size_t)id * 0x23c : nullptr;
}

// 1.13c D2Game+0x0CDC0 -- ECX game, EDX unit, stack skill id; EBP caller's.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_0CDC0(uint32_t p1, void* p2, int32_t skill, uint32_t ebp,
                                                           const D2GameBatch0CDC0Env* e)
{
    const uint8_t* rec = skillRec(e, skill);
    if (!rec) return 0;
    int32_t iVar2 = e->o10511(p2);
    if (iVar2 == 0) return 0;
    e->o10980();
    uint32_t u3 = e->o10649(iVar2), u4 = e->o10179(iVar2), u5 = e->o10631(iVar2);
    e->o10331(p2);
    int32_t iVar6 = e->sub1280();
    if (iVar6 == 0) return 0;
    uint8_t* r7 = (uint8_t*)e->subEF650(ebp, (void*)(uintptr_t)u5, iVar2, iVar6, u3, u4, -1, 0);
    if (!r7 && !(r7 = (uint8_t*)e->subEF650(ebp, (void*)rec, iVar2, iVar6, u3, u4, 4, 0))) return 0;
    wr32(r7, 0xc4, rd32(r7, 0xc4) | 0x4020000);
    e->o10210(r7, u5);
    int16_t s = rd16s(rec, 0xe6);
    if (s >= 0 && s < (int32_t)rd32(e->tables, 0xbc0)) e->o10689(r7, s, 0);
    (void)p1;
    return 1;
}

static bool isQuestObj(const D2GameBatch0CDC0Env* e, void* reg, int32_t* idOut) {
    int32_t n = (int32_t)rd32(e->tables, 0xa80);
    int32_t i = e->sub18A0(reg);
    if (i < 0 || n <= i || i != 0x1ec) return false;
    i = e->sub18A0(reg);
    if (i < 0 || n <= i) i = -1;
    *idOut = i;
    return i == 0x1b3 || i == 0x1b9;
}

// 1.13c D2Game+0x0D040 -- ECX this, stack game, EAX word, ESI unit.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_0D040(void* self, void* game, uint32_t eax, int32_t* esi,
                                                       const D2GameBatch0CDC0Env* e)
{
    int32_t id;
    if (!esi || !self || !isQuestObj(e, esi, &id)) return;
    e->o10945(); e->o10210(nullptr, 0); e->o10210(self, 0);
    e->subE900(5);
    uint8_t* p = e->subE680(esi);
    p[2] |= 4; p[0] = 5;
    wr32(p, 4, 0xffffffff); wr32(p, 0x14, eax); wr32(p, 0x18, 0x8f); wr32(p, 0x2c, 0);
    uint32_t* head = (uint32_t*)((uint8_t*)(uintptr_t)rd32(game, 0xb8) + 0xa04 + (size_t)e->tabA3DC[esi[0]] * 4);
    uint32_t old = *head;
    wr32(p, 0x1c, old); wr32(p, 0x20, 0);
    if (old) wr32((void*)(uintptr_t)old, 0x20, (uint32_t)(uintptr_t)p);
    *head = (uint32_t)(uintptr_t)p;
    esi[0x31] &= ~0xe;
    e->subD230(game, esi);
}

// 1.13c D2Game+0x0D290 -- ECX game, EDX unit, stack skill, stack arg4.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_0D290(void* game, int32_t* unit, int32_t skill, uint32_t a4,
                                                           const D2GameBatch0CDC0Env* e)
{
    const uint8_t* rec = skillRec(e, skill);
    if (!rec) return 0;
    int32_t k = rd16s(rec, 0x48);
    if (!(k > 0 && k < (int32_t)rd32(e->tables, 0xb6c))) return 0;
    if (unit) unit[0x31] |= 0x40;
    int32_t xy[2];
    if (!e->sub9FA30(game, &xy[1])) return 0;
    int32_t x = e->sub1100(unit), y = e->sub1130(unit);
    int32_t* m = (int32_t*)e->subC28F0(game, k, unit, skill, a4, 0, 0, 0, 0, 0);
    if (!m) return 0;
    e->o10637(m, -(xy[1] - y));
    e->o10019(m, xy[0] - x);
    return 1;
}

// 1.13c D2Game+0x0D370 -- ECX game, EDX unit, stack skill, stack arg4. ponytail: Ghidra merged stack slots
// here; the +0x138/+0x13c reads use the skill record and the second 10786 skill arg is param_3.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_0D370(void* game, int32_t* unit, int32_t skill, int32_t a4,
                                                           const D2GameBatch0CDC0Env* e)
{
    const uint8_t* rec = skillRec(e, skill);
    if (!rec) return 0;
    int32_t k = rd16s(rec, 0x48);
    if (!(k > 0 && k < (int32_t)rd32(e->tables, 0xb6c))) return 0;
    int32_t xy[2];
    if (e->sub9FA30(game, &xy[1])) {
        int32_t dx = xy[0] - e->sub1100(unit), dy = xy[1] - e->sub1130(unit);
        int32_t v[2] = { dx, dy };
        e->sube490(v);
        e->sube450();
        int32_t lo = (int32_t)rd32(rec, 0x138);
        uint32_t cur = (uint32_t)(uintptr_t)unit;
        int32_t n = e->o10786(unit, lo, skill, a4);
        uint32_t s[23] = {};
        s[0] = 0x20; s[1] = s[2] = (uint32_t)(uintptr_t)unit; s[0xc] = a4; s[4] = xy[1];
        s[0xf] = (uint32_t)e->o10786(unit, (int32_t)rd32(rec, 0x13c), skill, a4);
        for (; n > 0; --n) {
            s[3] = cur; s[4] = (uint32_t)lo;
            e->sub8F930(game, s);
            cur += (uint32_t)skill; lo += a4;
        }
        if (e->o10494(unit, 0xc)) { e->o10542(unit, 8); unit[0x12] = 0x600; }
    }
    return 1;
}

// 1.13c D2Game+0x0D5D0 -- ECX, EDX word, stack unit, arg4, count, arg6, arg7.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_0D5D0(uint32_t p1, uint32_t p2, void* unit, uint32_t p4,
                                                           int32_t count, uint32_t p6, uint32_t p7,
                                                           const D2GameBatch0CDC0Env* e)
{
    uint32_t s[23] = {};
    s[0] = 3; s[1] = p2; s[4] = p4; s[0xb] = p6; s[0xc] = p7;
    s[0x15] = (uint32_t)(uintptr_t)e->cb61F50;
    int32_t xy[2];
    if (!e->sub9FA30(unit, &xy[1])) return 0;
    s[5] = xy[0]; s[6] = xy[1];
    for (int32_t i = 0; i < count; ++i) {
        s[7] = e->tabD0B4[i & 3]; s[8] = e->tabD0C4[i & 3]; s[0x16] = i;
        e->sub8F930(unit, s);
    }
    (void)p1;
    return 1;
}

// 1.13c D2Game+0x0DB00 -- stack game, EAX word, ESI unit.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_0DB00(uint32_t game, void* eax, int32_t* esi,
                                                       const D2GameBatch0CDC0Env* e)
{
    int32_t id;
    if (!esi || !eax || !isQuestObj(e, esi, &id)) return;
    e->o10945(); e->o10210(nullptr, 0); e->o10210(nullptr, 0);
    if (e->o11027() < 10) { e->subFEEE0(game, esi, nullptr, 1); return; }
    esi[0x31] |= 0xe;
    e->subE900(5);
    e->sub18E0();
    e->subD230((void*)(uintptr_t)game, esi);
}

// 1.13c D2Game+0x0DE20 -- EAX game, EBX unit.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_0DE20(void* eax, int32_t* ebx, const D2GameBatch0CDC0Env* e)
{
    if (ebx) {
        char* n = (char*)(uintptr_t)(uint32_t)ebx[0x37];
        while (n) {
            char c = *n;
            n = (char*)(uintptr_t)rd32(n, 0x24);
            if (c == 0) e->subE6F0(n);
        }
    }
    e->subEA80(eax, ebx, 2, (int32_t)rd32(eax, 0xa8) + 0xd, 0, 0, 0);
}

// 1.13c D2Game+0x0F180 -- EAX level, stack arg, EBX class, EDI record, retaddr seed ptr.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_0F180(int32_t level, int32_t flag, int32_t ebx,
                                                           const uint8_t* rec, void* seed,
                                                           const D2GameBatch0CDC0Env* e)
{
    if (level < 1) level = 1;
    if (rec[0x133] && !rec[0x12a] && (int32_t)rec[0xfd] <= level) {
        if (flag == 0) {
            uint32_t v = e->o10459(level) & 0xff;
            if (rec[0xfc] != v && (int32_t)(rec[0xfc] - v) >= 0 && e->sub11D0(seed) != 0) return 0;
        }
        if (ebx != -1 && rd16s(rec, 0x11e) != ebx) return 0;
        return 1;
    }
    return 0;
}

// 1.13c D2Game+0x0F260 -- EBX unit, EDI pointer to int.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_0F260(int32_t* ebx, int32_t* edi, const D2GameBatch0CDC0Env* e)
{
    if (!ebx || !edi) return;
    if (e->o11088() != 4) return;
    int32_t v = e->o10910(ebx, 0, 0) + 100;
    int32_t w = e->sub41260(ebx);
    if (w) v += e->o10910((void*)(intptr_t)w, 0x4f, 0);
    v = (e->o10973() * v) / 100;
    if (v < 0 || (*edi == 0 && e->o10049() < v)) e->o10887();
    else e->o10887();
}

// 1.13c D2Game+0x0F2F0 -- ESI unit.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_0F2F0(int32_t* esi, const D2GameBatch0CDC0Env* e)
{
    if (!esi || *esi < 0 || *esi >= 2) return 0;
    int32_t v = e->o10910(esi, 0, 0);
    int32_t w = e->sub41260(esi);
    if (w) v += e->o10910((void*)(intptr_t)w, 0x50, 0);
    return v;
}

// Shared body of 0xF8F0 (range +8/+0x10) and 0xFA30 (range +0x10/+0x18): collect up to 0x3ff candidates
// (0x1a8-byte records) accepted by 0xF180, pick one with the LCG 0x6AC690C5.
static int32_t pickRec(int32_t base, uint32_t lo, uint32_t hi, int32_t* ctx, uint64_t* seed, int32_t level,
                       int32_t flag, int32_t ebx, const D2GameBatch0CDC0Env* e)
{
    uint8_t* tbl = (uint8_t*)e->o10535();
    uint32_t rec = rd32(tbl, lo);
    int32_t off = (int32_t)(rec - rd32(tbl, 4));
    int32_t cnt = ((int32_t)rd32(tbl, hi) - (int32_t)rec) / 0x1a8;
    if (rec == 0) return -1;
    int32_t idx[1024], ptr[1024]; uint32_t n = 0;
    for (int32_t i = 0; i < cnt; ++i, rec += 0x1a8) {
        if (d2_fn_d2game_0F180(level, flag, ebx, (const uint8_t*)(uintptr_t)rec, seed, e) &&
            (rd32((void*)(intptr_t)base, 0x70) != 0 || (uint16_t)(rd16s((void*)(uintptr_t)rec, 0xf6)) < 100) &&
            (int32_t)n < 0x3ff) {
            idx[n] = i + off / 0x1a8; ptr[n] = (int32_t)rec; ++n;
        }
    }
    uint32_t pick = 0;
    if ((int32_t)n > 0) {
        uint32_t lo32 = (uint32_t)*seed, hi32 = (uint32_t)(*seed >> 32);
        *seed = (uint64_t)lo32 * 0x6ac690c5u + hi32;
        pick = (n & (n - 1)) == 0 ? ((n - 1) & (uint32_t)*seed) : (uint32_t)((*seed & 0xffffffffu) % n);
    }
    (void)ctx;
    return ((int32_t)pick <= (int32_t)n) ? idx[pick] : -1;
}

// 1.13c D2Game+0x0F8F0 -- ECX game(param_1 is stack), seed ptr, level, flag.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_0F8F0(int32_t param_1, uint64_t* seed, int32_t level,
                                                          int32_t flag, int32_t ebx, const D2GameBatch0CDC0Env* e)
{ return pickRec(param_1, 8, 0x10, nullptr, seed, level, flag, ebx, e); }

// 1.13c D2Game+0x0FA30
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_0FA30(int32_t param_1, uint64_t* seed, int32_t level,
                                                          int32_t flag, int32_t ebx, const D2GameBatch0CDC0Env* e)
{ return pickRec(param_1, 0x10, 0x18, nullptr, seed, level, flag, ebx, e); }
