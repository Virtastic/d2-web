// src/core/d2game/d2game_batch_3fed0.cpp -- nine 1.13c D2Game.dll monster-mode "do" functions (0x3FED0-0x42570)
// transcribed from the cached Ghidra decompiles. Base 0x6FC20000. Same host-seam style as d2game_batch_cc030.cpp:
// every unmapped callee is a D2GameBatch3fEnv seam; unit/game are opaque retail records read by byte offset.
// Not transcribed (decompile drops stack stores / garbles control flow): 0x405E0, 0x406D0, 0x41FB0.
// ponytail: where Ghidra shows an unaff_ register or a stale param, the obvious live value is used (game for the
// tail-call argument, the monstats row for 0x41240's EBX); noted per function.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "../fn/hookenv.h"

static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void* rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }

#include "d2game_batch_3fed0_env.h"

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

// D2Game+0x3FED0 moved to d2game_3fed0.cpp (retail convention ECX = game, EDX = unit).

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
    if (!rec) return 2;
    // retail (disassembled): q = Ordinal_11029(unit) / max(row+0x40, 1); 0x3D1E0(EAX = max(row+0x3C - q, 3); stack game, row+0x38 + max(q, 2), row+0x18, 5)
    int32_t step = (int32_t)rd32(rec, 0x40);
    if (step < 1) step = 1;
    int32_t q = (int32_t)e->o11029(unit) / step;
    int32_t a = q < 2 ? 2 : q;
    int32_t b = (int32_t)rd32(rec, 0x3C) - q;
    if (b < 3) b = 3;
    e->sub_3D1E0((uint32_t)b, game, rd32(rec, 0x38) + (uint32_t)a, (uint32_t)(int32_t)(int16_t)rd16(rec, 0x18), 5);
    return e->tail(game);
}

// 1.13c D2Game+0x42570 (re-read from the disassembly: EDX = unit, stack = skill index, p4; ret 8)
// owner = 0xE0BD0(ESI = unit); a null owner, or one whose 0xE0570(ECX = unit) is zero, falls back to the unit. The 8-dword block {unit, owner, skill, p4,
// 0xAC9A(unit, row+0x60, skill, p4), -1, 0, row+0x80} goes to 0xA00B0 (EAX); its list takes one 0xA6E2 per valid skill word; 0xA706(owner, row+0x80, 1).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_42570(void* game, void* unit, int32_t skillIdx, uint32_t p4, const D2GameBatch3fEnv* e)
{
    void* sk = skillRow(e, skillIdx);
    if (!sk) return 0;
    int16_t a = (int16_t)rd16(sk, 0x54), b = (int16_t)rd16(sk, 0x80);
    if (a < -1 || a >= (int32_t)rd32(e->dataTables, 0xBD4) || b < 0 || b >= (int32_t)rd32(e->dataTables, 0xC4)) return 0;
    (void)game;
    uint32_t owner = e->sub_BD00(unit);
    if (!owner || e->sub_E0570(unit) == 0) owner = (uint32_t)(uintptr_t)unit;
    uint32_t blk[8] = { (uint32_t)(uintptr_t)unit, owner, (uint32_t)skillIdx, p4, 0, 0xFFFFFFFFu, 0, (uint32_t)(int32_t)b };
    blk[4] = (uint32_t)(uintptr_t)e->o10786r(unit, rd32(sk, 0x60), (uint32_t)skillIdx, p4);
    void* list = e->sub_C00B0(blk);
    if (!list) return 0;
    for (uint32_t i = 0; i < 6; ++i) {
        int16_t s = (int16_t)rd16(sk, 0x54 + 2 * i);
        if (s >= 0 && s < (int32_t)rd32(e->dataTables, 0xBD4)) {
            void* it = e->o10786r(unit, rd32(sk, 0x68 + 4 * i), (uint32_t)skillIdx, p4);
            if (it) e->o10188(list, s, it, 0);
        }
    }
    e->o11039((void*)(uintptr_t)owner, b, 1);
    return 1;
}

// function probe seam (src/core/fn/d2game_40420.cpp). The port's seams are Ghidra-shaped; retail's calls are
// 1130 y, 1100 x, B0FC(unit) level, B26A(unit) class, E0A20(eax unit, game) client (esi), CC0000(esi; game, class, level, skill, x, y).
static HookEnv* g4H; static void* g4U; static void* g4Game; static uint32_t g4Lvl, g4Cls, g4Sk, g4X, g4Y, g4Cli;
uint32_t D2Game_40420_probe(void* game, void* unit, const uint8_t* dt, uint32_t* lg, const uint32_t* sc)
{
    HookEnv h(lg, sc); g4H = &h; g4U = unit; g4Game = game;
    D2GameBatch3fEnv e{};
    e.dataTables = (void*)dt;
    auto P = [](const void* p) { return (uint32_t)(uintptr_t)p; };
    (void)P;
    e.o10985 = [](void* u) { return g4H->h(0xB28E, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)u); };
    e.unitY = [](void* u) { return g4H->h(0x1130, 0, (uint32_t)(uintptr_t)u); };
    e.unitX = [](void* u) { return g4H->h(0x1100, 0, (uint32_t)(uintptr_t)u); };
    e.o11029set = [](void* u, int32_t sk, uint32_t x, uint32_t y) {
        g4Lvl = g4H->h(0xB0FC, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)u); g4Cls = g4H->h(0xB26A, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)u);
        g4Sk = (uint32_t)sk; g4X = x; g4Y = y; return 0u; };
    e.o10240set = [](void*, uint32_t) {};
    e.gameKey = [](void* g) { g4Cli = g4H->h(0xE0A20, (uint32_t)(uintptr_t)g4U, 0, 0, 0, 0, (uint32_t)(uintptr_t)g); return g4Cli; };
    e.sub_CC0000 = [](void* g) { g4H->h(0xA0000, 0, 0, 0, 0, g4Cli, (uint32_t)(uintptr_t)g, g4Cls, g4Lvl, g4Sk, g4X, g4Y); };
    e.tail = [](void* g) { return g4H->h(0x3F320, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)g); };
    return d2_game_40420(game, unit, &e);
}
