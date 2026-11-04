// src/core/d2game/d2game_batch_70fb0.cpp -- twelve 1.13c D2Game.dll rows (0x70FB0..0x765E0) transcribed
// from the cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Stage A batch lane:
// implementation only, no spec, no vector.
//
// Marker policy: a row whose every argument is attributed from the comments carries the exact
// `1.13c D2Game+0x<rva>` marker (MAPPED). A row with a register/stack argument the decompile leaves
// unattributed (unaff_*, in_*, zero-arg calls that clearly take registers) is tagged `1.13c-NAMED` and
// every guess is marked `GUESS:`; it stays NAMED.
// Seams are function pointers in retail argument order (D2Common/Fog ordinals in 1.13c numbering).
// Register-only arguments are explicit parameters. Fatal path (Fog #10265 -> #10024 -> _exit(-1)) is
// d2game_fatal. unit+0x60 = inventory (cross-checked: 0x537D0 is called by 0x70FB0 with
// (dest, *(unit+0x60), game=EDI, unit=EBX, 0x2000), so EBX is the unit and EDI the game).
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <emscripten.h>

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void*    rdp (const void* p, uint32_t o) { return (void*)(uintptr_t)rd32(p, o); }
static inline void     wr8 (void* p, uint32_t o, uint8_t v)  { ((uint8_t*)p)[o] = v; }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void     wrp (void* p, uint32_t o, void* v)    { wr32(p, o, (uint32_t)(uintptr_t)v); }

[[noreturn]] static void d2game_fatal() { _exit(-1); }

// 0x70FB0 and 0x71060 moved to d2game_70fb0.cpp / d2game_71060.cpp (pb9).

// ---- 0x71170 ---------------------------------------------------------------------------------------
struct D2Game71170Env {
    void* (*firstItem)(void* inv);        // #10460
    void* (*unitIsItem)(void* node);      // #11147
    void* (*nextItem)(void* node);        // #10464
    char  (*getUnitType)(void* unit);     // #10020
    int   (*check)(int* p);               // 0xD2F00
};
// 1.13c D2Game+0x71170: unit (EAX) inventory walk; 0 when a type-2 unit passes check(param_1), else 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_71170(void* unit, int* param1, const D2Game71170Env* e) {
    void* n = e->firstItem(rdp(unit, 0x60));
    while (n) {
        void* u = e->unitIsItem(n);
        n = e->nextItem(n);
        if (e->getUnitType(u) == 2 && e->check(param1) == 0) return 0;
    }
    return 1;
}

// ---- 0x711D0 (NAMED) -------------------------------------------------------------------------------
struct D2Game711d0Env {
    void* (*firstItem)(void* inv); void* (*unitIsItem)(void* n); void* (*nextItem)(void* n);
    char  (*getUnitType)(void* u);
    void  (*a10758)(uint32_t* p, int a, int b);   // #10758
    void  (*a11030)(void* u, int a);              // #11030
    void  (*f8c810)();                            // GUESS: register args unattributed
    void  (*fd2d90)(uint32_t stackArg);           // 0xD2D90
};
// 1.13c-NAMED D2Game+0x711D0: for each type-3 node unit in unit(EAX)'s inventory run the reset sequence
// and count it. GUESS: EBX is the caller's running count; retaddr slot is a stack argument.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_711d0(void* unit, int32_t count, uint32_t stackArg, const D2Game711d0Env* e) {
    if (rd32(unit, 0x60) == 0) return 0;
    uint32_t l8 = 0;
    void* n = e->firstItem(rdp(unit, 0x60));
    while (n) {
        void* u = e->unitIsItem(n);
        n = e->nextItem(n);
        if (u && e->getUnitType(u) == 3) {
            l8 = 0;
            e->a10758(&l8, 0x20, 1);
            e->a11030(u, 3);
            e->f8c810();
            e->fd2d90(stackArg);
            count++;
        }
    }
    return count;
}

// ---- 0x717E0 (NAMED) -------------------------------------------------------------------------------
struct D2Game717e0Env {
    void* (*getOwner)(void* u);        // #10331
    int   (*isValid)(void* o);         // #10057
    uint32_t (*getStat)(void* u, int stat, int layer);   // #10973
    int   (*getHp)(void* u);           // #10049
    uint32_t (*getMax)(void* u);       // #11060
    void  (*f32df0)(); void (*f21670)(int delta); void (*fd4570)(); void (*fdffd0)(int a, void* u);
};
// 1.13c-NAMED D2Game+0x717E0: GUESS on every hidden register: EAX=unitId, SI=message (0x12/0x13/0x14),
// the f21670/f32df0/fd4570 arguments are not shown. Looks the unit up in the game hash at +0x1520+(id&0x7f)*4
// (chain next at +0xE4), requires type 0x10b at +4, then dispatches on the message.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_717e0(void* param1, void* game, uint32_t unused, int32_t param4,
                                                        uint32_t unitId, int16_t msg, const D2Game717e0Env* e) {
    (void)unused;
    if (!game) d2game_fatal();                                // line 0x2d3
    void* n = rdp(game, 0x1520 + (unitId & 0x7f) * 4);
    if (!n) return;
    while (rd32(n, 0xc) != unitId) { n = rdp(n, 0xe4); if (!n) return; }
    if (rd32(n, 4) != 0x10b) return;
    void* a = e->getOwner(param1); if (!a || !e->isValid(a)) return;
    void* b = e->getOwner(n);      if (!b || !e->isValid(b)) return;
    if (msg == 0x12) {
        if (rd8(param1, 0x6c) != 0 && rd32(param1, 0x68) == 2) { wr32(param1, 100, 0xffffffff); wr32(param1, 0x68, 6); wr8(param1, 0x6c, 0); }
        e->fd4570();
    } else if (msg == 0x13) {
        if (param4 > 0 && (int32_t)e->getStat(param1, 0xf, 0) >= param4) {
            int32_t cur = (int32_t)e->getStat(param1, 0xe, 0);
            if (cur + param4 <= e->getHp(param1)) { e->f32df0(); e->f21670(-param4); return; }
            e->fdffd0(0x13, param1);
        }
    } else if (msg == 0x14 && param4 > 0) {
        int32_t have = (int32_t)e->getStat(param1, 0xe, 0);
        if (param4 <= have) {
            uint32_t mx = e->getMax(param1);
            uint32_t o = e->getStat(param1, 0xf, 0);
            if (o + param4 <= mx) { e->f21670(-param4); e->f21670(param4); return; }
            if (o < mx) { e->f21670(mx - o); e->f21670(o - 0xe); return; }
        }
    }
}

// ---- 0x734E0 (NAMED) -------------------------------------------------------------------------------
struct D2Game734e0Env { void (*fd4570)(int* unit); void (*f72fc0)(int* p2, int* eax); };   // 0xD4570: EAX = unit (still live from the entry), no stack words; 0x72FC0: stack (p2, unit)
// 1.13c D2Game+0x734E0: EAX = unit, CX = msg, stack p2 (ret 4). msg 0x17 clears a state-4 sub-record, 0x18 forwards.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_734E0(int32_t* eax, int16_t msg, int32_t* p2, const D2Game734e0Env* e) {
    if (msg == 0x17) {
        if ((uint8_t)eax[0x1b] != 0 && eax[0x1a] == 4) { eax[0x19] = -1; eax[0x1a] = 6; *(uint8_t*)(eax + 0x1b) = 0; }
        e->fd4570(eax);
    } else if (msg == 0x18) e->f72fc0(p2, eax);
}

// ---- 0x758D0 / 0x75900 -----------------------------------------------------------------------------
struct D2Game75680Env { void (*f75680)(int* p, void* eax, uint32_t one, const uint32_t* rec); };
// 1.13c D2Game+0x758D0: EAX, ECX and one stack word (ret 4). Builds a 0x20-byte local whose dwords 1 and 2 are EAX and ECX
// (dword 0 and the rest are stack residue) and calls 0x75680(param1, EAX, 1, &local), which pops the four words.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_758D0(int* param1, void* eax, void* ecx, const D2Game75680Env* e) {
    uint32_t local[8] = {};
    local[1] = (uint32_t)(uintptr_t)eax; local[2] = (uint32_t)(uintptr_t)ecx;
    e->f75680(param1, eax, 1, local);
}


// ---- 0x75A40 (NAMED, last) / 0x75C60 ---------------------------------------------------------------

struct D2Game75c60Env {
    int (*a10362)(); void* (*a10237)(uint8_t); uint32_t (*a10973)();
    int32_t (*spawn)(void* game, int, int32_t* unit, int, int, int, int, int, int, int);
    void (*f75a40)(void* game, uint32_t tag, int32_t* unit);
};
// 1.13c-NAMED D2Game+0x75C60: EAX=unit, EBX=game. GUESS: a10362/a10973 register args hidden.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_75c60(int32_t* unit, void* game, const D2Game75c60Env* e) {
    if (!unit) return 0;
    int k = e->a10362();
    if (k < 0) return 0;
    void* t = e->a10237(rd8(game, 0x6d));
    int lvl = (int)rd32(t, 0x10) + 1;
    if ((unit[0x31] >> 9) & 1) lvl = (int)e->a10973();
    if (e->spawn(game, k, unit, 0, lvl, 0, 0, 0, 0, 1)) e->f75a40(game, (uint32_t)k, unit);
    return 1;
}

// ---- 0x76570 ---------------------------------------------------------------------------------------
struct D2Game76570Env {
    uint32_t (*a11067)(int32_t v); void (*a10422)(uint32_t a, int32_t v); void (*a10158)(int32_t* u);
    void (*f8ea80)(void* game, int32_t* unit, int a, uint32_t b, int c, int d, int e_);
};
// 1.13c D2Game+0x76570: ECX=game, EDX=unit. If the timer at unit+0xA4 has expired (<= game+0xA8) clear it
// and flag 0x100, else reschedule.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_76570(void* game, int32_t* unit, const D2Game76570Env* e) {
    if (!unit || unit[0x29] == 0) return;
    int32_t v = unit[0x29];
    uint32_t t = e->a11067(v);
    if (t <= rd32(game, 0xa8)) {
        e->a10422(rd32(game, 0x1c), v);
        unit[0x29] = 0;
        e->a10158(unit);
        unit[0x31] |= 0x100;
        return;
    }
    e->f8ea80(game, unit, 6, t, 0, 0, 0);
}

// ---- 0x765E0 (NAMED) -------------------------------------------------------------------------------
struct D2Game765e0Env {
    void (*a10498)(uint32_t v); uint32_t (*f1560)(uint32_t ecx, int a);
    const uint8_t* monstats; int32_t monCount;
    int (*a10494)(int32_t* u, int a); void (*a11103)(int32_t* u, int a);
    void (*f75e10)(void* game, void* locals);
    void (*f8ea80)(void* game, int32_t* unit, int a, uint32_t b, int c, int d, int e_);
};
// 1.13c-NAMED D2Game+0x765E0: ECX=game, EDX=unit. GUESS: f1560's ECX and f75e10's stack-local block are
// hidden; class 0x4e units reschedule at +0x2d / +0xf frames, otherwise reset to state 0xa0.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_765e0(void* game, int32_t* unit, uint32_t ecx, void* locals, const D2Game765e0Env* e) {
    e->a10498(unit[0xb]);
    int32_t cls = unit[1];
    if (e->f1560(ecx, 3) == 0) { e->f8ea80(game, unit, 2, rd32(game, 0xa8) + 1, 0, 0, 0); return; }
    if (unit[0] == 1 && cls >= 0) {
        if (cls < e->monCount) cls = rd16(e->monstats + (uint32_t)cls * 0x1a8, 2);
        if (cls >= 0 && cls < e->monCount && cls == 0x4e) {
            uint32_t t = rd32(game, 0xa8) + (e->a10494(unit, 0x15) ? 0x2d : 0xf);
            e->f8ea80(game, unit, 2, t, 0, 0, 0);
            return;
        }
    }
    e->a11103(unit, 0);
    unit[0x2c] = 0xa0;
    e->f75e10(game, locals);
}
