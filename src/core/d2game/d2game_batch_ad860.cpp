// src/core/d2game/d2game_batch_ad860.cpp -- twelve 1.13c D2Game.dll functions transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000.
//
// Unmapped callees and D2Common ordinals are seams on D2GameBatchAD860Env in retail argument order.
// `ord(n, ...)` is D2Common ordinal n with up to five dword args. Ghidra's unaff_* / in_* registers are
// explicit trailing parameters named at each function. `dt` is *sgptDataTables (table base).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline int16_t  rd16(const void* p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }

struct D2GameBatchAD860Env {
    const void* dt;                                                         // *sgptDataTables
    uint32_t (*ord)(uint32_t n, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e);
    void     (*subDE0C0)(void* self, void* unit, uint32_t one, void* buf);  // 0xDE0C0
    void     (*subDCF20)(void* self, void* unit, void* buf);                // 0xDCF20
    uint32_t (*subE03A0)(uint32_t a, uint32_t b);                           // 0xE03A0
    uint32_t (*subC9E0)();                                                  // 0xAC9E0
    void     (*subF6BC0)();                                                 // 0xD6BC0
    void     (*subF1730)(int32_t a, uint32_t b, void* c);                   // 0xF1730
    void     (*subF17E0)(void* unit);                                       // 0xF17E0
    uint32_t (*sub211D0)(const void* p);                                    // D2Game+0x11D0 (64-bit at p)
    uint32_t (*sub21420)();                                                 // 0x1420
    uint32_t (*sub210D0)();                                                 // 0x10D0
    void     (*subBEA30)(int32_t a, uint32_t b);                            // 0x9EA30
    void     (*subBF7A0)();                                                 // 0x9F7A0 (callback address)
    void     (*subAEA80)(uint32_t a, void* b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g); // 0x8EA80
    uint32_t (*subCE4D0)(uint32_t a, void* b, void* c);                     // 0xAE4D0
    uint32_t (*subB0A20)(void* unit);                                       // 0xB0A20
    uint32_t (*sub41260)(void* unit);                                       // 0x21260
    uint32_t (*subC21320)();                                                // 0x1320
    uint32_t (*sub07A0)(uint32_t a);                                        // 0xE07A0
    void     (*subDDDE0)(uint32_t a, void* b, void* c);                     // 0xDDDE0
    uint32_t (*callPtr)(uint32_t fn, uint32_t a, uint32_t b);               // indirect call through a retail fn pointer
};
#define O(n, a, b, c, d, e) env->ord(n, (uintptr_t)(a), (uintptr_t)(b), (uintptr_t)(c), (uintptr_t)(d), (uintptr_t)(e))

// AD860 / AD910 share one body. ECX=self, p4 packs two words (Ordinal_10910(unit, p4>>16, p4)).
static uint32_t useSkillItem(void* self, void* unit, uint32_t p2, uint32_t p4, const D2GameBatchAD860Env* env)
{
    if (unit && p2 && (rd32((void*)(uintptr_t)p2, 0xc4) >> 2 & 1) && (int32_t)O(10910, unit, p4 >> 16, p4, 0, 0) > 0) {
        uint32_t buf[0x1c] = {0};
        env->subDE0C0(self, unit, 1, buf);
        env->subDCF20(self, unit, buf);
        return 1;
    }
    return 0;
}
// 1.13c D2Game+0xAD860
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_AD860(void* self, void* unit, uint32_t p2, uint32_t p3, uint32_t p4, const D2GameBatchAD860Env* env)
{ (void)p3; return useSkillItem(self, unit, p2, p4, env); }
// 1.13c D2Game+0xAD910
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_AD910(void* self, void* unit, uint32_t p2, uint32_t p3, uint32_t p4, const D2GameBatchAD860Env* env)
{ (void)p3; return useSkillItem(self, unit, p2, p4, env); }

// 1.13c D2Game+0xADAC0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_ADAC0(uint32_t p1, void* p2, uint32_t p3, uint32_t p4, const D2GameBatchAD860Env* env)
{
    (void)p3;
    if (((const char*)p2)[0x6c] != 0 && rd32(p2, 0x68) == 0 && env->subE03A0(p1, rd32(p2, 100)) != 0) return 0;
    if (O(10202, p4, 0x10, 0x94, "..\\Source\\D2Game\\SKILLS\\SkillItem.cpp", 0) == 0 && env->subC9E0() != 0) {
        env->subF6BC0();
        return 1;
    }
    return 0;
}

// 1.13c D2Game+0xADCE0   ebx / retaddr are Ghidra's unaff_EBX and unaff_retaddr (stack word, a record ptr).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_ADCE0(void* p1, uint32_t p2, uint32_t p3, uint32_t p4,
                                                             int32_t ebx, const void* retaddr, const D2GameBatchAD860Env* env)
{
    (void)p2; (void)p3;
    if (!p1) return 0;
    int32_t n = (int32_t)O(10910, p1, p4 >> 16, 0, 0, 0);
    if (n < 1) return 0;
    uint32_t sh = rd32(env->dt, 0xc6c), mask = rd32(env->dt, 0xc70);
    if (retaddr && (((const uint8_t*)retaddr)[4] & 4)) {
        if ((int32_t)env->sub211D0((uint8_t*)p1 + 0x20) < n && env->sub21420() != 0) {
            if (ebx == 0) {
                O(10764, rd32(p1, 0x2c), 0, 0, 0, 0);
                uint32_t r = O(10983, rd32((void*)(uintptr_t)p4, 0x2c), 0, 0, 0, 0);
                env->subF1730((int32_t)(p4 & 0xffff) >> (sh & 0x1f), mask & p4 & 0xffff, (void*)(uintptr_t)r);
                return 1;
            }
            env->subF17E0(p1);
            return 1;
        }
    }
    return 0;
}

// 1.13c D2Game+0xAE4D0   edi is Ghidra's unaff_EDI (the record whose +0x9e..+0xa4 arrays are walked),
// ebx is unaff_EBX (the Ordinal_10428 stack word).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_AE4D0(void* p1, int32_t* unit, void* p3, void* edi, uint32_t ebx, const D2GameBatchAD860Env* env)
{
    int32_t iVar1 = p3 ? (int32_t)rd32(p3, 4) : -1;
    uint32_t rec = O(10695, 0, 0, 0, 0, 0);
    if (!rec) return 0;
    const void* r = (const void*)(uintptr_t)rec;
    int32_t skill = rd16(r, 0x98);
    if (!(0 < skill && skill < (int32_t)rd32(env->dt, 0xc4))) return 0;
    uint32_t owner = rd32(r, 0xb0);
    if ((int32_t)O(10428, unit, p3, 0, 0, 0) <= 0) return 0;
    env->subBEA30(skill, 0);
    int32_t base = 0;
    uint32_t sk = O(10871, unit, skill, 0, 0, 0);
    if (sk) base = (int32_t)O(10959, sk, 0, 0, 0, 0) - (int32_t)rd32(p1, 0xa8);
    uint32_t amt = rd32(p1, 0xa8) + base + iVar1;
    if (!sk) {
        int32_t a, b;
        if (!unit) { b = -1; a = 6; } else { b = unit[3]; a = unit[0]; }
        sk = O(11013, rd32((void*)(uintptr_t)owner, 0x1c), 2, 0, a, b);
        if (!sk) return 0;
        O(10785, sk, unit, 0, 0, 0);
        O(10945, unit, unit, 1, 0, 0);
        O(10911, sk, (uintptr_t)env->subBF7A0, 0, 0, 0);
        O(10807, unit, sk, 1, 0, 0);
    }
    if (amt) {
        O(11043, sk, amt, 0, 0, 0);
        env->subAEA80(owner, unit, 0xc, amt, 0, 0, 0);
    }
    for (int i = 0; i < 3; i++) {
        int32_t id = rd16(edi, 0x9e + 2 * i);
        if (-1 < id && id < (int32_t)rd32(env->dt, 0xbd4) && env->sub210D0() != 0) {
            int32_t v = (int32_t)O(10428, unit, ebx, rd32(edi, 0xa4 + 4 * i), 0, 0);
            if (v != 0) {
                O(10188, sk, id, v, 0, 0);
                if (id == 0x1c && 0 < v) {
                    int32_t hi = (int32_t)O(10973, unit, 0xb, 0, 0, 0);
                    int32_t lo = (int32_t)O(10973, unit, 10, 0, 0, 0);
                    if (lo < hi) O(10887, unit, 10, hi + 0x100, 0, 0);
                }
            }
        }
    }
    return 1;
}

// 1.13c D2Game+0xAF370   p1 is the ECX pass-through for 0xAE4D0 (Ghidra's iStack_8 slot).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_AF370(uint32_t p1, int32_t* unit, void* p3, const D2GameBatchAD860Env* env)
{
    uint32_t rec = O(10695, p3 ? rd32(p3, 4) : 0xffffffffu, 0, 0, 0, 0);
    if (!rec) return 0;
    uint32_t ret = 0;
    for (int i = 0; i < 2; i++) {
        int32_t id = rd16((void*)(uintptr_t)rec, 0x9a + 2 * i);
        if (0 < id && id < (int32_t)rd32(env->dt, 0xc4)) {
            uint32_t sk = O(10871, unit, id, 0, 0, 0);
            if (sk) { O(11111, unit, sk, 0, 0, 0); O(11108, sk, 0, 0, 0, 0); }
            if (O(10494, unit, id, 0, 0, 0)) O(10945, unit, id, 0, 0, 0);
            ret = 1;
        }
    }
    if (env->subCE4D0(p1, unit, p3)) ret = 1;
    return ret;
}

// 1.13c D2Game+0xB06F0   p2[0] is a retail code pointer called as fn(p1, p2[3]).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_B06F0(uint32_t p1, const uint32_t* p2, int32_t* p3, const D2GameBatchAD860Env* env)
{
    if (*p3 == 0) {
        int32_t r = (int32_t)env->callPtr(p2[0], p1, p2[3]);
        if (r != 0) *p3 = r;
    }
}

// 1.13c D2Game+0xB08A0
extern "C" EMSCRIPTEN_KEEPALIVE uint8_t d2_fn_d2game_B08A0(const int32_t* p1, const D2GameBatchAD860Env* env)
{
    if (p1) {
        if (p1[0] == 0) return 0xe;
        if (p1[0] == 1 && p1[1] >= 0 && p1[1] < (int32_t)rd32(env->dt, 0xa80)) {
            uint32_t rec = p1[1] * 0x1a8 + rd32(env->dt, 0xa78);
            if (rec) return ((const uint8_t*)(uintptr_t)rec)[0x4e];
        }
    }
    return 0;
}

// 1.13c D2Game+0xB12B0   p4 = {unit*, count, dist, maxDist}.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_B12B0(uint32_t p1, uint32_t p2, int32_t* u, uint32_t* p4, const D2GameBatchAD860Env* env)
{
    (void)p1;
    int32_t d = (int32_t)env->subB0A20(u);
    if (d <= (int32_t)p4[3] && u && u[0] == 1 && u[4] == 0xc && (((uint32_t)u[0x31] >> 1) & 1)) {
        if (O(10541, u, 0, 0, 0, 0) == 0 && env->sub41260(u) == p2) {
            p4[1]++;
            p4[0] = (uint32_t)(uintptr_t)u;
            p4[2] = d;
        }
    }
    return 0;
}

// 1.13c D2Game+0xB3270   esi is Ghidra's unaff_ESI (the unit).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_B3270(const void* p1, int32_t* esi, const D2GameBatchAD860Env* env)
{
    if (esi && esi[0] == 1) {
        uint32_t rec = env->subC21320();
        if (rec) {
            const uint8_t* r = (const uint8_t*)(uintptr_t)rec;
            if (((const uint8_t*)p1)[0x11] != 0x0e) { esi[0x11] = (uint32_t)r[0x109] << 8; return; }
            O(10542, 0, 0, 0, 0, 0);
            esi[0x12] = (r[0x109] + 1) * 0x100;
        }
        return;
    }
    O(10542, 0, 0, 0, 0, 0);
    esi[0x12] = 0x500;
}

// 1.13c D2Game+0xB35A0   ebp is Ghidra's unaff_EBP (stack word for the second #10786).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_B35A0(uint32_t p1, uint32_t* unit, int32_t idx, uint32_t ebp, const D2GameBatchAD860Env* env)
{
    if (-1 < idx && idx < (int32_t)rd32(env->dt, 0xba0)) {
        uint32_t rec = idx * 0x23c + rd32(env->dt, 0xb98);
        if (rec) {
            uint32_t iVar1 = O(10511, 0, 0, 0, 0, 0);
            if (iVar1) {
                env->sub07A0(p1);
                uint32_t* tgt = (uint32_t*)(uintptr_t)O(10392, unit[0xb], 0, 0, 0, 0);
                if (tgt != unit && tgt) {
                    O(10936, 0, 0, 0, 0, 0);
                    O(10614, 0, 0, 0, 0, 0);
                    uint32_t pair[2] = { rd32((void*)(uintptr_t)rec, 0x138), (uint32_t)idx };
                    int32_t a = (int32_t)O(10786, unit, 0, 0, 0, 0);
                    O(10786, unit, rd32((void*)(uintptr_t)rec, 0x13c), ebp, iVar1, 0);
                    a += (int32_t)env->sub211D0(pair);
                    if (a < 1) a = 1;
                    O(10243, unit, a, 0, 0, 0);
                    O(10554, unit, tgt[3], 0, 0, 0);
                    O(10748, unit, tgt[0], 0, 0, 0);
                    return 1;
                }
            }
        }
    }
    return 0;
}

// 1.13c D2Game+0xB3EB0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_B3EB0(int32_t* p1, int32_t* p2, const D2GameBatchAD860Env* env)
{
    env->sub07A0((uint32_t)(uintptr_t)p1);
    int32_t* tgt = (int32_t*)(uintptr_t)O(10392, p2[0xb], 0, 0, 0, 0);
    if (tgt != p2 && tgt) {
        uint32_t buf[0x1c] = {0};
        env->subDDDE0(0x80, p1, tgt); (void)buf;
        return 1;
    }
    return 0;
}
