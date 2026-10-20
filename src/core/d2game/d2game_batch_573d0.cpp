// src/core/d2game/d2game_batch_573d0.cpp -- twelve 1.13c D2Game.dll quest-script functions (A1 quests,
// 0x573D0..0x687D0) transcribed from the cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp).
// Prior art: D2MOO 1.10f QUESTS/ quest callbacks (same shape, different offsets).
// Records are opaque host objects read at retail byte offsets. Callees src/ does not own are seams on
// D2G573Env in retail argument order. Ghidra in_EAX/unaff_* register inputs are marked REG?.
#include <cstdint>
#include <cstring>
#include <unistd.h>

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr8 (void* p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }
static inline void wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void* rdp(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }

[[noreturn]] static void fatal573(uint32_t) { _exit(-1); }

struct D2G573Env {
    void    (*setMode)(void* unit, int32_t mode);                          // D2Common #11090
    int32_t (*questFlag)(uint32_t questPtr, int32_t q, int32_t bit);       // #10174
    void*   (*getPlayerData)(void* unit);                                  // #10920
    void*   (*getUnitPtr)(void* x);                                        // #10331 (unit -> room/inv/etc.)
    int32_t (*vA568)(void* unit);                                          // 0xA568 helper
    int16_t (*fnBBEB0)();                                                  // 0x9BEB0
    void    (*fnBC160)(void* cb);                                          // 0x9C160 (callback = LAB_ addr)
    void*   cbLab80ec0; void* cbLab81ec0; void* cbLab821b0;               // retail LAB_6fc80ec0 / 81ec0 / 821b0
    bool    (*fn64B60)(void* q);                                           // 0x64B60
    int32_t (*fn538D0)(int32_t a, int32_t b);                              // 0x538D0
    int32_t (*rand10587)(void* a, int32_t b, int32_t c);                   // #10587
    void    (*fnDFCC0)(void* a, int32_t b);                                // 0xDFCC0 FUN_6fcffcc0
    int32_t debugOn;                                                       // DAT_6fd31c20
    void    (*fnCBE70)(void* a, const char* s);                            // 0xCBE70 debug log
    void*   (*fnE1D90)(int32_t a, int32_t b, uint8_t c, uint32_t d, void* e); // 0xE1D90 (REG? c,d,e)
    void    (*fn66320)(void* ctx);                                         // 0x66320
    void    (*fn660D0)(void* game, void* a);                               // 0x660D0
    void*   (*fn1100)(void* u); void* (*fn1130)(void* u); int32_t (*fn1280)(); // 0x1100/0x1130/0x1280
    void    (*ord10141)(void* a, void* b);                                 // #10141
    void*   (*alloc10045)(const char* file, int32_t line, int32_t z);      // #10045
    int32_t (*ord10750)(void* p);                                          // #10750
    int32_t (*ord10867)(void* p);                                          // #10867
    int32_t (*ord10459)(int32_t a);                                        // #10459
    int32_t (*ord10826)(void* p);                                          // #10826
    void*   (*fnC22E0)(void* game, void* a, void* b);                      // 0xA22E0 FUN_6fcc22e0
    uint8_t (*ord10258)();                                                 // #10258
    void    (*ord10111)(void* u, uint32_t v);                              // #10111
    void    (*ord10346)(void* a, void* b);                                 // #10346
    void    (*fn68410)(void* game, void* u, void* cb);                     // 0x48410
    void    (*ord11103)(void* u, int32_t v);                               // #11103
    void    (*fn95E10)(void* game, void* buf);                             // 0x75E10
    void*   cb82A30;
    // 0x664F0 callees
    void    (*fn833C0)(void* q);  void (*fn835B0)(void* q);  void (*fn55EB0)(); void (*fn55E90)(int32_t);
    int32_t (*fn2E660)(uint32_t tag);
    void    (*fnFFFD0)(int32_t a, void* u);
    void    (*fn566A0)(void* game);
    void*   (*fnC0000)(void* game);
    void    (*ord10003)(uint32_t q, int32_t a, int32_t b);
    void*   (*ord10331b)(void* u, int32_t** list, uint32_t* n);
    void    (*ord10383)(void* x);
    int16_t (*fnFFA60)();
    void*   cb82A80; void* cb816f0;
    // 0x687D0 callbacks
    void* cbLab[16]; void (*fnA2E6)(void*, int, int);
};
extern "C" D2G573Env* d2g573_env();

// 1.13c D2Game+0x573D0  (ECX = event ctx: [0]=game, [1]=unit)
extern "C" void d2game_573d0(int32_t* ctx)
{
    D2G573Env* e = d2g573_env();
    uint8_t* game = (uint8_t*)(uintptr_t)ctx[0];
    uint8_t* q = nullptr;
    void** head = (void**)rdp(game, 0x10f4);
    if (head) for (uint8_t* p = (uint8_t*)head[0]; p; p = (uint8_t*)rdp(p, 0xF4))
        if (rd32(p, 0) == 0x23) { q = p; break; }
    uint8_t* d = (uint8_t*)rdp(q, 0x18);      // null q faults in retail too
    void* unit = (void*)(uintptr_t)ctx[1];
    wr8(d, 0x58, 1);
    wr32(d, 0x5c, unit ? rd32(unit, 0xc) : 0xffffffffu);
    e->setMode(unit, (int32_t)rd32(d, 0x60));
    if (rd8(d, 0) == 0 && rd32(d, 0x60) != 2) {
        wr32(d, 0x60, 2);
        e->setMode(unit, 1);
    }
}

// shared body of 0x64150 / 0x64FE0 / 0x65200: (ECX=quest, EDX=unit)
static int32_t quest_flag_cb(const uint8_t* quest, uint8_t* unit, int32_t flagQ, void* cb)
{
    D2G573Env* e = d2g573_env();
    uint8_t* pd = (uint8_t*)e->getPlayerData(unit);
    int32_t r = e->questFlag(rd32(pd, 0x10 + rd8(quest, 0x6d) * 4), flagQ, 0xd);
    if (r && unit && rd32(unit, 0x80) && rd32(unit, 0) == 0 && e->fnBBEB0() != -1) e->fnBC160(cb);
    return 0;
}
// 1.13c D2Game+0x64150
extern "C" int32_t d2game_64150(const uint8_t* q, uint8_t* u) { return quest_flag_cb(q, u, 5, d2g573_env()->cbLab80ec0); }
// 1.13c D2Game+0x64FE0
extern "C" int32_t d2game_64fe0(const uint8_t* q, uint8_t* u) { return quest_flag_cb(q, u, 2, d2g573_env()->cbLab81ec0); }
// 1.13c D2Game+0x65200
extern "C" int32_t d2game_65200(const uint8_t* q, uint8_t* u) { return quest_flag_cb(q, u, 1, d2g573_env()->cbLab821b0); }

// 1.13c D2Game+0x64940  (ECX=param_1 -> FUN_6fcffcc0 arg, ESI=quest REG?)
extern "C" void d2game_64940(void* param1, uint8_t* q /*REG? ESI*/)
{
    D2G573Env* e = d2g573_env();
    uint8_t* d = (uint8_t*)rdp(q, 0x18);
    auto toState3 = [&](void* a) {
        wr8(q, 0x14, 0); wr8(q, 0xb, 1);
        e->fnDFCC0(a, 0);
        wr8(q, 0xc, 3);
        if (e->debugOn) e->fnCBE70(a, "changing state to 3\n");
        wr32(d, 0x58, 0);
    };
    if (rd8(d, 0x47) != 1) { toState3(param1); return; }
    uint32_t key = rd32(d, 0x30);
    uint8_t* game = (uint8_t*)rdp(q, 4);
    if (!game) fatal573(0x2d3);
    for (uint8_t* u = (uint8_t*)rdp(game, 0x1520 + (key & 0x7f) * 4); u; u = (uint8_t*)rdp(u, 0xe4)) {
        if (rd32(u, 0xc) == key) {
            if (rd32(u, 4) == 0x1e) e->setMode(u, 0);
            break;
        }
    }
    toState3((void*)(uintptr_t)key);
    e->fnDFCC0((void*)(uintptr_t)key, 0);
}

// 1.13c D2Game+0x65500  (ECX=quest, EDX=param_2, param_3 unused, param_4=questFlags, param_5=out char)
extern "C" int32_t d2game_65500(uint8_t* q, void* p2, void* p3, uint32_t flags, char* out)
{
    (void)p3;
    D2G573Env* e = d2g573_env();
    *out = 0;
    if (e->questFlag(flags, 3, 1)) { *out = '\n'; return 1; }
    if (e->fn64B60(q)) {
        if (!e->questFlag(flags, 3, 0)) { *out = 2; return 1; }
        return 1;
    }
    if (rd8(q, 9)) {
        if (e->questFlag(flags, 3, 0xd)) {
            int32_t t = e->questFlag(flags, 3, 1);
            uint32_t v = (uint32_t)(t ? -1 : 0) & 0xfffffffdu;
            *out = (char)v + '\r';
            return 1;
        }
        if (e->questFlag(flags, 3, 0xe)) { *out = '\f'; return 1; }
        if (rd8(q, 0xc) < 5) { *out = (char)rd8(q, 0xb); return 1; }
        if (e->fn538D0(3, 0xd)) {
            int32_t r = e->rand10587(p2, 0xc, 0);
            *out = (char)((7 < r) * 8 + 4);
        }
    }
    return 1;
}

// 1.13c D2Game+0x66320  (ECX=ctx: [0]=game,[1]=unit,[2]=a,[5]/[6]=pos)
extern "C" void d2game_66320(int32_t* ctx)
{
    D2G573Env* e = d2g573_env();
    uint8_t* game = (uint8_t*)(uintptr_t)ctx[0];
    void** head = (void**)rdp(game, 0x10f4);
    if (!head || !head[0]) return;
    uint8_t* p = (uint8_t*)head[0];
    while (rd32(p, 0) != 4) { p = (uint8_t*)rdp(p, 0xF4); if (!p) return; }
    uint8_t* d = (uint8_t*)rdp(p, 0x18);
    void* unit = (void*)(uintptr_t)ctx[1];
    wr32(d, 0x6c, unit ? rd32(unit, 0xc) : 0xffffffffu);
    wr8(d, 0x70, 1);
    wr32(d, 0x84, ctx[5]);
    wr32(d, 0x88, ctx[6]);
    if (rd8(d, 0x52) == 1 && rd8(d, 0x51) == 0) e->fn660D0(game, (void*)(uintptr_t)ctx[2]);
}

// 1.13c D2Game+0x663A0  (ECX=game; unaff_BP/EBX/ESI REG? passed through to 0xE1D90)
extern "C" void d2game_663a0(uint8_t* game, uint8_t regBP, uint32_t regBX, void* regSI)
{
    D2G573Env* e = d2g573_env();
    void** head = (void**)rdp(game, 0x10f4);
    if (!head || !head[0]) return;
    uint8_t* p = (uint8_t*)head[0];
    while (rd32(p, 0) != 4) { p = (uint8_t*)rdp(p, 0xF4); if (!p) return; }
    uint8_t* d = (uint8_t*)rdp(p, 0x18);
    wr8(d, 0x91, 1);
    wr8(d, 0x52, 1);
    if (rd8(d, 0x70) != 1) return;
    uint32_t id = rd32(d, 0x6c);
    uint8_t* u = (uint8_t*)rdp(game, 0x1520 + (id & 0x7f) * 4);
    while (u && rd32(u, 0xc) != id) u = (uint8_t*)rdp(u, 0xe4);
    if (!u) return;
    // ctx on stack: [0]=game, [2]=local_1c(game), [3]=unit-room, [4]=fn1100, [5]=fn1130
    int32_t local28[2]; int32_t st[7] = {};
    local28[0] = (int32_t)(uintptr_t)game;
    st[0] = (int32_t)(uintptr_t)game;           // *param_1 = game (iStack_20 frame base)
    st[1] = (int32_t)(uintptr_t)u;              // [1] = unit
    st[3] = (int32_t)(uintptr_t)e->getUnitPtr(u);
    st[4] = (int32_t)(uintptr_t)e->fn1100(u);   // ctx[5] = pos x
    st[5] = (int32_t)(uintptr_t)e->fn1130(u);   // ctx[6] = pos y
    // retail frame: FUN_6fc86320(&iStack_20) reads [0]=game [1]=unit [2]=? [5],[6]=pos
    int32_t frame[7] = { st[0], st[1], (int32_t)0, 0, 0, st[4], st[5] }; // [2] REG? left 0 as in uninit stack slot
    e->fn66320(frame);
    if (rd8(d, 0x51) != 1) return;
    uint32_t id2 = rd32(d, 0x68);
    uint8_t* m = (uint8_t*)rdp(game, 0x1320 + (id2 & 0x7f) * 4);
    while (m && rd32(m, 0xc) != id2) m = (uint8_t*)rdp(m, 0xe4);
    if (!m) return;
    e->ord10141(m, local28);
    e->getUnitPtr(m);
    if (e->fn1280()) {
        uint8_t* n = (uint8_t*)e->fnE1D90(2, 0xbd, regBP, regBX, regSI);
        if (n) { wr8(d, 0x96, 1); wr32(d, 0xa4, rd32(n, 0xc)); }
    }
}

// 1.13c D2Game+0x664F0  (ECX=ctx: [0]=game,[1]=unit,[2]=a,[4]=value). REG? uStack_18 is an uninitialised
// stack slot in the decompile (feeds #10174 / #10003); passed as the player's quest ptr from #10920.
extern "C" int32_t d2game_664f0(int32_t* ctx)
{
    D2G573Env* e = d2g573_env();
    uint8_t* game = (uint8_t*)(uintptr_t)ctx[0];
    void** head = (void**)rdp(game, 0x10f4);
    uint8_t* p = nullptr;
    if (head) for (uint8_t* c = (uint8_t*)head[0]; c; c = (uint8_t*)rdp(c, 0xF4))
        if (rd32(c, 0) == 4) { p = c; break; }
    if (!p) fatal573(0x761);
    uint8_t* d = (uint8_t*)rdp(p, 0x18);
    if (rd8(d, 0x4a) == 0) { wr8(d, 0x4a, 1); e->fn833C0(p); }
    uint8_t* pd = (uint8_t*)e->getPlayerData((void*)(uintptr_t)ctx[2]);
    uint32_t qp = rd32(pd, 0x10 + rd8(game, 0x6d) * 4);
    if (e->questFlag(qp, 4, 0) == 1 || e->questFlag(qp, 4, 1) == 1) { e->fnFFFD0(0x13, (void*)(uintptr_t)ctx[2]); return 0; }
    uint32_t uStack18 = qp;   // REG? uninitialised slot; see header comment
    if (e->fn2E660(0x20646b62) == 0) {
        if ((rd16(d, 0x2c) & 0x3f) == 0 && e->questFlag(uStack18, 4, 3) == 0 && e->questFlag(uStack18, 4, 4) == 0)
            e->fnFFFD0(0x27, (void*)(uintptr_t)ctx[2]);
        wr16(d, 0x2c, rd16(d, 0x2c) + 1);
        return 0;
    }
    if (rd8(p, 9) == 0) return 0;
    if (rd8(p, 0xc) > 5) return 0;
    uint8_t* link = nullptr;
    if (rd8(d, 0x49) == 1) {
        if (!game) fatal573(0x2d3);
        uint32_t k = rd32(d, 0x28);
        link = (uint8_t*)rdp(game, 0x1520 + (k & 0x7f) * 4);
        while (link && rd32(link, 0xc) != k) link = (uint8_t*)rdp(link, 0xe4);
        if (rd8(d, 0x4e) == 0 && link && rd32(link, 4) == 0x3d) {
            if (rd8(p, 0xc) == 0 && rd8(p, 9) == 1) { e->fn835B0(p); e->fn55EB0(); }
            if (e->fn2E660(0x20646b62) == 0) return 0;
        }
    }
    if (rd8(d, 0x4f) == 1) return 0;
    if (rd8(p, 0xc) != 5) {
        if (rd8(p, 0xc) == 0 && rd8(p, 9) == 1) { e->fn835B0(p); e->fn55EB0(); }
        if (e->fn2E660(0x20646b62) == 0) return 0;
        wr8(p, 0xc, 5);
    }
    uint16_t n = rd16(d, 0xc);
    if ((int16_t)ctx[4] != (int16_t)rd16(d, n * 2)) return 0;
    uint8_t* unit = (uint8_t*)(uintptr_t)ctx[1];
    wr32(d, 0x10 + n * 4, unit ? rd32(unit, 0xc) : 0xffffffffu);
    wr16(d, 0xc, n + 1);
    n = rd16(d, 0xc);
    if (unit && rd32(unit, 0x10)) return 0;
    if (n < 5) {
        e->setMode(unit, 1);
        if (link) e->setMode(link, n + 1);
        return 0;
    }
    if (n != 5) return 0;
    e->setMode(unit, 1);
    if (link) e->setMode(link, 6);
    wr8(d, 0x4f, 1);
    e->fn566A0(game);
    wr32(d, 0x7c, rd32(d, 0x7c) - 1);
    int32_t found[3] = {0, 0, 0};
    for (int i = 0; i < 5; i++) {
        if (rd16(d, i * 2) != 0x15) continue;
        if (!game) fatal573(0x2d3);
        uint32_t k = rd32(d, 0x10 + i * 4);
        for (uint8_t* u = (uint8_t*)rdp(game, 0x1520 + (k & 0x7f) * 4); u; u = (uint8_t*)rdp(u, 0xe4))
            if (rd32(u, 0xc) == k) { if (rd32(u, 4) == 0x15) e->ord10141(u, found); break; }
    }
    if (found[0] == 0) {
        int32_t* list = nullptr; uint32_t cnt = 0;
        e->ord10383(e->ord10331b(unit, &list, &cnt));
        for (uint32_t i = 0; i < cnt; i++) {
            uint8_t* c = (uint8_t*)(uintptr_t)list[i];
            for (uint8_t* s = (uint8_t*)rdp(c, 0x74); s; s = (uint8_t*)rdp(s, 0xe8))
                if (rd32(s, 0) == 2 && rd32(s, 4) == 0x15) { e->ord10141(s, found); goto done; }
        }
    done:;
    }
    if (found[0] != 0) {
        void* x = e->fnC0000(game);
        if (x) e->ord10346(e->getUnitPtr(x), nullptr);
    }
    if (rd8(p, 0xb) < 4) {
        wr8(p, 0x14, 0);
        wr8(p, 0xb, 4);
        e->fnDFCC0(e->cb82A80, ctx[2]);
        if (e->fnFFA60() != -1) e->fnBC160(e->cb816f0);
        e->ord10003(uStack18, 4, 4);
    }
    e->fn55E90(1);
    return 0;
}

// 1.13c D2Game+0x67A90  (ECX=param_1 game, EDX=param_2 quest; unaff_EBP REG? = game for 0xA22E0/E1D90)
extern "C" int32_t d2game_67a90(uint8_t* game, uint8_t* q, void* regBP)
{
    D2G573Env* e = d2g573_env();
    uint8_t* d = (uint8_t*)rdp(q, 0x18);
    uint32_t key = rd32(d, 0x40);
    if (!game) fatal573(0x2d3);
    uint8_t* u = (uint8_t*)rdp(game, 0x1520 + (key & 0x7f) * 4);
    while (u && rd32(u, 0xc) != key) u = (uint8_t*)rdp(u, 0xe4);
    if (!u) { wr8(d, 0x44, 0); return 1; }
    uint32_t t = rd32(u, 0);
    uint8_t* mon = (uint8_t*)rdp(u, 0x2c);
    int32_t a = (t == 2 || (t > 3 && t < 6)) ? (int32_t)rd32(mon, 0x10) : (mon ? e->ord10750(mon) : 0);
    char b = (t == 2 || (t > 3 && t < 6)) ? (char)rd32(mon, 0xc) : (mon ? (char)e->ord10867(mon) : 0);
    uint8_t* room = (uint8_t*)e->getUnitPtr(u);
    if (!room) fatal573(0xe42);
    if ((char)e->ord10459(0x26) != (char)e->ord10459(e->ord10826(room))) fatal573(0xe5c);
    if (!e->fn1280()) return 0;
    uint8_t* n = (uint8_t*)e->fnE1D90(2, 0x3c, (uint8_t)(b + 4), (uint32_t)(a + 4), regBP);
    e->setMode(n, 1);
    uint8_t* m = (uint8_t*)e->fnC22E0(regBP, n, (void*)0x26);
    if (!m) return 0;
    e->ord10111(m, e->ord10258() | 3);
    e->ord10346(room, nullptr);
    e->ord10346(e->getUnitPtr(m), nullptr);
    wr8(m, 0x45, 1);
    wr8(m, 0x44, 0);
    return 1;
}

// 1.13c D2Game+0x68150  (ECX=game, EDX=msg; msg stack buf at ESP-0x28 passed to 0x75E10 is REG?/uninit)
extern "C" int32_t d2game_68150(uint8_t* game, uint8_t* msg)
{
    D2G573Env* e = d2g573_env();
    if (!msg || rd32(msg, 4) != 0x92) return 0;
    uint8_t* owner = (rd32(msg, 0) == 1 && rd32(msg, 0x14)) ? (uint8_t*)rdp((uint8_t*)rdp(msg, 0x14), 0x30) : nullptr;
    for (uint8_t* s = (uint8_t*)rdp(owner, 0); s; s = (uint8_t*)rdp(s, 8))
        if (rd32(s, 0) != 0) { e->fn68410(game, msg, e->cb82A30); return 1; }
    e->ord11103(msg, 0);
    uint8_t buf[0x40] = {};     // REG? stack0xffffffd8 contents not initialised in retail
    e->fn95E10(game, buf);
    void** head = (void**)rdp(game, 0x10f4);
    if (head && head[0]) {
        uint8_t* p = (uint8_t*)head[0];
        while (rd32(p, 0) != 4) { p = (uint8_t*)rdp(p, 0xF4); if (!p) return 1; }
        wr8((uint8_t*)rdp(p, 0x18), 0x61, 1);
    }
    return 1;
}

// 1.13c D2Game+0x687D0  (ECX=quest) a1q4 quest-record init
extern "C" void d2game_687d0(uint8_t* q)
{
    D2G573Env* e = d2g573_env();
    std::memset(q + 0xa0, 0, 0x3c);
    // cbLab slots: 0 6860,1 884b0,2 83570,3 85440,4 818d0,5 854a0,6 85a70,7 81750,8 86ce0,9 81880,
    // 10 835d0,11 81540,12 846d0,13 &DAT_6fd293c8
    wr32(q, 0xa0, (uint32_t)(uintptr_t)e->cbLab[0]);
    wr32(q, 0xac, (uint32_t)(uintptr_t)e->cbLab[1]);
    wr32(q, 0xb0, (uint32_t)(uintptr_t)e->cbLab[2]);
    wr32(q, 0xb8, (uint32_t)(uintptr_t)e->cbLab[3]);
    wr32(q, 0xd8, (uint32_t)(uintptr_t)e->cbLab[4]);
    wr32(q, 0xc4, (uint32_t)(uintptr_t)e->cbLab[5]);
    wr32(q, 0xc0, (uint32_t)(uintptr_t)e->cbLab[6]);
    wr32(q, 0xdc, (uint32_t)(uintptr_t)e->cbLab[13]);
    wr32(q, 0xe0, 4);
    wr8(q, 0xb, 0); wr8(q, 10, 1);
    wr32(q, 0xe8, 0);
    wr8(q, 0xd, 6);
    wr32(q, 0xec, (uint32_t)(uintptr_t)e->cbLab[7]);
    wr32(q, 0xcc, (uint32_t)(uintptr_t)e->cbLab[8]);
    wr32(q, 200, (uint32_t)(uintptr_t)e->cbLab[9]);
    wr32(q, 0xa8, (uint32_t)(uintptr_t)e->cbLab[10]);
    wr32(q, 0xd4, (uint32_t)(uintptr_t)e->cbLab[11]);
    wr32(q, 0xf0, (uint32_t)(uintptr_t)e->cbLab[12]);
    wr32(q, 0x10, 3);
    uint8_t* d = (uint8_t*)e->alloc10045("..\\Source\\D2Game/Quests/a1q4.cpp", 0x95f, 0);
    wr32(q, 0x18, (uint32_t)(uintptr_t)d);
    std::memset(d, 0, 0x6f * 4);   // remaining field stores in retail are zero over the zeroed block
}
