// src/core/d2game/d2game_batch_bbf30.cpp -- quest-record and unit-state helpers of the 1.13c D2Game.dll,
// transcribed from the cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000.
//
// Style as d2game_batch_bd880.cpp: register-only inputs (in_EAX, unaff_EBX) are explicit parameters, guest
// 32-bit addresses are uint32_t and are dereferenced through env->at(), and callees we do not own are seams
// named by their RVA (sub<RVA>) or by their D2Common ordinal (o<ordinal>), in the decompile's argument order.
// A quest record is a guest struct: +0x0 type id, +0x9/+0xa/+0xb/+0xc.. flag bytes, +0x18 data pointer,
// +0x3c.. state bytes, +0xf4 next record. A unit's quest-slot array is at +0x10 (dword per index) of the
// structure Ordinal_10920 returns.
#include <cstdint>
#include <cstring>

struct D2GameBatchBbf30Env {
    void*    (*at)(uint32_t guest);                                   // guest address -> host pointer
    // D2Common ordinals (argument order as the decompile calls them)
    uint32_t (*o10920)(uint32_t unit);                                // Ordinal_10920: unit -> quest-slot block
    int32_t  (*o10174)(uint32_t unit, int32_t set, int32_t bit);      // Ordinal_10174: test a quest bit
    void     (*o10003)(uint32_t unit, int32_t set, int32_t bit);      // Ordinal_10003: set a quest bit
    void     (*o10495)(uint32_t unit, int32_t set);                   // Ordinal_10495
    uint32_t (*o10331)(uint32_t unit);                                // Ordinal_10331: unit -> target unit
    int32_t  (*o10826)(uint32_t x);                                   // Ordinal_10826: unit -> class id
    uint8_t  (*o10459)(uint32_t x);                                   // Ordinal_10459: class predicate
    uint32_t (*o10816)(uint32_t unit, int32_t a, int32_t b, uint32_t* o1, uint32_t* o2, int32_t c);
    uint32_t (*o11049)(uint32_t x);                                   // Ordinal_11049
    void     (*o11090)(uint32_t unit, int32_t v);                     // Ordinal_11090: unit state
    uint32_t (*o10688)(uint32_t item);                                // Ordinal_10688: item -> record
    int32_t  (*o10494)();                                             // Ordinal_10494 (no arguments in the decompile)
    int32_t  (*o10590)(int32_t i);                                    // Ordinal_10590
    uint32_t (*o10871)();                                             // Ordinal_10871
    void     (*o11111)();                                             // Ordinal_11111
    void     (*o11108)(uint32_t v);                                   // Ordinal_11108
    int32_t  (*dataTableCount)();                                     // *sgptDataTables_exref +0xc4
    // D2Game internals (RVA in the name)
    void     (*sub8EA80)(uint32_t game, uint32_t unit, int32_t a, uint32_t b, int32_t c, int32_t d, int32_t e2);
    void     (*subBB3B0)(uint32_t game);                              // 0xBB3B0
    void     (*sub33AF0)(uint32_t rec);                               // 0x33AF0
    void     (*sub48300)();                                           // 0x48300
    void     (*sub34C00)();                                           // 0x34C00
    void     (*sub9C160)(uint32_t cb);                                // 0x9C160: iterate with a callback
    int16_t  (*subDFA60)();                                           // 0xDFA60
    int32_t  (*subE660)(uint32_t hash);                               // 0xE660: string-hash probe
    int32_t  (*sub70D60)(uint32_t unit, uint32_t owner);             // 0x70D60
    void     (*sub365E0)(uint32_t unit, uint32_t owner);             // 0x365E0
    uint32_t (*subE03A0)(uint32_t game, uint32_t value);             // 0xE03A0
    void     (*subDFFD0)(int32_t a, uint32_t b);                     // 0xDFFD0
    void     (*subDFAE0)(int32_t a, int32_t b);                      // 0xDFAE0
    void     (*sub8AB20)();                                           // 0x8AB20
    int32_t  (*sub37680)();                                           // 0x37680
    void     (*sub4CB80)();                                           // 0x4CB80
};

static inline uint32_t R32(const D2GameBatchBbf30Env* e, uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (uint8_t*)e->at(a) + o, 4); return v; }
static inline uint8_t  R8 (const D2GameBatchBbf30Env* e, uint32_t a, uint32_t o) { return ((uint8_t*)e->at(a))[o]; }
static inline void     W32(const D2GameBatchBbf30Env* e, uint32_t a, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)e->at(a) + o, &v, 4); }
static inline void     W8 (const D2GameBatchBbf30Env* e, uint32_t a, uint32_t o, uint8_t v) { ((uint8_t*)e->at(a))[o] = v; }

// 1.13c D2Game+0xBBF30 -- __fastcall(p1): p1 = {game, unit, .., item at +0x10}. Sets the quest-17 pickup bit
// for the unit's id and, once all five bits are set on a record of type 0x17 with state 3, advances it.
uint32_t Quest17PickupBitsAndAdvance(const D2GameBatchBbf30Env* e, uint32_t p1) {
    uint32_t game = R32(e, p1, 0), unit = R32(e, p1, 4);
    if (unit != 0 && R32(e, unit, 0x10) != 0) return 0;
    e->o11090(unit, 1);
    uint32_t item = e->o10688(R32(e, p1, 0x10));
    e->sub8EA80(game, unit, 1, (R32(e, item, 0xdc) >> 7) + R32(e, game, 0xa8), 0, 0, 0);
    uint32_t head = R32(e, game, 0x10f4);
    if (head == 0) return 0;
    uint32_t rec = R32(e, head, 0);
    if (rec == 0) return 0;
    while (R32(e, rec, 0) != 0x17) {
        rec = R32(e, rec, 0xf4);
        if (rec == 0) return 0;
    }
    uint32_t data = R32(e, rec, 0x18);
    uint32_t id = (unit == 0) ? 0xffffffffu : R32(e, unit, 4);
    switch (id) {
        case 0x188: W8(e, data, 0xc, 1); break;
        case 0x189: W8(e, data, 0xd, 1); break;
        case 0x18a: W8(e, data, 0xe, 1); break;
        case 0x18b: W8(e, data, 0xf, 1); break;
        case 0x18c: W8(e, data, 0x10, 1); break;
        default: break;
    }
    for (int i = 0; i < 5; ++i)
        if (R8(e, data, 0xc + i) == 0) return 0;
    if (R32(e, data, 0x48) == 3) {
        e->subBB3B0(game);
        if (R8(e, data, 1) == 0) {
            W32(e, data, 0x18, 0);
            W8(e, data, 3, 0);
            W8(e, data, 2, 1);
            W8(e, data, 1, 1);
            e->sub33AF0(rec);
        }
    }
    return 0;
}

// 1.13c D2Game+0xBC920 -- no arguments. For each data-table entry that is active, notifies the unit and
// returns 1 when at least one notification was sent.
uint32_t NotifyAnyActiveSlot(const D2GameBatchBbf30Env* e) {
    uint32_t r = 0;
    int32_t count = e->dataTableCount();
    for (int32_t i = 0; i < count; ++i) {
        if (e->o10494() != 0 && e->o10590(i) != 0) {
            uint32_t x = e->o10871();
            if (x != 0) {
                e->o11111();
                e->o11108(x);
                r = 1;
            }
        }
    }
    return r;
}

// 1.13c D2Game+0xBCB90 -- EAX = a record pointer (may be null); the record gets the reset values, then
// the timer routine runs. The decompile has the same call on both branches of its EAX test, so the
// test is not reproduced; the EAX read has no effect of its own.
void ResetTimerRecord(const D2GameBatchBbf30Env* e, uint32_t rec, uint32_t inEAX) {
    (void)inEAX;
    W32(e, rec, 100, 0xffffffffu);
    W32(e, rec, 0x68, 6);
    W8(e, rec, 0x6c, 0);
    e->sub48300();
}

// 1.13c D2Game+0xC1B20 -- EAX = game. Returns 1 when the first type-0xd quest record is active and its flag
// at +0xc is set; 0 otherwise (the first 0xd record decides either way).
uint32_t QuestIdx13ActiveFlag(const D2GameBatchBbf30Env* e, uint32_t game) {
    uint32_t head = R32(e, game, 0x10f4);
    if (head == 0) return 0;
    for (uint32_t rec = R32(e, head, 0); rec != 0; rec = R32(e, rec, 0xf4)) {
        if (R32(e, rec, 0) == 0xd) {
            if (R8(e, rec, 9) != 0 && R8(e, rec, 0xc) == 1) return 1;
            break;
        }
    }
    return 0;
}

// 1.13c D2Game+0xC1C20 -- param_1 = quest record, param_2 = packet id, param_4 = unit. Gates a packet on the
// unit's quest-14 slots: returns 1 when the packet may be sent.
uint32_t QuestIdx14PacketGate(const D2GameBatchBbf30Env* e, uint32_t rec, int32_t pkt, uint32_t unit) {
    bool ok;
    if (pkt == 0xc9) {
        if (e->o10174(unit, 0xe, 0) == 1) return 0;
        if (R8(e, rec, 0xc) == 1) {
            if (e->o10174(unit, 0xe, 3) == 0 && e->o10174(unit, 0xe, 4) == 0) return 1;
        }
        ok = e->o10174(unit, 0xe, 3) == 1;
    } else if (pkt == 0xd2) {
        ok = e->o10174(unit, 0xe, 4) == 1;
    } else if (pkt == 0xfb) {
        uint32_t data = R32(e, rec, 0x18);
        if (R8(e, rec, 9) != 1 || R8(e, data, 1) != 1) return 0;
        ok = R8(e, data, 0xf) == 0;
    } else {
        if (e->o10174(unit, 0xe, 0xd) == 0) return 0;
        uint32_t slot;
        if (pkt == 0xb0) slot = 6;
        else if (pkt == 0xaf) slot = 7;
        else if (pkt == 0xb1) slot = 8;
        else if (pkt == 0xca) slot = 9;
        else if (pkt == 0xf4) slot = 10;
        else if (pkt == 0xb2) slot = 0xb;
        else return 0;
        ok = e->o10174(unit, 0xe, (int32_t)slot) == 0;
    }
    return ok ? 1 : 0;
}

// 1.13c D2Game+0xC2250 -- param_1 = quest record (+0xe0 set id, +0xc flag), param_2 = packet id, param_4 = unit.
// Returns 1 when the unit's set bit 1 is set for one of the listed packet ids (or 0xb1 with the flag).
uint32_t QuestSlotIdSetGate(const D2GameBatchBbf30Env* e, uint32_t rec, int32_t pkt, uint32_t unit) {
    uint32_t set = R32(e, rec, 0xe0);
    if (e->o10174(unit, (int32_t)set, 0) == 1) return 0;
    int32_t bit1 = e->o10174(unit, (int32_t)set, 1);
    bool listed = pkt == 0xb0 || pkt == 0xaf || pkt == 0xc6 || pkt == 0xc7 || pkt == 0xb1 ||
                  pkt == 0xca || pkt == 0xf4 || pkt == 0xd2 || pkt == 0xc8 || pkt == 0xc9;
    if ((bit1 == 1 && listed) || (pkt == 0xb1 && R8(e, rec, 0xc) == 1)) return 1;
    return 0;
}

// 1.13c D2Game+0xC55B0 -- param_1 = owner (quest-slot index at +0x6d), param_2 = unit. When the unit's class
// is 1 and the quest-9 bits are not yet set, marks them and runs the completion routine.
void Quest9CompleteOnUnit(const D2GameBatchBbf30Env* e, uint32_t owner, uint32_t unit) {
    uint32_t q = R32(e, e->o10920(unit), 0x10 + (uint32_t)R8(e, owner, 0x6d) * 4);
    if (e->o10174(q, 9, 0) == 1) return;
    if (e->o10174(q, 9, 1) == 1) return;
    uint32_t target = e->o10331(unit);
    if (target == 0) return;
    int32_t cls = e->o10826(target);
    if (cls == 0) return;
    if (e->o10459((uint32_t)cls) == 1) {
        e->o10003(q, 9, 0xd);
        e->o10003(q, 9, 1);
        e->o10003(q, 9, 5);
        e->sub34C00();
    }
}

// 1.13c D2Game+0xC5820 -- param_1 = owner, param_2 = unit. Class 0x49 unit: when quest-14 bits 0, 3, 4 and 5
// are clear, sets bit 5 and, if the lookup succeeds, schedules the slot-5 callback (0xC29E0).
void Quest14StartOnClass0x49(const D2GameBatchBbf30Env* e, uint32_t owner, uint32_t unit) {
    uint32_t q = R32(e, e->o10920(unit), 0x10 + (uint32_t)R8(e, owner, 0x6d) * 4);
    uint32_t target = e->o10331(unit);
    if (target == 0) return;
    if (e->o10826(target) != 0x49) return;
    if (e->o10174(q, 0xe, 0) != 0) return;
    if (e->o10174(q, 0xe, 3) != 0) return;
    if (e->o10174(q, 0xe, 4) != 0) return;
    if (e->o10174(q, 0xe, 5) != 0) return;
    e->o10003(q, 0xe, 5);
    if (e->subDFA60() != -1) e->sub9C160(0x6FCE29E0u);
}

// 1.13c D2Game+0xC5C60 -- param_1 = owner, param_2 = unit. Class 0x4a unit: when quest-12 bits 0 and 1 are
// clear, resets the quest-12 block and schedules the callback at 0xC3480.
void Quest12CompleteOnClass0x4A(const D2GameBatchBbf30Env* e, uint32_t owner, uint32_t unit) {
    uint32_t q = R32(e, e->o10920(unit), 0x10 + (uint32_t)R8(e, owner, 0x6d) * 4);
    if (e->o10174(q, 0xc, 0) == 1) return;
    if (e->o10174(q, 0xc, 1) == 1) return;
    uint32_t target = e->o10331(unit);
    if (target == 0) return;
    if (e->o10826(target) != 0x4a) return;
    e->o10003(q, 0xc, 0xd);
    e->o10003(q, 0xc, 1);
    e->o10003(q, 0xc, 0);
    e->o10495(q, 0xc);
    e->o10003(q, 0xc, 8);
    e->o10003(q, 0xc, 7);
    if (e->subDFA60() != -1) e->sub9C160(0x6FCE3480u);
}

// 1.13c D2Game+0xC7FB0 -- EAX = game, EBX = out block (three dwords written on success). Finds the first
// type-0xd quest record; it must be active (+0x3c == 1). Asks the unit at game+0xc0 for a match and writes
// the result to out[0..1] and to the unit's first dword.
uint32_t QuestListFindType0D(const D2GameBatchBbf30Env* e, uint32_t game, uint32_t ebxOut) {
    uint32_t head = R32(e, game, 0x10f4);
    if (head == 0) return 0;
    for (uint32_t rec = R32(e, head, 0); rec != 0; rec = R32(e, rec, 0xf4)) {
        if (R32(e, rec, 0) != 0xd) continue;
        uint32_t data = R32(e, rec, 0x18);
        if (R8(e, data, 0x3c) != 1) return 0;
        uint32_t unit = R32(e, game, 0xc0);
        uint32_t l1 = 0, l2 = 0;
        uint32_t m = e->o10816(unit, 0x28, 0xc, &l1, &l2, 3);
        if (m == 0) return 0;
        e->sub4CB80();
        uint32_t v = e->o11049(m);
        if (v == 0) return 0;
        W32(e, ebxOut, 0, 3);
        W32(e, ebxOut, 4, 0xbe11);
        W32(e, unit, 0, v);
        return 1;
    }
    return 0;
}

// 1.13c D2Game+0xC29E0 -- param_1 = owner, param_2 = unit. Runs only when the owner has a type-0xd record.
// When the unit's quest-14 bits 0, 3, 4 and 5 are clear and the unit's class predicate holds, sets bit 5.
void QuestIdx14SetSlot5OnUnit(const D2GameBatchBbf30Env* e, uint32_t owner, uint32_t unit) {
    uint32_t head = R32(e, owner, 0x10f4);
    if (head == 0) return;
    uint32_t rec = R32(e, head, 0);
    if (rec == 0) return;
    while (R32(e, rec, 0) != 0xd) {
        rec = R32(e, rec, 0xf4);
        if (rec == 0) return;
    }
    uint32_t q = R32(e, e->o10920(unit), 0x10 + (uint32_t)R8(e, owner, 0x6d) * 4);
    if (e->o10174(q, 0xe, 0) == 1) return;
    if (e->o10174(q, 0xe, 3) == 1) return;
    if (e->o10174(q, 0xe, 4) == 1) return;
    if (e->o10174(q, 0xe, 5) == 1) return;
    uint32_t target = e->o10331(unit);
    if (target == 0) return;
    uint32_t cls = (uint32_t)e->o10826(target);
    if (e->o10459(cls) == 1) e->o10003(q, 0xe, 5);
}

// 1.13c D2Game+0xC6240 -- param_1 = {game, unit, timer record}. Quest-13 timer tick: the first type-0xd record
// runs the timer; a timer expiring against the same unit is cleared and reported (returns 0). Returns 1 when
// the routine is still running.
uint32_t Quest13TimerTick(const D2GameBatchBbf30Env* e, uint32_t game, uint32_t unit, uint32_t timer) {
    uint32_t head = R32(e, game, 0x10f4);
    if (head == 0) return 0;
    for (uint32_t rec = R32(e, head, 0); rec != 0; rec = R32(e, rec, 0xf4)) {
        if (R32(e, rec, 0) != 0xd) continue;
        uint32_t r4 = 0;
        if (R8(e, timer, 0x6c) != 0) r4 = e->subE03A0(game, R32(e, timer, 100));
        if (timer != 0 && R32(e, timer, 0) == 0) e->o10920(timer);
        uint32_t lv = (unit == 0) ? 0 : R32(e, unit, 0x10);
        if (lv != 0) {
            if (lv == 1 && r4 == unit) {
                W32(e, timer, 100, 0xffffffffu);
                W32(e, timer, 0x68, 6);
                W8(e, timer, 0x6c, 0);
                e->o11090(unit, 2);
                return 0;
            }
            return 0;
        }
        if (e->sub37680() != 1) {
            if (e->subE660(0x20747368u) != 0) {
                int32_t lvl = (unit == 0) ? -1 : (int32_t)R32(e, unit, 0xc);
                e->subDFAE0(2, lvl);
                e->o11090(unit, 1);
                e->sub8AB20();
                return 0;
            }
            e->subDFFD0(0x13, timer);
        }
        return 1;
    }
    return 0;
}

// 1.13c D2Game+0xC7D00 -- param_1 = owner, param_2 = unit. Class-predicate route: clears quest-10 bit 0, sets
// bit 0xd, then runs the three string-keyed handlers for the unit when the probe at 0x70D60 reports none.
void ItemTagSlotCheck(const D2GameBatchBbf30Env* e, uint32_t owner, uint32_t unit) {
    uint32_t q = R32(e, e->o10920(unit), 0x10 + (uint32_t)R8(e, owner, 0x6d) * 4);
    if (e->o10174(q, 10, 0) == 1) return;
    uint32_t target = e->o10331(unit);
    if (target == 0) return;
    uint32_t cls = (uint32_t)e->o10826(target);
    if (e->o10459(cls) != 1) return;
    e->o10003(q, 10, 0);
    e->o10003(q, 10, 0xd);
    if (e->sub70D60(unit, owner) != 0) return;
    if (e->subE660(0x20747368u) != 0) e->sub365E0(unit, owner);
    if (e->subE660(0x20706976u) != 0) e->sub365E0(unit, owner);
    if (e->subE660(0x2066736du) != 0) e->sub365E0(unit, owner);
}
