// src/core/d2game/d2game_batch_A0000.cpp -- twelve 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c), in RVA order:
//   0xA0000 0xA0630 0xA06F0 0xA09A0 0xA0E80 0xA1280 0xA1350 0xA1600 0xA2060 0xA20C0 0xA24C0 0xA26E0
// All twelve sit in the SKILLS.cpp region (0xA2060 passes "..\Source\D2Game\SKILLS\Skills.cpp" to the
// debug-line overload of D2Common #10601). Base 0x6FC20000.
//
// Stage A batch lane (OBJECTIVE v9): implementation + marker, no spec, no vector.
//
// Host model: retail module state is opaque and read by retail byte offsets. Register-only arguments
// (in_EAX / unaff_EBX / unaff_ESI / unaff_EDI / unaff_retaddr / extraout_ECX) and caller-frame stack
// words the decompile could not tie to a parameter are explicit parameters, named at each function.
// Unmapped callees are seams on D2GameBatchAEnv in retail argument order (rva in the seam name);
// sgptDataTables and the direction tables at D2Game+0xFB6D4 arrive as host pointers. The D2Common
// ordinals are 1.13c import numbers -- D2MOO's 1.10f comments for the same numbers (e.g. 1.10f
// #10331 UNITS_SetYForStaticUnit) are shifted and do not describe these imports. The fatal path is
// Ordinal_10265(id) -> Ordinal_10024(&moduleName, id) -> _exit(-1); only the exit is observable.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
// Retail addresses arrive as 32-bit integers in the decompile (unaff_EDI, in_EAX, ...).
static inline uint32_t rd32(uint32_t a, uint32_t o) { return rd32((const void*)(uintptr_t)a, o); }
static inline void     wr32(uint32_t a, uint32_t o, uint32_t v) { wr32((void*)(uintptr_t)a, o, v); }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

// Host seams for the callees the cached decompiles name but src/ does not implement. A seam whose
// decompile shows no arguments is taken to have none.
struct D2GameBatchAEnv {
    // sgptDataTables: count at +0xB6C with 0x1A4-byte records at +0xB64; missile count at +0xBA0
    // with 0x23C-byte records at +0xB98.
    const uint8_t* dataTables;
    // D2Game+0xFB6D4: the six 4-byte-strided direction tables 0xA26E0 multiplies.
    const uint8_t* dirTable;

    // 0x9FA30: eax = unit, ebx = pX, ecx = self (passed through), stack = pY. Writes both
    // coordinates and answers whether both are non-zero.
    uint32_t (*fn9FA30)(uint32_t unit, uint32_t* pX, uint32_t* pY, uint32_t self);
    // 0x6F930: ecx = game, edx = request; the missile-from-request creator.
    uint32_t (*fn6F930)(uint32_t game, uint32_t* request);
    int32_t  (*fn1280)();                                     // 0x1280 room/game sweep
    void     (*fnE07A0)(uint32_t game);                       // 0xE07A0 unit update
    uint32_t (*fnDDDE0)(uint32_t amount, uint32_t game, uint32_t target); // 0xDDDE0 damage
    void     (*fn9F9A0)();                                    // 0x9F9A0
    void     (*fn8D5F0)();                                    // 0x8D5F0
    void     (*fnD24B0)();                                    // 0xD24B0
    void     (*fnD51A0)(uint32_t edi, uint32_t ctx);          // 0xD51A0
    int32_t  (*fnE930)(uint32_t a, uint32_t ebx);             // 0xE930
    void     (*fnE5D0)(uint32_t zero);                        // 0xE5D0
    int32_t  (*fn11490)(int32_t param, uint32_t* buf);        // 0x11490
    int32_t  (*fn9EB80)();                                    // 0x9EB80
    void     (*fn79100)(uint32_t param);                      // 0x79100
    void     (*fn78A90)(uint32_t ecx);                        // 0x78A90
    void     (*fn9FF70)(int32_t missileId);                   // 0x9FF70
    void     (*fnA19A0)(uint32_t unit, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e); // 0xA19A0
    uint32_t (*fn9F3B0)(uint32_t unit, uint32_t esi);         // 0x9F3B0
    void     (*fnDED30)(uint32_t game, uint32_t unit, uint32_t target); // 0xDED30
    uint32_t (*fnDFFD0)(uint32_t a, uint32_t game);           // 0xDFFD0
    uint32_t (*fnE1D90)(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e); // 0xE1D90
    uint32_t (*fnA22E0)(uint32_t a, uint32_t b, uint32_t c);  // 0xA22E0

    // D2Common ordinals (1.13c import numbers).
    uint32_t (*ordinal10080)();
    uint32_t (*ordinal10331)(uint32_t unit);                  // 0xA0630: room (0 = none)
    uint32_t (*ordinal10331NoArg)();                          // 0xA06F0, 0xA1350
    uint32_t (*ordinal10331Two)(uint32_t a, uint32_t b);      // 0xA24C0: (item, 0)
    uint32_t (*ordinal10306)();
    int32_t  (*ordinal10044)(uint32_t act, int32_t index);
    int32_t  (*ordinal10052)(int32_t* rect);
    int32_t  (*ordinal10057)(uint32_t arg);
    uint8_t  (*ordinal10111)(uint32_t a, uint32_t value);
    void     (*ordinal10141)();
    void     (*ordinal10201)();
    uint8_t  (*ordinal10258)(uint32_t item);
    void     (*ordinal10344)(int32_t* value);
    uint32_t (*ordinal10346)(uint32_t unit);
    void     (*ordinal10346Two)(uint32_t unit, uint32_t value);
    uint32_t (*ordinal10392)(uint32_t path);                  // 0xA0E80, 0xA20C0: target unit
    uint8_t  (*ordinal10459)(uint32_t value);
    int32_t  (*ordinal10511)(uint32_t unit);                  // 0xA2060: unit in a register
    void     (*ordinal10551)();
    uint32_t (*ordinal10591)(uint32_t a, int32_t* b, int32_t c, uint32_t d);
    uint32_t (*ordinal10601)(uint32_t id, const char* file, uint32_t line);
    int32_t  (*ordinal10654)(uint32_t unit, int32_t* a, int32_t* b, uint32_t c, uint32_t d, uint32_t e, uint32_t f);
    int32_t  (*ordinal10708)(uint32_t flag);
    int32_t  (*ordinal10786)(uint32_t unit);
    int32_t  (*ordinal10814)();                               // 0xA1350: result answers -1
    uint32_t (*ordinal10826)(uint32_t unit);
    void     (*ordinal10920)();
    void     (*ordinal11090)(uint32_t value, uint32_t one);
    int32_t  (*ordinal11120)(uint32_t act);
};

// 0x9FA30 pX/pY are 32-bit retail addresses in the binary; the seam takes host pointers.

// 0xA0000: a missile request is zeroed and handed to 0x6F930 when a unit is present, the caller's
// two coordinate words are non-zero (or 0x9FA30 fills them), and D2Common #10080 answers < 0x65.
// ECX = param_1 (the 0x9FA30 "self" word), ESI = the unit, [esp+0x14]/[esp+0x18] = the two
// coordinate words, ECX at the 0x6F930 call (Ghidra's uStack_8) = the game.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t* d2_fn_d2game_A0000(uint32_t self, uint32_t unit, uint32_t arg14, uint32_t arg18, uint32_t game, const D2GameBatchAEnv* env) {   // 1.13c D2Game+0xA0000
    if (unit != 0 &&
        ((arg14 != 0 || arg18 != 0) || (env->fn9FA30(unit, &arg18, &arg14, self), arg18 != 0)) &&
        env->ordinal10080() < 0x65) {
        uint32_t request[0x17] = {};
        return (uint32_t*)(uintptr_t)env->fn6F930(game, request);
    }
    return nullptr;
}

// 0xA0630: index the 0x1A4-byte record table at sgptDataTables+0xB64 and range-check against the
// count at +0xB6C; with a record present, ask #10331 for the unit's room and, if it has one, run
// 0x9FA30 over the caller's coordinate slots. No record is retail's fatal 0x1281. EAX = the index,
// param_2 = the unit, unaff_retaddr (0x9FA30's ECX = self) is explicit.
extern "C" EMSCRIPTEN_KEEPALIVE bool d2_fn_d2game_A0630(int32_t index, uint32_t unit, uint32_t self, const D2GameBatchAEnv* env) {   // 1.13c D2Game+0xA0630
    const uint8_t* dt = env->dataTables;
    int32_t record = 0;
    if (!(index < 0) && (int32_t)rd32(dt, 0xB6C) > index)
        record = (int32_t)((uint32_t)index * 0x1A4 + rd32(dt, 0xB64));
    const int32_t room = (int32_t)env->ordinal10331(unit);
    if (record != 0) {
        if (room != 0) {
            uint32_t pX = 0, pY = 0;
            env->fn9FA30(unit, &pX, &pY, self);
        }
        return false;
    }
    d2game_fatal(0x1281);   // Ordinal_10265(0x1281) -> Ordinal_10024 -> _exit(-1)
}

// 0xA06F0: param_5 (a count/flag) must be non-zero. 0x9FA30 fills local_68; then a 0x38-byte frame
// (local_38[0] = param_2, [2] = [3] = param_3, local_28 = local_68[0]) is prepared, #10331 and 0x1280
// run, and when 0x1280 answers non-zero #10591/#10052 fill the callback list that the caller's
// handler (pFStack_14, fatal 0x1061 when bad) walks once per element against uStack_18; #10344
// closes it. EAX = the element count/source unit, EBX = the list length, unaff_retaddr = the owner
// record (its +0x1C is passed to #10591), uStack_18 = the handler's second argument.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_A06F0(uint32_t self, uint32_t param2, uint32_t param3, uint32_t param4, int32_t param5, uint32_t unit, int32_t count, uint32_t owner, uint32_t cbArg, void (*handler)(uint32_t, uint32_t), const D2GameBatchAEnv* env) {   // 1.13c D2Game+0xA06F0
    if (param5 == 0) d2game_fatal(0);   // Ordinal_10265() / Ordinal_10024() pair with no code
    (void)param4;
    uint32_t local68[11] = {};
    if (env->fn9FA30(unit, local68, local68 + 8, self) != 0) {
        uint32_t local38[14] = {};
        (void)local38;
        local38[0] = param2;
        local38[2] = param3;
        local38[3] = param3;
        const uint32_t local28 = local68[0];
        (void)local28;
        env->ordinal10331NoArg();
        const int32_t found = env->fn1280();
        if (found != 0) {
            int32_t aiStack88[2] = { (int32_t)unit, 0 };        // [1] = 0 then [0] = in_EAX
            int32_t iStack8c = (int32_t)local68[0];
            env->ordinal10591(rd32(owner, 0x1C), (int32_t*)local68, found, param3);
            env->ordinal10052(aiStack88);
            if (handler == nullptr) d2game_fatal(0x1061);      // IsBadCodePtr collapses to a null test
            for (int32_t i = 0; i < count; ++i)
                handler(rd32((uint32_t)aiStack88[0], (uint32_t)i * 4), cbArg);
            env->ordinal10344(&iStack8c);
        }
    }
}

// 0xA09A0: a valid missile id (in [0, sgptDataTables+0xBA0) with a non-null 0x23C-byte record) and
// D2Common #10786 answering positive run 0x9FF70 on the id. ECX = param_1 (the missile id); the unit
// #10786 reads in a register is the second parameter.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_A09A0(int32_t missileId, uint32_t unit, const D2GameBatchAEnv* env) {   // 1.13c D2Game+0xA09A0
    const uint8_t* dt = env->dataTables;
    if (missileId > -1 && missileId < (int32_t)rd32(dt, 0xBA0) &&
        (int32_t)((uint32_t)missileId * 0x23C + rd32(dt, 0xB98)) != 0) {
        if (env->ordinal10786(unit) > 0) env->fn9FF70(missileId);
    }
}

// 0xA0E80: __fastcall (ECX = game, EDX = unit). Update the game, resolve the unit's target through
// D2Common #10392 (the unit's +0x2C), and when it is a different live unit run 0xDDDE0 with amount
// 0x80 and answer 1. The zeroed 0x1C-dword frame is dead (0xDDDE0 takes no pointer).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_A0E80(uint32_t game, uint32_t unit, const D2GameBatchAEnv* env) {   // 1.13c D2Game+0xA0E80
    env->fnE07A0(game);
    const uint32_t target = env->ordinal10392(rd32(unit, 0x2C));
    if (target != unit && target != 0) {
        uint32_t frame[0x1C] = {};
        (void)frame;
        env->fnDDDE0(0x80, game, target);
        return 1;
    }
    return 0;
}

// 0xA1280: the act tick for one context record. EAX = ctx ({+0 state, +4 act}); when the state is
// zero, walk D2Common #11120's element count: #10044 per element and, when #10708(-1) answers
// non-zero, #10306 + 0x9F9A0, #10920 while the state is still zero (a roll-back), and 0x8D5F0.
// #10551, 0xD24B0, 0xD51A0(EDI, ctx) and #10201 close the tick.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_A1280(int32_t* ctx, uint32_t edi, const D2GameBatchAEnv* env) {   // 1.13c D2Game+0xA1280
    if (ctx == nullptr || *ctx != 0) return;
    const uint32_t act = (uint32_t)ctx[1];
    const int32_t count = env->ordinal11120(act);
    for (int32_t i = 0; i < count; ++i) {
        env->ordinal10044(act, i);
        if (env->ordinal10708(0xFFFFFFFF) != 0) {
            env->ordinal10306();
            env->fn9F9A0();
            if (*ctx == 0) env->ordinal10920();
            env->fn8D5F0();
        }
    }
    env->ordinal10551();
    env->fnD24B0();
    env->fnD51A0(edi, (uint32_t)(uintptr_t)ctx);
    env->ordinal10201();
}

// 0xA1350: while D2Common #10814 is not -1, #10141 and #10331 run, 0xE930(#10331 result, EBX) must
// answer non-zero, and 0xE5D0(0) then 0x11490(param_1, 0x84-byte zeroed frame) produce the result.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_A1350(int32_t param1, uint32_t ebx, const D2GameBatchAEnv* env) {   // 1.13c D2Game+0xA1350
    if (env->ordinal10814() != -1) {
        env->ordinal10141();
        const uint32_t value = env->ordinal10331NoArg();
        if (env->fnE930(value, ebx) != 0) {
            uint32_t frame[0x21] = {};
            env->fnE5D0(0);
            return env->fn11490(param1, frame);
        }
    }
    return 0;
}

// 0xA1600: 0x9EB80 decides. When it answers zero, 0x79100(param_1) runs and 0x78A90 receives the ECX
// observed after that call, then the function answers 0; otherwise it answers 1. ECX = param_1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_A1600(uint32_t param1, uint32_t ecxAfter79100, const D2GameBatchAEnv* env) {   // 1.13c D2Game+0xA1600
    if (env->fn9EB80() == 0) {
        env->fn79100(param1);
        env->fn78A90(ecxAfter79100);
        return 0;
    }
    return 1;
}

// 0xA2060: D2Common #10511's value goes through the file/line debug overload #10601 and then
// 0xA19A0(param_1, itemRecord, #10306(), 1, 0, 0); 0x9F3B0(param_1, ESI) closes it. param_1 = ECX.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_A2060(uint32_t param1, uint32_t esi, const D2GameBatchAEnv* env) {   // 1.13c D2Game+0xA2060
    const uint32_t value = env->ordinal10511(esi);
    if (value != 0) {
        const uint32_t record = env->ordinal10601(value, "..\\Source\\D2Game\\SKILLS\\Skills.cpp", 0xCA3);
        const uint32_t other = env->ordinal10306();
        env->fnA19A0(param1, record, other, 1, 0, 0);
        env->fn9F3B0(param1, esi);
    }
}

// 0xA20C0: set the unit's flag bit 0x40 at +0xC4, update the game (EDI), resolve the unit's target
// from its +0x2C through D2Common #10392, and when it is a different live unit run
// 0xDED30(game, unit, target) and answer 1. ESI = the unit, EDI = the game, both register-only.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_A20C0(uint32_t unit, uint32_t game, const D2GameBatchAEnv* env) {   // 1.13c D2Game+0xA20C0
    if (unit != 0) wr32(unit, 0xC4, rd32(unit, 0xC4) | 0x40);
    env->fnE07A0(game);
    const uint32_t target = env->ordinal10392(rd32(unit, 0x2C));
    if (target != unit && target != 0) {
        env->fnDED30(game, unit, target);
        return 1;
    }
    return 0;
}

// 0xA24C0: the item-create path. param_2 (the item id) is required (fatal 0xE42); *param_6 is
// cleared. EAX (game) non-zero with #10057(param_2) non-zero rejects unless param_5 is one of
// 0x27/0x85/0x86/0x87/0x88 with param_7 == 0x3C: rejected runs 0xDFFD0(0x18, game) and answers 0.
// #10459(param_5) must equal #10459(#10826(param_2)) (fatal 0xE5C). Then param_5 != 0 or
// #10654(param_2, &a, &a, 3, 0x3E01, 0xC01, 0) non-zero, and 0x1280 non-zero: 0xE1D90(2, param_4,
// (byte)EBX, param_1, local_8) yields the item, *param_3 receives it, #11090(item, 1) runs and
// 0xA22E0(local_8, item, EBX) must answer non-zero; the low byte of uStack_4 is replaced by
// #10258(item) and OR'ed with 3 before #10111(item, that), then #10346(found, 0),
// #10331(item, 0) and #10346(value). EAX = game, unaff_retaddr = EBX, iStack_8 = local_8.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_A24C0(uint32_t param1, uint32_t param2, uint32_t* param3, uint32_t param4, int32_t param5, uint32_t* param6, uint32_t param7, uint32_t game, uint32_t ebx, uint32_t local8, uint32_t uStack4, const D2GameBatchAEnv* env) {   // 1.13c D2Game+0xA24C0
    if (param2 == 0) d2game_fatal(0xE42);
    if (param6 != nullptr) *param6 = 0;
    const bool classOk = param5 == 0x27 || param5 == 0x85 || param5 == 0x86 || param5 == 0x87 || param5 == 0x88;
    if (game != 0 && env->ordinal10057(param2) != 0 && (!classOk || param7 != 0x3C)) {
        env->fnDFFD0(0x18, game);
        return 0;
    }
    const uint8_t cVar1 = env->ordinal10459((uint32_t)param5);
    const uint8_t cVar2 = env->ordinal10459(env->ordinal10826(param2));
    if (cVar1 != cVar2) d2game_fatal(0xE5C);
    int32_t tmp = 0;
    int32_t found = 0;
    if ((param5 != 0 || env->ordinal10654(param2, &tmp, &tmp, 3, 0x3E01, 0xC01, 0) != 0) &&
        (found = env->fn1280()) != 0) {
        uint32_t item = env->fnE1D90(2, param4, (uint8_t)ebx, param1, local8);
        if (param3 != nullptr) *param3 = item;
        env->ordinal11090(item, 1);
        item = env->fnA22E0(local8, item, ebx);
        if (item != 0) {
            const uint8_t value = env->ordinal10258(item);
            const uint32_t flags = (uStack4 & 0xFFFFFF00u) | ((uint32_t)value & 0xFFu) | 3u;
            env->ordinal10111(item, flags);
            env->ordinal10346Two((uint32_t)found, 0);
            env->ordinal10346(env->ordinal10331Two(item, 0));
            return 1;
        }
    }
    return 0;
}

// 0xA26E0: a missile fan. param_2 (a count) must satisfy (param_2 - 1) / 5 <= 7 (fatal 0xF0D if
// not); request[7]/[8] start at 0xE / -0xE and 0x6F930(ECX = game, EDX = request) fires. The outer
// loop then walks the tables at D2Game+0xFB6D4 in 4-byte steps: per inner step two missiles with the
// cross products of the +0x88/+0x5C and +0x78/+0x40 tables, and after each 0x10-byte sweep one from
// the +0x20/+0x00 tables. ESI = the request array.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_A26E0(uint32_t game, int32_t param2, uint32_t* request, const D2GameBatchAEnv* env) {   // 1.13c D2Game+0xA26E0
    const uint8_t* t = env->dirTable;   // retail 0x6FD1B6D4: +0x00, +0x20, +0x40, +0x5C, +0x78, +0x88
    param2 -= 1;
    if (7 < param2 / 5) d2game_fatal(0xF0D);
    request[7] = 0xE;
    request[8] = 0xFFFFFFF2u;
    env->fn6F930(game, request);
    int32_t iVar2 = 0;
    for (;;) {
        if (param2 == 0) return;
        int32_t iVar3 = 0;
        do {
            if (param2 < 1) break;
            request[7] = rd32(t, 0x88 + (uint32_t)iVar3) * rd32(t, 0x5C + (uint32_t)iVar2);
            request[8] = rd32(t, 0x78 + (uint32_t)iVar3) * rd32(t, 0x40 + (uint32_t)iVar2);
            env->fn6F930(game, request);
            request[7] = rd32(t, 0x88 + (uint32_t)iVar3) * rd32(t, 0x40 + (uint32_t)iVar2);
            request[8] = rd32(t, 0x78 + (uint32_t)iVar3) * rd32(t, 0x5C + (uint32_t)iVar2);
            env->fn6F930(game, request);
            param2 -= 2;
            iVar3 += 4;
        } while (iVar3 < 0x10);
        request[7] = rd32(t, 0x20 + (uint32_t)iVar2);
        request[8] = rd32(t, 0x00 + (uint32_t)iVar2);
        env->fn6F930(game, request);
        param2 -= 1;
        iVar2 += 4;
        if (0x1B < iVar2) return;
    }
}

