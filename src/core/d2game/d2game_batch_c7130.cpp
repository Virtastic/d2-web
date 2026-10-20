// src/core/d2game/d2game_batch_c7130.cpp -- twelve 1.13c D2Game.dll quest/unit helpers transcribed from
// the cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000.
// Stage A batch lane: implementation + marker only. Records are opaque host blocks read at their
// retail byte offsets; pointer fields are 32-bit retail values resolved through env->ptr. Register-only
// arguments (in_EAX / unaff_ESI / unaff_EDI) are explicit parameters. Unmapped callees are Env seams in
// retail argument order. Fatal path: Ordinal 10265(code) -> 10024(text) -> _exit(-1).
// The two unit-spawn bodies (0xC8CD0, 0xC94E0) share one inlined SUnit init; the decompile's duplicated
// alloc-failed arm of 0xC8CD0 is a stack-variable artefact and is modelled as the early return.
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

static inline uint8_t  r8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint32_t r32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void w8 (void* p, uint32_t o, uint8_t v)  { ((uint8_t*)p)[o] = v; }
static inline void w16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
static inline void w32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

struct D2GameBatchC7130Env {
    void*    (*ptr)(uint32_t v);                       // retail 32-bit pointer -> host record
    uint32_t (*ord10265)(uint32_t code);
    void     (*ord10024)(uint32_t text, uint32_t code);
    uint32_t (*ord10920)();                            // Ordinal_10920
    void     (*sendPacket8A3E0)(const void* buf, uint32_t len);
    uint8_t  (*subC6A30)(void* ecx);
    void     (*ord11090)(void* unit, uint32_t v);
    void     (*regCb)(uint32_t fn, uint32_t arg);      // 0xDFCC0
    void     (*sub1780)(void* p);
    void     (*sub33AF0)(void* quest);
    uint32_t (*ord10045)(const char* file, uint32_t line, uint32_t z);   // alloc, returns retail ptr
    void*    (*ord10688)(uint32_t classId);
    void*    (*ord11153)(uint32_t arg, uint32_t kind);
    uint32_t (*ord10331)(void* u);
    uint32_t (*ord10826)(uint32_t v);
    uint8_t  (*ord10459)(uint32_t v);
    void     (*sub0E0110)();
    void     (*ord10445)(void* u);
    void     (*sub2F300)();
    void     (*sub0E0F40)(void* u, void* a, void* b);
    void     (*sub8EA80)(void* game, void* unit, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);
    void     (*ord10141)();
    void     (*sub54B80)(void* unit, uint32_t v);
    void     (*sub55EB0)();
    uint32_t (*cbArg)(uint32_t i);                     // register arguments of the three 0xDFCC0 calls
    uint32_t (*ord10708)(uint32_t v);
    uint32_t (*sub57890)(uint8_t a, uint32_t b);
    uint32_t (*ord10830)();
    uint8_t  (*ord10459u)(uint32_t v);
    const uint8_t* dataTables;                         // sgptDataTables block
    uint32_t (*sub21100)(void* u); uint32_t (*sub21130)(void* u); uint32_t (*sub21280)();
    void     (*ord10346)(uint32_t v, uint32_t w);
};

[[noreturn]] static void fatal(const D2GameBatchC7130Env* e, uint32_t code) {
    e->ord10024(0x6fd19ff9, e->ord10265(code));
    _exit(-1);
}

// Shared SUnit init tail of 0xC8CD0 / 0xC94E0 (classId 0x13e / 100, mode byte 0xb, SUnit.cpp:0x265).
static void spawnInit(const D2GameBatchC7130Env* e, void* u, void* game, uint32_t classId, uint32_t kind4)
{
    w32(u, 0, 2); w32(u, 4, classId); w32(u, 8, r32(game, 0x1c)); w32(u, 0x80, 0); // +0x80 = game ptr
    uint32_t f = r32(u, 0xc8);
    f = r32(game, 0x70) == 0 ? (f & 0xfdffffff) : (f | 0x2000000);
    f |= 0x4000000; w32(u, 0xc8, f);
    uint8_t b = e->ord10459(e->ord10826(e->ord10331(u)));
    w8(u, 0x18, b); w32(u, 0x1c, r32(game, 0xbc + b * 4)); w32(u, 0x74, 0);
    w32(u, 0xc4, r32(u, 0xc4) | 0x10); w32(u, 0xd0, 0xb);
    uint32_t n = r32(game, 0x98) + 1; if (n == 0xffffffffu) n = 1; w32(game, 0x98, n);
    uint32_t p = e->ord10045("..\\Source\\D2Game\\UNIT\\SUnit.cpp", 0x265, 0);
    w32(u, 0x14, p); std::memset(e->ptr(p), 0, 0x38);
    w32(e->ptr(p), 0, (uint32_t)(uintptr_t)e->ord10688(classId));
    w32(u, 0x10, kind4); e->ord10445(u); e->sub2F300();
}

// 1.13c D2Game+0xC7130
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_c7130(void* ecx, void* quest, int32_t* edi, const D2GameBatchC7130Env* e)
{
    uint8_t* d = (uint8_t*)e->ptr(r32(quest, 0x18));
    if (e->subC6A30(ecx)) { d[2] = 1; return; }
    d[3] = 1;
    uint8_t pkt[6]; uint32_t id = 0x10a5d; std::memcpy(pkt, &id, 4); pkt[4] = pkt[5] = 0;
    if (edi && *edi == 0) e->ord10920();
    e->sendPacket8A3E0(pkt, 6);
}

// 1.13c D2Game+0xC71A0
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_c71a0(void* ecx, void* quest, void* param1, const D2GameBatchC7130Env* e)
{
    uint8_t* d = (uint8_t*)e->ptr(r32(quest, 0x18));
    d[5] = 1;
    uint32_t u = r32(param1, 4);
    w32(d, 0xc, u == 0 ? 0xffffffffu : r32(e->ptr(u), 0xc));
    if (r8(quest, 9) == 1) {
        if (r8(quest, 0xc) < 2) {
            if (e->subC6A30(ecx)) d[2] = 1; else d[3] = 1;
            w8(quest, 0xc, 3); w8(quest, 0x14, 0);
        }
        if (r8(quest, 0xb) == 0) {
            w8(quest, 0x14, 0); w8(quest, 0xb, 2);
            e->regCb(0x6fce52b0, 0);
        }
    }
    e->ord11090(u ? e->ptr(u) : nullptr, r32(d, 8));
}

// 1.13c D2Game+0xC7250
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_c7250(void* esi, const D2GameBatchC7130Env* e)
{
    uint8_t* d = (uint8_t*)e->ptr(r32(esi, 0x18));
    if (r8(esi, 0xc) == 0 && d[1] == 0) {
        uint32_t v = r32(e->ptr(r32(esi, 4)), 0x10f4);
        if (v == 0) fatal(e, 0x1fb);
        e->sub1780((uint8_t*)e->ptr(v) + 0x18);
        e->sub33AF0(esi);
        d[1] = 1;
    }
}

// Quest-record constructor shared by 0xC7480 / 0xC9400.
static void questInit(const D2GameBatchC7130Env* e, void* q, const uint32_t (&cb)[6], const char* file,
                      uint32_t line, uint32_t a2e0, uint32_t a10, uint8_t b0d, uint32_t zeroDwords)
{
    std::memset((uint8_t*)q + 0xa0, 0, 0x3c);
    w32(q, 0xa0, cb[0]); w32(q, 0xcc, cb[1]); w32(q, 0xac, cb[2]); w32(q, 0xc8, cb[3]); w32(q, 0xd4, cb[4]);
    w32(q, 0xdc, cb[5]); w8(q, 10, 1); w8(q, 0xc, 0);
    uint32_t p = e->ord10045(file, line, 0);
    w32(q, 0xe0, a2e0); w32(q, 0x10, a10); w32(q, 0xe8, 0); w32(q, 0x18, p); w8(q, 0xd, b0d);
    std::memset(e->ptr(p), 0, zeroDwords * 4);
}

// 1.13c D2Game+0xC7480
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_c7480(void* q, const D2GameBatchC7130Env* e)
{
    static const uint32_t cb[6] = {0x6fce37f0, 0x6fce6b30, 0x6fce73a0, 0x6fce3970, 0x6fce72c0, 0x6fd241f8};
    questInit(e, q, cb, "..\\Source\\D2Game/Quests/a2q3.cpp", 0x323, 0xb, 0xb, 4, 0x28);
    w32(q, 0xec, 0x6fce2250); w32(q, 0xf0, 0x6fce21e0);
    uint8_t* p = (uint8_t*)e->ptr(r32(q, 0x18));
    w32(p, 8, 0); w16(p, 0x94, 0);
}

// 1.13c D2Game+0xC9400
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_c9400(void* q, const D2GameBatchC7130Env* e)
{
    static const uint32_t cb[6] = {0x6fce4ea0, 0x6fce85a0, 0x6fce9190, 0x6fce3620, 0x6fce3320, 0x6fd246a8};
    w32(q, 0xa8, 0x6fce5db0);
    questInit(e, q, cb, "..\\Source\\D2Game/Quests/a2q4.cpp", 0x3dd, 0xc, 0xd, 5, 0x12);
    w16(q, 0x9c, 0); w32(q, 0xec, 0x6fce2080); w32(q, 0xf0, 0x6fce69d0);
    uint8_t* p = (uint8_t*)e->ptr(r32(q, 0x18));
    p[0] = p[1] = p[2] = p[3] = 0; p[4] = 0;
}

// 1.13c D2Game+0xC8CD0
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_c8cd0(void* quest, const D2GameBatchC7130Env* e)
{
    uint8_t* d = (uint8_t*)e->ptr(r32(quest, 0x18));
    e->ord10141();
    if (d[0xe] || d[0x11]) return;
    w16(d, 0x40, 0);
    void* game = e->ptr(r32(quest, 4));
    void* gctx = e->ptr(r32(game, 0x1c));
    void* u = e->ord11153((uint32_t)(uintptr_t)gctx, 2);
    if (!u) return;
    spawnInit(e, u, game, 0x13e, 0);
    e->sub0E0F40(u, game, d);
    w32(u, 0xc4, r32(u, 0xc4) | 0x3000000);
    w8(gctx, 0x11, 1); w32(gctx, 0x38, r32(u, 0xc));
}

// 1.13c D2Game+0xC94E0
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_c94e0(void* game, void* quest, const D2GameBatchC7130Env* e)
{
    uint8_t* d = (uint8_t*)e->ptr(r32(quest, 0x18));
    uint32_t found = 0;
    if (d[0xd] == 1) {
        if (d[6]) {
            if (!game) fatal(e, 0x2d3);
            for (uint32_t p = r32(game, 0x1520 + (r32(d, 0x28) & 0x7f) * 4); p; p = r32(e->ptr(p), 0xe4)) {
                void* u = e->ptr(p);
                if (r32(u, 0xc) == r32(d, 0x28)) {
                    found = 1;
                    if (r32(u, 0x10) == 0) {
                        if (d[0xc] == 0) {
                            e->ord11090(u, 1);
                            void* c = e->ord10688(r32(u, 4));
                            e->sub8EA80(e->ptr(r32(quest, 4)), u, 1,
                                        (r32(c, 0xdc) >> 8) + r32(e->ptr(r32(quest, 4)), 0xa8), 0, 0, 0);
                        } else e->ord11090(u, 2);
                    }
                    break;
                }
            }
        }
        if (d[4]) {
            if (!game) fatal(e, 0x2d3);
            for (uint32_t p = r32(game, 0x1520 + (r32(d, 0x20) & 0x7f) * 4); p; p = r32(e->ptr(p), 0xe4)) {
                void* u = e->ptr(p);
                if (r32(u, 0xc) == r32(d, 0x20)) {
                    e->sub21100(u); e->sub21130(u); e->ord10331(u);
                    if (e->sub21280()) {
                        void* n = e->ord11153(r32(game, 0x1c), 2);
                        if (n) {
                            spawnInit(e, n, game, 100, 1);
                            e->sub0E0F40(n, game, d);
                            if (d[0xc] == 0) {
                                e->ord11090(n, 1);
                                e->ord10346(e->ord10331(d), 1);
                                e->ord10346(0, 1);
                            } else e->ord11090(n, 2);
                        }
                    }
                    break;
                }
            }
        }
        if (!found) return 0;
        d[0xb] = 1; d[0xd] = 0;
    }
    d[3] = 0;
    return 1;
}

// 1.13c D2Game+0xC9A80
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_c9a80(int32_t* p, const D2GameBatchC7130Env* e)
{
    void* game = e->ptr(p[0]);
    uint32_t u = p[1];
    if (u == 0 || r32(e->ptr(u), 0x10) == 0) {
        e->ord11090(u ? e->ptr(u) : nullptr, 1);
        void* c = e->ord10688(p[4]);
        e->sub8EA80(game, u ? e->ptr(u) : nullptr, 1, (r32(c, 0xdc) >> 8) + r32(game, 0xa8), 0, 0, 0);
    }
    uint32_t list = r32(game, 0x10f4);
    if (!list) return 0;
    uint32_t q = r32(e->ptr(list), 0);
    if (!q) return 0;
    while (r32(e->ptr(q), 0) != 0xb) { q = r32(e->ptr(q), 0xf4); if (!q) return 0; }
    void* qr = e->ptr(q);
    uint8_t* d = (uint8_t*)e->ptr(r32(qr, 0x18));
    e->sub54B80(e->ptr(p[1]), 0x18c);
    w32(d, 8, e->ord10331(e->ptr(p[1])));
    if (r8(qr, 9) != 0 && r8(qr, 0xc) != 5) {
        e->sub55EB0();
        for (uint32_t i = 0; i < 3; i++) e->regCb(e->cbArg(i), 0);
    }
    return 0;
}

// 1.13c D2Game+0xCA490
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_ca490(const int32_t* unit, const int32_t* table, const D2GameBatchC7130Env* e)
{
    int32_t idx = 0;
    if (unit && unit[0] == 1) {
        const uint8_t* x = (const uint8_t*)e->ptr(unit[5]);
        if (unit[5]) { if (r8(x, 0x5c) & 2) return; idx = (int32_t)r32(x, 0x58); }
    }
    uint8_t* rec = (uint8_t*)e->ptr((uint32_t)table[idx]);
    if (e->ord10830() == 0) w32(rec, 0x2d0, r32(rec, 0x2d0) - 1);
}

// 1.13c D2Game+0xCB370
extern "C" EMSCRIPTEN_KEEPALIVE
uint8_t d2_fn_d2game_cb370(void* ecx, void* eax, const D2GameBatchC7130Env* e)
{
    uint8_t a = e->ord10459(e->ord10826(e->ord10331(ecx)));
    uint8_t b = e->ord10459(e->ord10826(e->ord10331(eax)));
    return (uint8_t)(((a == b) - 1u) & 2);
}

// 1.13c D2Game+0xCBC70
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2_fn_d2game_cbc70(const void* p1, int32_t p2, const D2GameBatchC7130Env* e)
{
    if (p2 != 9) return 3;
    uint32_t v = r32(p1, 1);
    if ((v >> 16) > 0xf) return 3;
    uint32_t n = v & 0x7fff;
    int32_t lim = (int32_t)r32(e->dataTables, 0xba0);
    if (lim < (int32_t)n) n = 0xffffffffu;
    else if (n != 0xffffffffu && (lim <= (int32_t)n || e->ord10708(r32(p1, 5)) == 0)) return 3;
    return e->sub57890((uint8_t)n, (v >> 15) & 1) != 0 ? 0 : 3;
}

// 1.13c D2Game+0xCBED0
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_cbed0(const int32_t* eax, const D2GameBatchC7130Env* e, void (*sub8B400)())
{
    if (eax && *eax == 0) e->ord10920();
    sub8B400();
}
