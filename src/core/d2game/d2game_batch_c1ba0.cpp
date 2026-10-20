// src/core/d2game/d2game_batch_c1ba0.cpp -- twelve 1.13c D2Game.dll functions transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000.
//
// Quest-state helpers: walk the game's list (game+0x10F4 -> head dword; node: +0 type, +0x18 data ptr,
// +0xF4 next) for a given node type. Register-only arguments (in_EAX, unaff_EDI, ECX/EDX of __fastcall)
// are explicit parameters. Guest pointers are uint32 resolved through env->mem; unmapped callees and the
// D2Common ordinals are seams on D2GameBatchC1BA0Env in retail argument order.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline void     wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
static inline uint8_t  rd8(const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline void     wr8(void* p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }

struct D2GameBatchC1BA0Env {
    void*    (*mem)(uint32_t guest);                          // guest pointer -> host (null stays null)
    const uint32_t* tableC5950;                               // DAT_6fd25950, 7 dwords
    int32_t  (*ordinal10518)(uint32_t v);                     // D2Common #10518
    uint32_t (*ordinal10920)(void* v);                        // #10920 (EAX = result ptr)
    int32_t  (*ordinal10174)(uint32_t a, uint32_t b, uint32_t c);
    void     (*ordinal10003)(uint32_t a, uint32_t b, uint32_t c);
    uint32_t (*ordinal10331)(void* v);
    uint32_t (*ordinal10826)(uint32_t v);
    int8_t   (*ordinal10459)(uint32_t v);
    void     (*ordinal10383)(uint32_t v, uint32_t* outA, uint32_t* outB);  // outA = stack-8 slot
    uint32_t (*sub33F30)(uint32_t arg);                       // 0x33F30 -> guest ptr or 0
    int32_t  (*isBadCodePtr)(uint32_t p);                     // kernel32 IsBadCodePtr
    void     (*fatal)(uint32_t code);                         // Ordinal_10265(code); Ordinal_10024(..); _exit(-1)
    uint32_t (*callCb)(uint32_t cb, void* self);              // (*cb)() with the node in the live register
    uint32_t (*subB07B0)(void);                               // 0xB07B0
    uint32_t (*sub1A00)(void* game);                          // 0xC1A00 (EAX = game)
};

static uint8_t* findNode(const D2GameBatchC1BA0Env* e, const void* game, uint32_t type)
{
    uint8_t* list = (uint8_t*)e->mem(rd32(game, 0x10f4));
    if (!list) return nullptr;
    for (uint8_t* n = (uint8_t*)e->mem(rd32(list, 0)); n; n = (uint8_t*)e->mem(rd32(n, 0xf4)))
        if (rd32(n, 0) == type) return n;
    return nullptr;
}

// Shared tail of 1FF0 / 21E0 / 24A0: look the callback owner up and call its +0xF0 slot.
static uint32_t dispatchCb(const D2GameBatchC1BA0Env* e, void* self, uint32_t fatalCode)
{
    uint8_t* o = (uint8_t*)e->mem(e->sub33F30(rd32(self, 0x10)));
    if (!o) return 0;
    if (e->isBadCodePtr(rd32(self, 0xf0))) e->fatal(fatalCode);
    return e->callCb(rd32(o, 0xf0), self);
}

// 1.13c D2Game+0xC1BA0  (EAX = unit-ish; +4 -> +0xC0 feeds #10518; +0x18 is the shuffle state)
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_C1BA0(void* in, const D2GameBatchC1BA0Env* e)
{
    int32_t k = e->ordinal10518(rd32(e->mem(rd32(in, 4)), 0xc0));
    if (k == 0) return 0x133;
    uint8_t* st = (uint8_t*)e->mem(rd32(in, 0x18));
    if (rd8(st, 0x38) == 0) {
        int32_t n = 0, i = 0;
        uint32_t o = 0x48;
        wr8(st, 0x38, 1);
        for (int32_t left = 7; left; --left) {
            if (i == k - 0x42) { ++i; }
            else if (i < 7 && n < 6) { wr32(st, o, e->tableC5950[i]); ++n; o += 4; ++i; }
        }
    }
    if (rd16(st, 0x3a) == 6) wr16(st, 0x3a, 0);
    uint32_t r = rd32(st, 0x48 + (uint32_t)rd16(st, 0x3a) * 4);
    wr16(st, 0x3a, (uint16_t)(rd16(st, 0x3a) + 1));
    return r;
}

// 1.13c D2Game+0xC1FF0  (EDI = self)
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_C1FF0(void* self, const D2GameBatchC1BA0Env* e)
{ return dispatchCb(e, self, 0x34e); }

// 1.13c D2Game+0xC2120  (EAX = unit-ish; returns byte at node10.data+4)
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_C2120(void* in, const D2GameBatchC1BA0Env* e)
{
    uint8_t* n = findNode(e, e->mem(rd32(in, 4)), 10);
    return n ? rd8(e->mem(rd32(n, 0x18)), 4) : 0;
}

// 1.13c D2Game+0xC21E0  (ECX = self)
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_C21E0(void* self, const D2GameBatchC1BA0Env* e)
{
    if (rd8(self, 0xc) != 5 && rd8(self, 9) != 0) return 1;
    return dispatchCb(e, self, 0x2a5);
}

// 1.13c D2Game+0xC2380  (EAX = unit-ish, param_1 the addend)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_C2380(void* in, int32_t add, const D2GameBatchC1BA0Env* e)
{
    uint8_t* n = findNode(e, e->mem(rd32(in, 4)), 9);
    if (n) { uint8_t* d = (uint8_t*)e->mem(rd32(n, 0x18)); wr32(d, 0x24, rd32(d, 0x24) + (uint32_t)add); }
}

// 1.13c D2Game+0xC2460  (EAX = game)
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_C2460(void* game, const D2GameBatchC1BA0Env* e)
{
    uint8_t* n = findNode(e, game, 8);
    if (n && rd8(n, 9) != 0) return rd8(n, 0xc) == 5;
    return 1;
}

// 1.13c D2Game+0xC24A0  (ECX = self)
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_C24A0(void* self, const D2GameBatchC1BA0Env* e)
{
    if (rd8(self, 0xc) != 5 && rd8(self, 9) != 0) return 1;
    return dispatchCb(e, self, 0x243);
}

// 1.13c D2Game+0xC2A90  (ECX = game, EDX = unit)
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_C2A90(void* game, void* unit, const D2GameBatchC1BA0Env* e)
{
    uint32_t t = rd32(e->mem(e->ordinal10920(unit)), 0x10 + (uint32_t)rd8(game, 0x6d) * 4);
    if (e->ordinal10174(t, 0xe, 0) != 1) {
        uint8_t* n = findNode(e, game, 0xd);
        if (n && rd8(n, 0xc) > 1) e->ordinal10003(t, 0xe, 2);
    }
    return 0;
}

// 1.13c D2Game+0xC2E70  (ECX = game, EDX = unit)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_C2E70(void* game, void* unit, const D2GameBatchC1BA0Env* e)
{
    if (!findNode(e, game, 0xc)) return;
    uint32_t t = rd32(e->mem(e->ordinal10920(unit)), 0x10 + (uint32_t)rd8(game, 0x6d) * 4);
    if (e->ordinal10174(t, 0xd, 0) == 1 || e->ordinal10174(t, 0xd, 1) == 1) return;
    uint32_t a = e->ordinal10331(unit);
    if (!a) return;
    a = e->ordinal10826(a);
    if (!a) return;
    if (e->ordinal10459(a) == 1) { e->ordinal10003(t, 0xd, 0xd); e->ordinal10003(t, 0xd, 1); }
}

// 1.13c D2Game+0xC2F10  (ECX = game, EDX = unit, EBX = id array, EBP = its count)
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_C2F10(void* game, void* unit, const uint32_t* ids, uint32_t count,
                                                 const D2GameBatchC1BA0Env* e)
{
    uint32_t t = rd32(e->mem(e->ordinal10920(unit)), 0x10 + (uint32_t)rd8(game, 0x6d) * 4);
    uint32_t a = e->ordinal10331(unit);
    if (!a) return 0;
    uint8_t* n = findNode(e, game, 0xc);
    if (!n) e->fatal(0x10d);
    uint32_t want = rd32(e->mem(rd32(n, 0x18)), 4);
    bool hit = false;
    if (a == want) hit = true;
    else {
        a = e->ordinal10331(unit);
        if (!a) e->fatal(0x11c);
        uint32_t o1, o2;                         // stack0xfffffff4 / fffffff0; count lands in EBP
        e->ordinal10383(a, &o1, &o2);
        for (uint32_t i = 0; i < count; ++i) if (ids[i] == want) { hit = true; break; }
    }
    if (e->ordinal10174(t, 0xd, 0) == 0 && e->ordinal10174(t, 0xd, 1) == 0 && hit) {
        e->ordinal10003(t, 0xd, 0xd); e->ordinal10003(t, 0xd, 1);
    }
    return 0;
}

// 1.13c D2Game+0xC31C0  (ECX = game, EDX = unit)
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_C31C0(void* game, void* unit, const D2GameBatchC1BA0Env* e)
{
    uint32_t t = rd32(e->mem(e->ordinal10920(unit)), 0x10 + (uint32_t)rd8(game, 0x6d) * 4);
    if (e->ordinal10174(t, 0xe, 0) == 1 || e->ordinal10174(t, 0xe, 1) == 1) return 0;
    uint8_t* n = findNode(e, game, 0xb);                    // retail dereferences n unchecked
    uint8_t* d = (uint8_t*)e->mem(rd32(n, 0x18));
    if (e->subB07B0() >= 0x1f) return 0;
    wr8(d, 0x1a, 1);
    wr32(d, 0x1c, unit ? rd32(unit, 0xc) : 0xffffffffu);
    return 1;
}

// 1.13c D2Game+0xC3260  (EAX = game; EDX out of 0xC1A00 is the node data)
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_C3260(void* game, const D2GameBatchC1BA0Env* e)
{
    uint8_t* n = findNode(e, game, 0xb);
    if (n) {
        uint8_t* d = (uint8_t*)e->mem(rd32(n, 0x18));
        if (rd8(d, 0xd) != 0 && (uint8_t)d2_fn_d2game_C2460(game, e) == 0 && (uint8_t)e->sub1A00(game) == 0
            && rd8(d, 9) == 1 && rd8(d, 0xc) < 2)
            return 0;
    }
    return 1;
}
