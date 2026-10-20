// src/core/d2game/d2game_batch_c5190.cpp -- twelve 1.13c D2Game.dll quest-callback functions transcribed
// from the cached Ghidra decompiles. Stage A batch lane: implementation + marker only. Base 0x6FC20000.
// Register-only arguments (in_EAX / unaff_ESI / extraout_ECX) are explicit parameters. Unmapped callees
// are seams on D2GameBatchC5190Env in retail argument order. Retail code addresses stored into the quest
// record (FUN_6fce4570, LAB_6fce4620, ...) arrive as env pointers. Fatal tail: Ordinal_10265/10024/_exit(-1).
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <emscripten.h>

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void*    rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void     wr8 (void* p, uint32_t o, uint8_t v)  { ((uint8_t*)p)[o] = v; }
static inline void     wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void     wrptr(void* p, uint32_t o, const void* v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }

struct D2GameBatchC5190Env {
    uint32_t (*ordinal10265)(uint32_t id);
    void     (*ordinal10024)(const char* module, uint32_t id);   // tail is _exit(-1)
    void*    (*ordinal10920)(void* unit);                        // unit -> player record (+0x10 slots, +0x9C)
    int32_t  (*ordinal10174)(uint32_t a, uint32_t b, uint32_t c);
    void     (*sub8A3E0)(void* buf, uint32_t len);               // 0x8A3E0 send packet
    void     (*sub554F0)(void* unit, void* arg);                 // 0x354F0 (a.k.a. FUN_6fc554f0)
    int16_t  (*sub9BEB0)();                                      // 0x9BEB0
    void     (*sub9C160)(const void* cb);                        // 0x9C160
    const void* cbCE2E70; const void* cbCE3900; const void* cbCE5910;   // LAB/FUN callbacks
    void     (*subDFCC0)(void* thisp, uint32_t zero);            // 0xDFCC0
    int32_t  (*sub4170)();                                       // 0xC4170
    void     (*ordinal11090)(void* unit, uint32_t v);
    void     (*ordinal10408)(void* unit);
    void     (*ordinal10895)(uint32_t v);
    void*    (*ordinal10045)(const char* file, uint32_t line, uint32_t zero);
    uint32_t (*ordinal10695)(uint32_t v);
    uint32_t (*ordinal10331)(void* unit);
    uint32_t (*ordinal10826)(uint32_t v);
    uint8_t  (*ordinal10459)(uint32_t v);
    int32_t  (*ordinal10518)(uint32_t v);
    void     (*sub53830)(void* p);                               // 0x33830
    void     (*sub55EB0)();                                      // 0x35EB0
    void     (*subC2A90)(uint32_t a, uint32_t b);                // 0xC2A90
    void     (*ordinal10337)(uint32_t v);
    void     (*subCBE70)(void* ecx, const char* msg);            // 0xCBE70
    int32_t  debugFlag;                                          // DAT_6fd31c20
    const void* a2q0Fn[10];  // +0xA0, +0xCC, +0xC8, +0xD4, +0xDC, +0xE8, +0xEC retail code/data addresses
};

static void fatal(const D2GameBatchC5190Env* e, uint32_t line) {
    static const char kModule[] = "D2Game.dll";
    e->ordinal10024(kModule, e->ordinal10265(line));
    _exit(-1);
}

// 0xC5190: if the player record's +0x9C block has byte +0x1AC == 1, send packets 0x553.. (10 bytes) and 5D 0A 10 (6 bytes).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_C5190(uint32_t p1, void* unit, const D2GameBatchC5190Env* env) {  // 1.13c D2Game+0xC5190
    (void)p1;
    if (unit != nullptr && rd32(unit, 0) == 0) {
        void* rec = env->ordinal10920(unit);
        void* blk = rdptr(rec, 0x9C);
        if (blk != nullptr) {
            if (rd8(blk, 0x1AC) == 1) {
                uint8_t a[10] = {};
                wr32(a, 0, 0x553); wr16(a, 8, 0x100);
                env->sub8A3E0(a, 10);
                const uint8_t b[6] = {0x5D, 0x0A, 0x10, 0, 0, 0};
                env->sub8A3E0((void*)b, 6);
            }
            return 0;
        }
    }
    fatal(env, 0x7CB);
    return 0;
}

// Shared body of 0xC52B0 (class 0xB) and 0xC5630 (class 9): quest-state check on the owner's slot.
static uint32_t slotCheck(void* p1, void* unit, uint32_t cls, const D2GameBatchC5190Env* env) {
    void* rec = env->ordinal10920(unit);
    const uint32_t s = rd32(rec, 0x10 + rd8(p1, 0x6D) * 4);
    if (((env->ordinal10174(s, cls, 0) != 0 || env->ordinal10174(s, cls, 0xF) != 0) &&
         env->ordinal10174(s, cls, 0xD) == 0 && env->ordinal10174(s, cls, 0xE) == 0)) return 0;
    env->sub554F0(unit, p1);
    return 0;
}
// 0xC52B0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_C52B0(void* p1, void* unit, const D2GameBatchC5190Env* env) { return slotCheck(p1, unit, 0xB, env); }  // 1.13c D2Game+0xC52B0
// 0xC5630
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_C5630(void* p1, void* unit, const D2GameBatchC5190Env* env) { return slotCheck(p1, unit, 0x9, env); }  // 1.13c D2Game+0xC5630

// 0xC5690: reset the 0xF-dword callback table at +0xA0 and install the a2q0 handlers; allocate its 0x80-byte state.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_C5690(void* q, const D2GameBatchC5190Env* env) {  // 1.13c D2Game+0xC5690
    std::memset((uint8_t*)q + 0xA0, 0, 0xF * 4);
    wrptr(q, 0xA0, env->a2q0Fn[0]);
    wrptr(q, 0xCC, env->a2q0Fn[1]);
    wrptr(q, 0xC8, env->a2q0Fn[2]);
    wrptr(q, 0xD4, env->a2q0Fn[3]);
    wrptr(q, 0xDC, env->a2q0Fn[4]);
    wr8(q, 10, 1); wr8(q, 0xC, 0); wr8(q, 0xB, 0);
    wr32(q, 0x18, 0);
    wr32(q, 0xE0, 8);
    wrptr(q, 0xE8, env->a2q0Fn[5]);
    wrptr(q, 0xEC, env->a2q0Fn[6]);
    void* st = env->ordinal10045("..\\Source\\D2Game/Quests/a2q0.cpp", 0xAC, 0);
    wrptr(q, 0x18, st);
    wr16(st, 0x80, 0);
}

// Shared body of 0xC5B80 / 0xC5ED0 / 0xC65A0: when slot (cls,0xD) is set and the unit is a player, register a callback.
static uint32_t cbCheck(void* p1, void* unit, uint32_t cls, const void* cb, const D2GameBatchC5190Env* env) {
    void* rec = env->ordinal10920(unit);
    if (env->ordinal10174(rd32(rec, 0x10 + rd8(p1, 0x6D) * 4), cls, 0xD) != 0 &&
        unit != nullptr && rd32(unit, 0x80) != 0 && rd32(unit, 0) == 0 && env->sub9BEB0() != -1)
        env->sub9C160(cb);
    return 0;
}
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_C5B80(void* p1, void* unit, const D2GameBatchC5190Env* env) { return cbCheck(p1, unit, 0xD, env->cbCE2E70, env); }  // 1.13c D2Game+0xC5B80
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_C5ED0(void* p1, void* unit, const D2GameBatchC5190Env* env) { return cbCheck(p1, unit, 0xB, env->cbCE3900, env); }  // 1.13c D2Game+0xC5ED0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_C65A0(void* p1, void* unit, const D2GameBatchC5190Env* env) { return cbCheck(p1, unit, 0xE, env->cbCE5910, env); }  // 1.13c D2Game+0xC65A0

// 0xC5D10 (EAX=q, ECX=this, stack=game): mark the quest, drop its unit from the game's 0x1520 hash and destroy it.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_C5D10(void* q, void* thisp, void* game, const D2GameBatchC5190Env* env) {  // 1.13c D2Game+0xC5D10
    wr8(q, 0xE, 1);
    if (rd16(q, 0x40) == 0) wr8(q, 0x46, 1);
    wr16(q, 0x40, 2);
    env->subDFCC0(thisp, 0);
    if (rd8(q, 0x11) == 1) {
        void* tbl = rdptr(game, 4);
        if (tbl == nullptr) fatal(env, 0x2D3);
        void* u = rdptr(tbl, 0x1520 + (rd32(q, 0x38) & 0x7F) * 4);
        while (u != nullptr) {
            if (rd32(u, 0xC) == rd32(q, 0x38)) { env->ordinal11090(u, 2); env->ordinal10408(u); return; }
            u = rdptr(u, 0xE4);
        }
    }
}

// 0xC5E90 (EAX=q, ECX=this, stack=ctx)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_C5E90(void* q, void* thisp, void* ctx, const D2GameBatchC5190Env* env) {  // 1.13c D2Game+0xC5E90
    if (rd32(q, 0xC0) != 0) {
        wr8(rdptr(ctx, 0x18), 2, 0);
        env->ordinal10895(rd32(q, 0xC0));
        env->subDFCC0(thisp, 0);
    }
}

// 0xC5F30: pick the next state from the owner's unit class and the (10,3) slot, then run 0xC4170 / 0x354F0.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_C5F30(void* p1, void* p2, const D2GameBatchC5190Env* env) {  // 1.13c D2Game+0xC5F30
    void* sub = rdptr(p2, 8);
    const uint32_t id = sub == nullptr ? 0xFFFFFFFFu : rd32(sub, 4);
    uint8_t* e = (uint8_t*)(uintptr_t)env->ordinal10695(id);
    void* owner = rdptr(p2, 0);
    void* unit = rdptr(p2, 0xC);
    void* rec = env->ordinal10920(unit);
    const int32_t slot = env->ordinal10174(rd32(rec, 0x10 + rd8(owner, 0x6D) * 4), 10, 3);
    wr8(p1, 0x14, 0);
    const uint8_t cv = env->ordinal10459(env->ordinal10826(env->ordinal10331(unit)));
    if (rd8(p1, 8) == cv) env->sub4170();
    const uint32_t k = rd32(e, 0x80);
    if (k == 0x20317274 && slot == 0) wr8(p1, 0xB, 1);
    else if (k == 0x20706976 || k == 0x20786f62 || k == 0x2066736d) wr8(p1, 0xB, slot == 0 ? 6 : 2);
    else return;
    void* arg = rdptr(p1, 4);
    if (env->sub4170() != 0) env->sub554F0(unit, arg);
}

// 0xC6450 (ECX=q, stack=msg): react to quest messages 0x28..0x4A.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_C6450(void* q, void* msg, const D2GameBatchC5190Env* env) {  // 1.13c D2Game+0xC6450
    const int32_t m = (int32_t)rd32(msg, 0x18);
    if (m < 0x28 || m > 0x4A) return;
    if (rd32(msg, 0x14) == 0x28 && rd16(q, 0x9C) != 0) env->sub53830((uint8_t*)q + 0x1C);
    if (rd8(q, 9) == 0) return;
    uint8_t* st = (uint8_t*)rdptr(q, 0x18);
    bool second = false;
    if (rd8(st, 1) == 0) {
        if (rd32(st, 0x34) == 0) wr32(st, 0x34, (uint32_t)env->ordinal10518(rd32(rdptr(msg, 0), 0xC0)));
        if (rd32(st, 0x34) == 0) return;
        if ((uint32_t)m != rd32(st, 0x34)) {
            if (m == 0x2E) {
                if (rd8(q, 0xC) == 0) { env->sub55EB0(); env->subC2A90(rd32(msg, 0), rd32(msg, 0xC)); }
                if (rd8(q, 0xB) != 0) return;
                wr8(q, 0x14, 0); wr8(q, 0xB, 1);
                env->subDFCC0(q, 0);
                return;
            }
            second = true;
        } else {
            if (rd8(q, 0xC) == 2) return;
            env->sub55EB0();
            if (rd8(q, 0xB) > 1) { env->subC2A90(rd32(msg, 0), rd32(msg, 0xC)); return; }
        }
    } else second = true;
    if (second) {
        if (m != 0x49) return;
        if (rd8(q, 0xC) > 1) return;
        env->sub55EB0();
        if (rd8(q, 0xB) > 1) return;
    }
    wr8(q, 0x14, 0); wr8(q, 0xB, 2);
    env->subC2A90(rd32(msg, 0), rd32(msg, 0xC));
}

// 0xC6A30 (ECX=this, ESI=q)
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_C6A30(void* thisp, void* q, const D2GameBatchC5190Env* env) {  // 1.13c D2Game+0xC6A30
    wr8(q, 0x14, 0); wr8(q, 0xB, 1);
    env->subDFCC0(thisp, 0);
    if (rd8(q, 0xC) == 0) {
        wr8(q, 0xC, 1);
        char buf[128];
        std::snprintf(buf, sizeof buf, "changing state to %d\n", 1);
        if (env->debugFlag != 0) env->subCBE70(thisp, buf);
    }
    const uint32_t u = rd32(rdptr(q, 4), 0xC0);
    if (u == 0) {
        void* st = rdptr(q, 0x18);
        wr8(st, 3, 1);
        return (uint32_t)(uintptr_t)st & 0xFFFFFF00u;
    }
    env->ordinal10337(u);
    env->subDFCC0(nullptr, 0);
    return 1;
}
