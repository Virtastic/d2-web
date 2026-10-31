// src/core/d2game/d2game_batch_d62e0.cpp -- 1.13c D2Game.dll item-mode functions transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000.
//
// Style as d2game_batch_bd880.cpp: register-only inputs (in_EAX / unaff_EDI / unaff_EBP) and stack locals the
// decompile does not tie to a parameter are explicit parameters. Guest addresses are uint32_t and go through
// env->at(). Callees we do not own are seams in retail argument order; the decompile's own variable roles are
// kept (param_1, param_2 ...) where the source meaning is not pinned.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GameBatchD62E0Env {
    void*    (*at)(uint32_t guest);                                  // guest address -> host pointer
    void     (*fatal)(uint32_t line);                                // Ordinal_10265(line); Ordinal_10024; _exit(-1)
    uint32_t fileItemMode;                                           // guest address of "..\\Source\\D2Game\\ITEMS\\ItemMode.cpp"
    // ordinals and D2Game internals named by their decompile address
    int32_t  (*o11017)(uint32_t inv);
    int32_t  (*sub57680)();                                          // FUN_6fc57680
    uint8_t  (*o10020)(uint32_t item);
    void     (*sub29F0)(uint32_t self);                              // 0x29F0
    uint32_t (*o10646)(uint32_t inv, uint32_t item);
    void     (*sub46B0)(uint32_t p1, uint32_t zero);                 // 0x46B0
    int32_t  (*o11057)(uint32_t item, uint32_t self);
    void     (*sub51A0)(uint32_t p1, uint32_t self);                 // 0x51A0
    void     (*o10201)(uint32_t self, int32_t v);
    void     (*sub4630)(uint32_t self);                              // 0x4630
    int32_t  (*o10584)(uint32_t self, uint8_t kind, uint32_t v);
    int32_t  (*sub21130)(uint32_t item);                             // 0x21130
    int32_t  (*sub21100)(uint32_t item);                             // 0x21100
    void     (*o10869)(uint32_t file, uint32_t line, uint32_t item);
    void     (*o11030)(uint32_t item, uint8_t kind);
    void     (*o10012)(uint32_t item, uint32_t v);
    void     (*o10466)(uint32_t file, uint32_t item);
    void     (*o11090)(uint32_t item, uint32_t v);
    void     (*o10977)(uint32_t item, uint32_t flags, uint32_t on);
    void     (*sub91330)(uint32_t file, uint32_t self, uint32_t item); // 0x91330
    void     (*o10892)(uint32_t item, uint32_t flag, uint32_t on);
    int32_t  (*sub1BC0)();                                           // 0x1BC0
    int32_t  (*o10202)(uint32_t item, uint32_t flag, uint32_t line, uint32_t file);
    void     (*o10050)(uint32_t a, uint32_t b);
};

template <typename E> static inline uint32_t R32(const E* e, uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (uint8_t*)e->at(a) + o, 4); return v; }
template <typename E> static inline void     W32(const E* e, uint32_t a, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)e->at(a) + o, &v, 4); }

// 1.13c D2Game+0xD62E0 -- move an item out of a unit's inventory onto the cursor (ItemMode.cpp 0x966/0x979).
// self = unit whose +0x60 holds the inventory (zero type = player), p1 = game-side table base, p2 / p3 = out cells,
// eax = inventory override (0 keeps the unit's own), edi = caller flag (low byte), p5 = bypass the lock check.

extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_D62E0(uint32_t self, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t eax, uint32_t edi,
                            int32_t p5, const D2GameBatchD62E0Env* e)
{
    if (p1 == 0) e->fatal(0x907);
    if (self == 0) e->fatal(0x908);
    W32(e, p3, 0, 0);
    uint32_t inv = R32(e, self, 0x60);
    if (eax) inv = eax;
    if (inv == 0) e->fatal(0x911);
    if (e->o11017(inv) != 0 && p5 == 0) return 0;

    uint32_t item = R32(e, p1, 0x1720 + (p1 & 0x7f) * 4);
    for (;;) {
        if (item == 0) { W32(e, p2, 0, 1); return 0; }
        if (R32(e, item, 0xC) == p1) break;
        item = R32(e, item, 0xE4);
    }
    if (R32(e, item, 0) != 4) return 0;
    if (R32(e, self, 0) == 0 && e->sub57680() == 0 && e->o10020(item) != 0) { W32(e, p2, 0, 1); return 0; }
    if (R32(e, item, 0x10) != 0) { W32(e, p2, 0, 1); return 0; }

    e->sub29F0(self);
    W32(e, item, 0xC4, R32(e, item, 0xC4) & ~0x2u);
    uint32_t moved = e->o10646(inv, item);
    if (moved == 0) e->fatal(0x948);
    if (item != moved) e->fatal(0x949);

    e->sub46B0(p1, 0);
    int32_t iVar7 = e->o11057(item, self);
    if (iVar7 != 0) { e->sub51A0(p1, self); e->o10201(self, 1); }
    e->sub4630(self);
    uint8_t kind = e->o10020(item);
    iVar7 = e->o10584(self, kind, R32(e, p1, 0x70));
    bool notifyClient = !(R32(e, self, 0) != 0 || (edi & 0xff) == 1);
    int32_t iVar5 = e->sub21130(item);
    int32_t iVar8 = e->sub21100(item);
    e->o10869(e->fileItemMode, 0x966, item);
    e->o11030(item, kind);
    e->o10012(item, 0xff);
    if (notifyClient) e->o10466(e->fileItemMode, item);
    e->o11090(item, 4);
    if (iVar8 == 1) {
        if (iVar5 != 0) e->o10977(item, 0x20, 1);
        if (iVar7 == 1) e->sub91330(e->fileItemMode, self, item);
        e->o10892(item, 4, 1);
        if (e->sub1BC0() != 0) e->o10977(item, 1, 1);
        if (e->o10202(item, 0x4000, 0x979, e->fileItemMode) != 0) e->o10977(item, 0x4000, 0);
        e->o10050(item, item);
        e->o10201(self, 1);
    }
    return 1;
}

struct D2GameBatchD6E30Env {
    void*    (*at)(uint32_t guest);
    void     (*fatal)(uint32_t line);
    uint32_t fileItemMode;
    int32_t  (*o11017)(uint32_t inv);
    int32_t  (*sub57680)();
    void     (*sub29F0)(uint32_t unit);
    int32_t  (*sub2F00)(uint32_t retaddr);                           // 0x2F00
    void     (*sub2840)();                                           // 0x2840
    void     (*subFFD0)(uint32_t a, uint32_t unit);                  // 0xFFD0
    int32_t  (*o11088)(uint32_t item);
    void     (*sub6C80)();                                           // 0x6C80
    int32_t  (*sub4360)(uint32_t self);                              // 0x4360
    int32_t  (*sub2610)(uint32_t unit, uint32_t* out, uint32_t z);   // 0x2610
    int32_t  (*o10946)(uint32_t unit, uint32_t ebp, uint32_t item, uint32_t z);
    int32_t  (*o10331)(uint32_t item);
    void     (*o10885)(uint32_t a, uint32_t b, uint32_t c);
    void     (*o10885x1)(uint32_t a);
    uint32_t (*o10331x3)(uint32_t item, uint32_t type, uint32_t id);
    void     (*o10408)(uint32_t item);
    void     (*o11016)(uint32_t item);
    int32_t  (*sub4960)(uint32_t item, uint32_t unit);               // 0x4960
    void     (*sub45A0)();                                           // 0x45A0
    int32_t  (*o10792)(uint32_t item);
    int32_t  (*o10624)(uint32_t self, uint32_t item);
    int32_t  (*o10729)(uint32_t self, uint32_t item);
    int32_t  (*o10405)(uint32_t self, uint32_t item);
    void     (*o10466)(uint32_t self, uint32_t v);
    void     (*sub4670)(uint32_t unit);                              // 0x4670
    int32_t  (*o11057)(uint32_t item, uint32_t unit);
    void     (*sub47C0)();                                           // 0x47C0
    void     (*o11090)(uint32_t item, uint32_t v);
    void     (*o10012)(uint32_t item, uint32_t v);
    void     (*o10892)(uint32_t item, uint32_t flag, uint32_t on);
    void     (*o10050)(uint32_t a, uint32_t b);
    void     (*o10201)(uint32_t unit, int32_t v);
    void     (*sub51A0)(uint32_t a, uint32_t unit);
    int32_t  (*sub6590)(uint32_t unit, uint32_t self, uint32_t one, uint32_t zero, uint32_t file, uint32_t line); // 0x6590
};

// 1.13c D2Game+0xD6E30 -- place an item from the cursor into a unit's inventory slot (ItemMode.cpp 0x77B/0x7F0).
// ecx = self (item-table owner), eax = item id, p1 = game, p2 = unit, p3 = out cell, retaddr / ebp are the
// decompile's unaff_ inputs passed through unchanged, stackOut is the decompile's local at esp+0xC.

extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_D6E30(uint32_t self, uint32_t eax, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t retaddr,
                            uint32_t ebp, uint32_t* stackOut, const D2GameBatchD6E30Env* e)
{
    if (p1 == 0) e->fatal(0x77b);
    if (p2 == 0) e->fatal(0x77c);
    W32(e, p3, 0, 0);
    uint32_t inv = R32(e, p2, 0x60);
    if (inv == 0) e->fatal(0x781);
    if (e->o11017(inv) != 0) return 0;
    if (e->sub57680() != 0) return 0;
    uint32_t item = R32(e, p1, 0x1720 + (eax & 0x7f) * 4);
    for (;;) {
        if (item == 0) { W32(e, inv, 0, 1); return 0; }
        if (R32(e, item, 0xC) == eax) break;
        item = R32(e, item, 0xE4);
    }
    if (R32(e, item, 0) != 4) return 0;
    if (R32(e, item, 0x10) != 3) { W32(e, inv, 0, 1); return 0; }
    e->sub29F0(p2);
    if (e->sub2F00(retaddr) == 0) { e->sub2840(); return 0; }
    e->subFFD0(1, p2);
    if (e->o11088(item) == 4) {
        e->sub6C80();
        return 1;
    }
    if (e->sub4360(self) != 0) return 0;
    if (e->sub2610(p2, stackOut, 0) != 0) {
        if (e->o10946(p2, ebp, item, 0) == 1) {
            uint32_t slot = e->o10331(item);
            if (slot != 0) {
                e->o10885(slot, R32(e, item, 0), R32(e, item, 0xC));
                e->o10408(item);
                e->o11016(item);
            }
            if (e->sub4960(item, p2) == 0) return 0;
            e->sub45A0();
            return 1;
        }
        W32(e, inv, 0, 1);
        return 0;
    }
    if (e->o10792(item) != 0 && e->o10624(self, item) != 0 && e->o10729(self, item) != 0) {
        uint32_t cur = item;
        if (e->o10331(item) != 0) {
            e->o10885x1(e->o10331x3(item, R32(e, item, 0), R32(e, item, 0xC)));  // Ordinal_10331(item, type, id) -> Ordinal_10885(v)
            e->o10408(item);
            e->o11016(item);
        }
        if (e->o10405(self, item) == 0) e->fatal(0x7ee);
        e->o10466(self, 0);
        e->sub4670(p2);
        uint32_t target = self;
        uint32_t pvVar4 = self;
        if (e->o11057(item, p2) != 0) {
            pvVar4 = 2;             // decompile stores the literal 0x2 here (the (void*)0x2 value passed on)
            target = cur;
            e->sub47C0();
        }
        W32(e, item, 0xC4, R32(e, item, 0xC4) & ~0x2u);
        e->o11090(item, 2);
        e->o10012(item, 0xff);
        e->o10892(item, 0x2000, 1);
        W32(e, item, 0xC4, R32(e, item, 0xC4) & 0xFDFFFFFFu);
        e->o10050(target, item);
        e->o10201(p2, 1);
        if (e->o11057(item, p2) != 0) {
            e->sub51A0(pvVar4, p2);
            e->o10201(p2, 1);
        }
        return 1;
    }
    if (e->sub6590(p2, self, 1, 0, e->fileItemMode, 0x7f0) == 0) { e->sub2840(); return 0; }
    return 1;
}

struct D2GameBatchD7160Env {
    void*    (*at)(uint32_t guest);
    void     (*fatal)(uint32_t line);
    int32_t  (*o11017)(uint32_t inv);
    int32_t  (*sub57680)();
    void     (*sub29F0)(uint32_t unit);
    int32_t  (*sub2F00)(uint32_t game);                              // 0x2F00
    void     (*sub2840)();
    uint32_t (*o10331)(uint32_t item, uint32_t type, uint32_t id);
    void     (*o10885)(uint32_t v);
    int32_t  (*o11088)(uint32_t item);
    void     (*sub6C80)();
    void     (*subFFD0)(uint32_t a, uint32_t unit);
    void     (*o10408)(uint32_t item);
    void     (*o11016)(uint32_t item);
    void     (*o10466)(uint32_t inv, uint32_t item);
    void     (*o11090)(uint32_t item, uint32_t v);
    void     (*o10892)(uint32_t item, uint32_t flag, uint32_t on);
    void     (*o10012)(uint32_t item, uint32_t v);
    void     (*o10201none)();
    void     (*o10050)(uint32_t inv, uint32_t item);
    void     (*sub545A0)();                                          // 0x45A0 (FUN_6fc545a0)
};

// 1.13c D2Game+0xD7160 -- take an item off a unit's inventory into the cursor, ItemMode 0x559/0x55F.
// ebx = game, the id is the decompile's unaff_retaddr, self = unit (ECX), p1 / p2 are the out cells.

extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_D7160(uint32_t p1, uint32_t p2, uint32_t self, uint32_t game, uint32_t id, const D2GameBatchD7160Env* e)
{
    if (game == 0) e->fatal(0x559);
    if (self == 0) e->fatal(0x55a);
    W32(e, p2, 0, 0);
    uint32_t inv = R32(e, self, 0x60);
    if (inv == 0) e->fatal(0x55f);
    if (e->o11017(inv) != 0 || e->sub57680() != 0) return;
    e->sub29F0(self);
    uint32_t item = R32(e, game, 0x1720 + (id & 0x7f) * 4);
    if (item == 0) return;
    while (R32(e, item, 0xC) != id) {
        item = R32(e, item, 0xE4);
        if (item == 0) return;
    }
    if (R32(e, item, 0) != 4) return;
    if (R32(e, item, 0x10) != 3) { W32(e, p1, 0, 1); return; }
    if (e->sub2F00(game) == 0) { e->sub2840(); return; }
    e->o10885(e->o10331(item, R32(e, item, 0), R32(e, item, 0xC)));
    if (e->o11088(item) == 4) {
        e->sub6C80();
        e->subFFD0(1, self);
        return;
    }
    e->o10408(item);
    e->o11016(item);
    W32(e, item, 0xC4, R32(e, item, 0xC4) & ~0x2u);
    e->o10466(inv, item);
    e->o11090(item, 4);
    e->o10892(item, 0x40, 1);
    e->o10012(item, 0xff);
    e->o10201none();
    W32(e, item, 0xC4, R32(e, item, 0xC4) & 0xFDFFFFFFu);
    e->o10050(inv, item);
    e->sub545A0();
    e->subFFD0(1, self);
}

struct D2GameBatchD76A0Env {
    void*    (*at)(uint32_t guest);
    void     (*sub29F0)(uint32_t unit);
    uint8_t  (*o10020)(uint32_t item);
    int32_t  (*o10584)(uint32_t unit, uint8_t kind, uint32_t v);
    int32_t  (*o10133)(uint32_t inv, uint32_t item, uint32_t v, uint32_t z, uint32_t kind);
    int32_t  (*o10405)(uint32_t inv, uint32_t item);
    void     (*sub4670)(uint32_t unit);
    int32_t  (*o11057)(uint32_t item, uint32_t unit);
    void     (*sub47C0)();
    void     (*o10466)(uint32_t inv, uint32_t v);
    void     (*o11090)(uint32_t item, uint32_t v);
    void     (*o10892)(uint32_t item, uint32_t flag, uint32_t on);
    int32_t  (*sub1BC0)();
    void     (*o10977)(uint32_t item, uint32_t flags, uint32_t on);
    int32_t  (*o10202)(uint32_t item, uint32_t flag, uint32_t line, uint32_t file);
    void     (*o10201)(uint32_t unit, int32_t v);
    void     (*o10050)(uint32_t inv, uint32_t item);
    void     (*sub91B00)(uint32_t one, uint32_t unit);                // 0x1B00 (FUN_6fc91b00)
    void     (*sub51A0)(uint32_t inv, uint32_t unit);
    uint32_t fileItemMode;
};

// 1.13c D2Game+0xD76A0 -- move a belt / cursor item between unit and inventory (ItemMode 0x8EF).
// p1 = game, p2 = unit, id = item id, ebx / edi are the decompile's flags, stackInv the stack argument (0 = unit's inv).

extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_D76A0(uint32_t game, uint32_t unit, uint32_t id, uint32_t ebx, uint32_t edi, uint32_t stackInv,
                            const D2GameBatchD76A0Env* e)
{
    if (game == 0 || unit == 0) return 0;
    e->sub29F0(unit);
    uint32_t item = R32(e, game, 0x1720 + (id & 0x7f) * 4);
    for (;;) {
        if (item == 0) return 0;
        if (R32(e, item, 0xC) == id) break;
        item = R32(e, item, 0xE4);
    }
    if (R32(e, item, 0) != 4) return 0;
    if (R32(e, item, 0x10) != 4) return 0;
    uint8_t kind = e->o10020(item);
    uint32_t inv = stackInv;
    if (inv == 0) inv = R32(e, unit, 0x60);
    if (inv == 0) return 0;
    uint32_t v = e->o10584(unit, kind, R32(e, game, 0x70));
    if (e->o10133(inv, item, v, 0, kind) == 0) return 0;
    if (e->o10405(inv, item) == 0) return 0;
    if (e->o10020(item) != 4) e->sub4670(unit);
    W32(e, item, 0xC4, R32(e, item, 0xC4) & ~0x2u);
    if (e->o11057(item, unit) != 0) e->sub47C0();
    // the decompile's "if (iVar7 != 0) Ordinal_10466(inv, 0)" tests a flag set to 0 earlier and is dead.
    e->o11090(item, 0);
    if (edi != 0) {
        e->o10892(item, 2, 1);
        if (e->sub1BC0() != 0) e->o10977(item, 1, 1);
        if (e->o10202(item, 0x4000, 0x8ef, e->fileItemMode) != 0) e->o10977(item, 0x4000, 0);
        e->o10201(unit, 1);
        e->o10050(inv, item);
    }
    if (ebx == 1) e->sub91B00(1, unit);
    if (e->o11057(item, unit) != 0) {
        e->sub51A0(inv, unit);
        e->o10201(unit, 1);
    }
    return 1;
}

struct D2GameBatchD8310Env {
    void*    (*at)(uint32_t guest);
    void     (*fatal)(uint32_t line);
    uint32_t fileItemMode;
    void     (*sub29F0)(uint32_t unit);
    void     (*o10141)(uint32_t unit, uint32_t* out);
    uint32_t (*o10331)(uint32_t unit, uint32_t* out, uint32_t one);
    int32_t  (*sub2E930)(uint32_t v, uint32_t unit);                 // 0x2E930
    void     (*sub2E990)(uint32_t item);                             // 0x2E990
    void     (*o10466)(uint32_t inv, uint32_t v);
    int32_t  (*sub1BC0)();
    void     (*o10977)(uint32_t item, uint32_t flags, uint32_t on);
    int32_t  (*o10202)(uint32_t item, uint32_t flag, uint32_t line, uint32_t file);
    void     (*sub78C0)(uint32_t game, uint32_t unit, uint32_t item, uint32_t edi); // 0x78C0 (FUN_6fcf78c0)
};

struct D2GameBatchD8440Env {
    void*    (*at)(uint32_t guest);
    void     (*fatal)(uint32_t line);
    uint32_t (*o10695)(uint32_t classId);                            // returns the item-type record
    uint32_t (*o10450)(uint32_t v, uint32_t* out2);
    void     (*sub1E10)(uint32_t game);                              // 0x1E10
    void     (*sub39B0)(uint32_t unit);                              // 0x39B0
    uint32_t (*sub31490)(uint32_t game, void* buf);                  // 0x31490
    uint32_t (*sub211D0)(uint64_t* seed);                            // 0x11D0
    void     (*o10887)(uint32_t item, uint32_t kind, uint32_t v, uint32_t z);
    void     (*o10012)(uint32_t item, uint32_t v);
    uint32_t (*sub76A0)(uint32_t game, uint32_t unit, uint32_t id);  // 0x76A0 (ours: D76A0 above)
    void     (*sub8310)(uint32_t game, uint32_t unit, uint32_t eax);  // 0x8310 (D8310 above)
};

// 1.13c D2Game+0xD8310 -- drop a placed item back from the unit's cursor slot (ItemMode 0x834/0x835/0x85F).
// p1 = game, p2 = unit, eax = item id (the caller leaves the id in EAX), edi = the caller's flag, passed on.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_D8310(uint32_t p1, uint32_t p2, uint32_t eax, uint32_t edi, const D2GameBatchD8310Env* e)
{
    if (p1 == 0) e->fatal(0x834);
    if (p2 == 0) e->fatal(0x835);
    e->sub29F0(p2);
    uint32_t item = R32(e, p1, 0x1720 + (eax & 0x7f) * 4);
    if (item == 0) return;
    while (R32(e, item, 0xC) != eax) {
        item = R32(e, item, 0xE4);
        if (item == 0) return;
    }
    if (R32(e, item, 0) != 4 || R32(e, item, 0x10) != 4) return;
    uint32_t out[2] = {0, 0};
    e->o10141(p2, out);
    uint32_t v = e->o10331(p2, out, 1);
    if (e->sub2E930(v, p2) == 0) return;
    e->sub2E990(item);
    e->o10466(R32(e, p2, 0x60), 0);
    if (e->sub1BC0() != 0) e->o10977(item, 1, 1);
    if (e->o10202(item, 0x4000, 0x85f, e->fileItemMode) != 0) e->o10977(item, 0x4000, 0);
    e->sub78C0(p1, p2, item, edi);
}

// 1.13c D2Game+0xD8440 -- place an item-type record into a unit (ItemMode 0x161A..0x162E, 0x1623, 0x1628).
// p1 = game, p2 = unit (out cell), item = in_EAX (the item unit), the record and buffer are the decompile's locals.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_D8440(uint32_t p1, uint32_t p2, uint32_t item, const D2GameBatchD8440Env* e)
{
    if (p1 == 0) e->fatal(0x161a);
    if (p2 == 0) e->fatal(0x161b);
    if (item == 0) e->fatal(0x161c);
    if (R32(e, item, 0) != 4) e->fatal(0x161d);
    uint32_t rec = e->o10695(R32(e, item, 4));
    if (rec == 0) e->fatal(0x1623);
    if (*(uint8_t*)e->at(rec + 0x139) == 0) return 0;
    uint32_t local8[2] = {0, 0};
    if (e->o10450(R32(e, rec, 200), local8) == 0) e->fatal(0x1628);
    if (R32(e, item, 0x10) != 0) return 0;
    e->sub1E10(p1);
    e->sub39B0(p2);
    uint8_t buf[112];
    uint32_t slot = e->sub31490(p1, buf);
    if (slot == 0) e->fatal(0x162e);
    uint32_t v;
    if (*(uint8_t*)e->at(rec + 0x13a) == 0 || *(uint8_t*)e->at(rec + 0x13b) == 0) {
        v = 0;
    } else {
        uint64_t seed = 0;
        v = (uint32_t)e->sub211D0(&seed) + *(uint8_t*)e->at(rec + 0x13a);
    }
    e->o10887(slot, 0x46, v, 0);
    e->o10012(slot, 0);
    uint32_t id = R32(e, slot, 0xC);
    if (e->sub76A0(p1, p2, id) == 0) {
        if (e->sub76A0(p1, p2, id) == 0) e->sub8310(p1, p2, 0);
    }
    return 1;
}
