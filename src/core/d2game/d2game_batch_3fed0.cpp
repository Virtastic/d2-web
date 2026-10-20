// src/core/d2game/d2game_batch_3fed0.cpp -- nine 1.13c D2Game.dll monster-mode "do" functions (0x3FED0-0x42570)
// transcribed from the cached Ghidra decompiles. Base 0x6FC20000. Same host-seam style as d2game_batch_cc030.cpp:
// every unmapped callee is a D2GameBatch3fEnv seam; unit/game are opaque retail records read by byte offset.
// Not transcribed (decompile drops stack stores / garbles control flow): 0x405E0, 0x406D0, 0x41FB0.
// ponytail: where Ghidra shows an unaff_ register or a stale param, the obvious live value is used (game for the
// tail-call argument, the monstats row for 0x41240's EBX); noted per function.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void* rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }

struct D2GameBatch3fEnv {
    void*    dataTables;                                   // *sgptDataTables: +0xB64 monstats, +0xB6C count, +0xB98/+0xBA0 skills, +0xC4 missiles cnt, +0xBD4, +0xBC0
    uint32_t (*tail)(void* game);                          // 0x3F320, the common mode tail
    void*    (*getTarget)(void* game, uint32_t guid);      // 0xE03A0
    uint32_t (*targetBusy)(void);                          // 0xDFA90
    void*    (*refreshTarget)(void* game);                 // 0xE07A0
    uint32_t (*gameKey)(void* game);                       // 0xE0A20
    uint32_t (*unitX)(void* unit);                         // 0x1100 via 0x21100
    uint32_t (*unitY)(void* unit);                         // 0x1130 via 0x21130
    uint32_t (*roll)(uint32_t a, uint32_t b);              // 0x214D0 scaled-percent
    uint32_t (*sendRequest)(void* game, uint32_t* req);    // 0x6F930/0x8F930 request sink (23 dwords)
    void     (*sub_A0000)(void* game);                     // 0xA0000
    void     (*sub_CC0000)(void* game);                    // FUN_6fcc0000
    void     (*sub_3A1D0)(void* game);
    void     (*sub_3EDE0)(void* game, void* p, uint32_t f);
    void     (*sub_3D1E0)(uint32_t v);
    uint32_t (*o10240get)(void* unit);                    // Ordinal_10240(unit)
    void     (*o10240set)(void* unit, uint32_t v);
    uint32_t (*o10985)(void* unit);
    uint32_t (*o11029)(void* unit);
    uint32_t (*o11029set)(void* unit, int32_t skill, uint32_t x, uint32_t y);
    uint32_t (*o10973)(void* unit, uint32_t stat, uint32_t p);
    uint32_t (*o10913)(void* unit);
    void     (*o10786)(void* unit, uint32_t a, uint32_t b);
    void     (*o10887)(void* unit, uint32_t stat, uint32_t v, uint32_t p);
    void     (*o10689)(void* unit, int32_t a, uint32_t b);
    void     (*o10970)(void* unit, uint32_t mode);
    void*    (*o10203)(void* room);                        // room validity
    uint32_t (*o10737)(void* unit);
    uint32_t (*o10522)(void* a, void* b);
    void*    (*o10392)(void* room);
    void     (*o10286)(void* room, uint32_t x, uint32_t y);
    void     (*o10334)(void* room, void* unit, uint32_t z);
    uint32_t (*sub_BD00)(void* game);                      // 0xE0BD0
    void     (*sub_E0570)(void* unit);
    void*    (*sub_C00B0)(void);                           // 0xCC00B0
    void*    (*o10786r)(void* unit, uint32_t a, uint32_t b, uint32_t c);
    void     (*o10188)(void* p, int32_t a, void* b, uint32_t c);
    void     (*o11039)(void* game, int32_t skill, uint32_t one);
};

static inline void* monsRow(const D2GameBatch3fEnv* e, void* unit)
{
    if (!unit) return nullptr;
    int32_t i = (int32_t)rd32(unit, 4);
    if (i < 0 || i >= (int32_t)rd32(e->dataTables, 0xB6C)) return nullptr;
    return (uint8_t*)rdptr(e->dataTables, 0xB64) + (uint32_t)i * 0x1A4;
}
static inline void* skillRow(const D2GameBatch3fEnv* e, int32_t i)
{
    if (i < 0 || i >= (int32_t)rd32(e->dataTables, 0xBA0)) return nullptr;
    return (uint8_t*)rdptr(e->dataTables, 0xB98) + (uint32_t)i * 0x23C;
}
static inline bool hasTargetFlag(void* unit) { return (rd32(unit, 0xC8) >> 10) & 1; }

// 1.13c D2Game+0x3FED0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_3fed0(void* game, void* unit, const D2GameBatch3fEnv* e)
{
    void* rec = monsRow(e, unit);
    if (!rec) return 2;
    void* sk = skillRow(e, (int32_t)e->o10240get(unit));
    if (!sk) return 2;
    void* tgt;
    if (!hasTargetFlag(unit) || !(tgt = e->getTarget(game, rd32(unit, 0x98))) || e->targetBusy() != 0)
        return e->tail(game);
    uint32_t v = e->o11029(unit);
    if (e->o10985(unit) == rd32(rec, 0x38)) {
        int32_t cur = (int32_t)e->o10973(tgt, 6, 0);
        int32_t max = (int32_t)e->o10913(tgt) >> 8;
        if ((cur >> 8) < max) {
            e->o10786(tgt, rd32(sk, 0x138), v);
            int32_t n = (cur >> 8) + (int32_t)e->roll(100, (uint32_t)max);
            if (max <= n) n = max;
            e->o10887(tgt, 6, (uint32_t)n << 8, 0);
            int16_t s = (int16_t)rd16(rec, 0x36);   // ponytail: Ghidra read uVar4+0x36 (uVar4==1); monstats row +0x36 taken
            if (0 < s && s < (int32_t)rd32(e->dataTables, 0xBC0)) e->o10689(tgt, s, 0);
        }
    }
    return e->tail(game);
}

// 1.13c D2Game+0x40420
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_40420(void* game, void* unit, const D2GameBatch3fEnv* e)
{
    void* rec = monsRow(e, unit);
    if (!rec || rd16(rec, 0x18) == 0) return 2;
    int32_t n = (int32_t)rd32(rec, 0x38);
    if (n < 1) n = 1;
    if ((int32_t)e->o10985(unit) % n == 0) {
        uint32_t y = e->unitY(unit), x = e->unitX(unit);
        e->o10240set(unit, e->o11029set(unit, (int16_t)rd16(rec, 0x18), x, y));
        e->gameKey(game);
        e->sub_CC0000(game);
    }
    return e->tail(game);
}

// 1.13c D2Game+0x40860
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_40860(void* game, void* unit, const D2GameBatch3fEnv* e)
{
    if (unit && hasTargetFlag(unit) && e->getTarget(game, rd32(unit, 0x98)) && e->targetBusy() == 0) {
        e->sub_3A1D0(game);
        e->o10970(unit, 10);
        e->tail(game);
        return 1;
    }
    e->sub_3EDE0(game, nullptr, 1);
    return 2;
}

// 1.13c D2Game+0x41EB0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_41eb0(void* game, void* unit, const D2GameBatch3fEnv* e)
{
    void* rec = monsRow(e, unit);
    if (!rec) return 2;
    if (rd16(rec, 0x18) != 0 && e->o10203(rdptr(unit, 0x2C))) {
        uint32_t y = e->unitY(unit), x = e->unitX(unit);
        e->o10240set(unit, e->o11029set(unit, (int16_t)rd16(rec, 0x18), x, y));
        e->gameKey(game);
        e->sub_CC0000(game);
    }
    return d2_game_40860(game, unit, e);
}

// Shared 0x21-request body of 0x41110 / 0x41850 (23 dwords; +0 type 0x21, +4 key, +0x10 target+0x18, +0x14/+0x18 and
// +0x1C/+0x20 the unit x,y twice, +0x28 Ordinal_10240, +0x30 Ordinal_11029).
static void send21(const D2GameBatch3fEnv* e, void* game, uint32_t key, void* unit, void* tgt)
{
    uint32_t x = e->unitX(unit), y = e->unitY(unit);
    uint32_t b[23] = {};
    b[0] = 0x21; b[1] = key;
    b[4] = (uint32_t)(int32_t)(int16_t)rd16(tgt, 0x18);
    b[5] = x; b[6] = y; b[7] = x; b[8] = y;
    b[10] = e->o10240get(unit);
    b[12] = e->o11029(unit);
    e->sendRequest(game, b);
}

// 1.13c D2Game+0x41110
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_41110(void* game, void* unit, const D2GameBatch3fEnv* e)
{
    void* rec = monsRow(e, unit);
    if (!rec || (int16_t)rd16(rec, 0x18) < 0 || !hasTargetFlag(unit)) return 2;
    void* tgt = e->getTarget(game, rd32(unit, 0x98));
    if (!tgt) return 2;
    if (e->o10203(rdptr(unit, 0x2C))) send21(e, game, e->gameKey(game), unit, tgt);
    return e->tail(game);
}

// 1.13c D2Game+0x41850
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_41850(void* game, void* unit, const D2GameBatch3fEnv* e)
{
    if (!unit || !hasTargetFlag(unit)) return 2;
    void* tgt = e->getTarget(game, rd32(unit, 0x98));
    if (!tgt) return 2;
    void* rec = monsRow(e, unit);
    if (!rec || (int16_t)rd16(rec, 0x18) < 0 || !rdptr(unit, 0x2C)) return 2;
    if (e->o10203(rdptr(unit, 0x2C))) send21(e, game, (uint32_t)(uintptr_t)game, unit, tgt);  // key is param_1 as shown
    return e->tail(game);
}

// 1.13c D2Game+0x41240
// EBX is unresolved in the decompile; it is the monstats row of the unit (+0x38 period, +0x3C range, +0x40 step).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_41240(void* game, void* unit, const D2GameBatch3fEnv* e)
{
    void* rec = monsRow(e, unit);
    if (!rec) return 2;
    e->refreshTarget(game);
    void* near = e->o10392(rdptr(unit, 0x2C));
    if (near == unit) near = nullptr;
    void* tgt = hasTargetFlag(unit) ? e->getTarget(game, rd32(unit, 0x98)) : nullptr;
    if (!near) {
        if (!tgt) return e->tail(game);
        e->refreshTarget(game);
        near = e->o10392(rdptr(tgt, 0x2C));
        if (near == tgt || !near) return e->tail(game);
    }
    int32_t per = (int32_t)rd32(rec, 0x38);
    if (per < 1) per = 5;
    if ((int32_t)e->o10737(unit) % per == 0 && (int32_t)e->o10522(unit, near) <= (int32_t)rd32(rec, 0x3C)) {
        int32_t ux = (int32_t)e->unitX(unit), uy = (int32_t)e->unitY(unit);
        int32_t tx = (int32_t)e->unitX(near), ty = (int32_t)e->unitY(near);
        int32_t st = (int32_t)rd32(rec, 0x40);
        if (st < 1) st = 1;
        int32_t dx = tx > ux ? st : (tx < ux ? -st : 0);
        int32_t dy = ty > uy ? st : (ty < uy ? -st : 0);
        e->o10286(rdptr(unit, 0x2C), (uint32_t)(e->unitX(unit) + dx), (uint32_t)(e->unitY(unit) + dy));
        e->o10334(rdptr(unit, 0x2C), unit, 0);
    }
    return e->tail(game);
}

// 1.13c D2Game+0x41670
// ponytail: the decompile shows `invalid row && _DAT_00000018 < 0` (null deref artefact); read as invalid || row+0x18 < 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_41670(void* game, void* unit, const D2GameBatch3fEnv* e)
{
    void* rec = monsRow(e, unit);
    if (!rec || (int16_t)rd16(rec, 0x18) < 0) return 2;
    e->sub_3D1E0(e->o11029(unit));
    return e->tail(game);
}

// 1.13c D2Game+0x42570
// unaff_EDI in the decompile is taken as the game (Ordinal_11039's first argument).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_42570(void* game, void* unit, int32_t skillIdx, uint32_t p4, const D2GameBatch3fEnv* e)
{
    void* sk = skillRow(e, skillIdx);
    if (!sk) return 0;
    int16_t a = (int16_t)rd16(sk, 0x54), b = (int16_t)rd16(sk, 0x80);
    if (a < -1 || a >= (int32_t)rd32(e->dataTables, 0xBD4) || b < 0 || b >= (int32_t)rd32(e->dataTables, 0xC4)) return 0;
    if (e->sub_BD00(game)) e->sub_E0570(unit);
    e->o10786r(unit, rd32(sk, 0x60), (uint32_t)skillIdx, p4);
    void* list = e->sub_C00B0();
    if (!list) return 0;
    for (uint32_t i = 0; i < 6; ++i) {
        int16_t s = (int16_t)rd16(sk, 0x54 + 2 * i);
        if (s >= 0 && s < (int32_t)rd32(e->dataTables, 0xBD4)) {
            void* it = e->o10786r(unit, rd32(sk, 0x68 + 4 * i), 0, 0);
            if (it) e->o10188(list, s, it, 0);
        }
    }
    e->o11039(game, (int16_t)rd16(sk, 0x80), 1);
    return 1;
}
