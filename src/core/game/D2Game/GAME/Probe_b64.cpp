// src/core/game/D2Game/GAME/Probe_b64.cpp -- twelve UNMAPPED 1.13c D2Game.dll rows reimplemented from the
// cached decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c), in RVA order:
//   0xBD040 (0xBE2A0 moved to d2game_be2a0.cpp) 0xC39B0 0xC8FD0 0xCB1F0 0xCB690 0xCBA20 0xCBF50 0xCE040 0xCE0D0 0xCE120 0xCE150
// Base 0x6FC20000. Batch lane: implementation + marker only, no spec, no vector.
//
// Register-only inputs (EAX/EBX/EBP/ESI/EDI, caller-frame words) are explicit leading/extra params, commented
// per function. Callees not implemented here are seams on B64Env, in retail argument order; Ordinal_NNNNN
// are 1.13c D2Common/Fog import numbers. Addresses are 32-bit retail pointers.
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include "../../../d2game/d2game_cbf50.h"
#include <test_seam.h>
#include <GAME/Clients.h>
#include <PLAYER/PlrMsg.h>

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
static inline uint16_t rd16(uint32_t a, uint32_t o) { uint16_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 2); return v; }
static inline uint8_t  rd8 (uint32_t a, uint32_t o) { return *(const uint8_t*)(uintptr_t)(a + o); }
static inline void wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(a + o), &v, 4); }
static inline void wr16(uint32_t a, uint32_t o, uint16_t v) { std::memcpy((void*)(uintptr_t)(a + o), &v, 2); }
static inline void wr8 (uint32_t a, uint32_t o, uint8_t v)  { *(uint8_t*)(uintptr_t)(a + o) = v; }

struct B64Env {
    uint32_t (*o10695)(uint32_t id);                       // monster/item record by id
    uint32_t (*o10450)(uint32_t code, uint32_t* out);      // code lookup
    uint32_t (*rand211D0)(uint32_t seedPtr);               // 0x211D0 seed roll
    uint32_t (*o10041)();                                  // table getter (BE2A0)
    uint32_t (*o10237)(uint32_t v);
    uint32_t (*o10045)(const char* file, uint32_t line);   // alloc
    uint32_t (*o10387)(uint32_t a, uint32_t b);
    uint32_t (*o10973)(uint32_t game, uint32_t a, uint32_t b);
    uint32_t (*spawn31980)(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g, uint32_t h, uint32_t i, uint32_t j);
    void     (*o10012)(uint32_t unit, uint32_t v);
    uint32_t (*o10087)(uint32_t unit);
    uint32_t (*o10711)(uint32_t unit);
    uint32_t (*o10048)(uint32_t unit);
    uint32_t (*o10463)(uint32_t unit);
    void     (*o10887)(uint32_t unit, uint32_t a, uint32_t b, uint32_t c);
    void     (*f2380)(uint32_t a, uint32_t unit);
    uint32_t (*o10202)(uint32_t unit, uint32_t a, uint32_t b, const char* file);
    uint32_t (*o11116)(uint32_t unit);
    void     (*f5690)(uint32_t p);
    void     (*o10977)(uint32_t unit, uint32_t a, uint32_t b);
    uint32_t (*o10750)(uint32_t path);                     // path Y
    uint32_t (*o10867)(uint32_t path);                     // path X
    uint32_t (*f76a0)(uint32_t p, uint32_t room, uint32_t id);
    void     (*f2ee70)();
    uint32_t (*fE660)(uint32_t code);
    uint32_t (*fC1B60)(uint32_t game);
    uint32_t (*f1280)(uint32_t room);
    void     (*f54130)(uint32_t room, uint32_t pos, uint32_t a, uint32_t b);
    uint32_t (*fEF10)(void* q);
    uint32_t (*fF650)(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g, uint32_t h);
    void     (*f68410)(uint32_t game, uint32_t unit, uint32_t z);
    void     (*f49820)(uint32_t game);
    void     (*fE20A0)(uint32_t game, uint32_t unit);
    void     (*o10024)(const char* s, uint32_t a);
    uint32_t (*o10265)(uint32_t n);
    uint32_t (*o10601)(uint32_t p, const char* file, uint32_t line);
    uint32_t (*o11139)(uint32_t inv, uint32_t slot);
    uint32_t (*o11006)(uint32_t item);
    uint32_t (*o11088)(uint32_t item);
    uint32_t (*o10744)(uint32_t item, uint32_t stat);
    uint32_t (*o10631)(uint32_t inv);
    uint32_t (*o10708)(uint32_t unit, uint32_t idx, int32_t m1);   // 0xA3D0: ecx = unit, edx = idx, stack = -1
    void     (*o11031)(uint32_t inv, uint32_t slot);
    uint16_t (*o10764)(uint32_t path);
    uint16_t (*o10983)(uint32_t path);
    void     (*fAA3E0)(uint32_t ecx, const uint8_t* pkt, uint32_t len);
    uint32_t (*o10920)(uint32_t unit);
    uint32_t (*f1100)(uint32_t unit);
    uint32_t (*f1130)(uint32_t unit);
    void     (*fBF80)(uint32_t ecx, uint32_t a, uint32_t b, int16_t c, uint16_t d, uint8_t e);
    void     (*f98550)(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g);
    void     (*f98430)(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);
    void     (*o10329)();
    uint32_t (*fC230)(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint8_t e, uint32_t f, int16_t g, uint16_t h);
    uint32_t (*o10392)(uint32_t v);
    uint32_t (*o10331)(uint32_t unit);
    uint32_t (*f003A0)(uint32_t a, uint32_t b);
    void     (*f007A0)(uint32_t p);
    uint32_t (*tab_ce4)(uint32_t idx);                     // byte at DAT_6fd22ce4 + idx*12
    uint32_t (*tab_ce8)(uint32_t idx);                     // dword at DAT_6fd22ce8 + idx*12
    uint32_t datF80, datF7C;                               // DAT_6fd31f80 / DAT_6fd31f7c (cached codes)
};
static const B64Env* g_b;
extern "C" void d2_b64_setenv(const B64Env* e) { g_b = e; }

static inline bool unitHasPath(int32_t t) { return t == 2 || (t > 3 && t < 6); }
static uint32_t unitX(uint32_t u) { uint32_t p = rd32(u, 0x2c); return unitHasPath(rd32(u, 0)) ? rd32(p, 0xc) : p ? g_b->o10867(p) : 0; }
static uint32_t unitY(uint32_t u) { uint32_t p = rd32(u, 0x2c); return unitHasPath(rd32(u, 0)) ? rd32(p, 0x10) : p ? g_b->o10750(p) : 0; }

// Monster-class pick (thiscall on the game; level in a register; stack args classId, factorA, factorB). Both table lookups write their own
// out local (X for the first, Y for the second), and the random roll takes the game's seed slot plus 8 as its seed pointer.
uint32_t d2game_bd040(uint32_t game, uint32_t classId, int32_t level, int32_t factorA, int32_t factorB) {
    if (rd32(game, 0x70) == 0) return classId;
    uint32_t rec = g_b->o10695(classId);
    if (rec == 0) return classId;
    const uint32_t seed = rd32(game, 0x1d24) + 8;
    const uint32_t c2 = rd32(rec, 0x8c);
    const uint32_t c = rd32(rec, 0x88);
    if (c == 0x20202020 || c == 0) return classId;
    uint32_t X = 0, Y = 0;
    uint32_t r = g_b->o10450(c, &X);
    int32_t v;
    if (r == 0 || (v = (level - rd8(r, 0xfd)) * factorA + 1) <= 0) return classId;
    if ((int32_t)g_b->rand211D0(seed) < v) return X;
    const uint32_t e = rd32(rec, 0x8c);
    if (e != 0x20202020 && e != 0) {
        r = g_b->o10450(c2, &Y);
        if (r != 0 && (v = (level - rd8(r, 0xfd)) * factorB + 1) > 0 && (int32_t)g_b->rand211D0(seed) < v) return Y;
    }
    return classId;
}

// 0xC39B0, 0xC8FD0, 0xCB1F0, 0xCB690 and 0xCBA20 run the game-tree bodies (QUESTS/ACT2/A2Q2.cpp, A2Q4.cpp, PLAYER/PlrMsg.cpp); their probes are src/core/fn/d2game_<rva>.cpp.

// D2Game 0xCBF50 lives in d2game_cbf50.cpp (retail convention: EAX = packet, ESI = unit, stack game/length/&x/&y); this adapter
// feeds it the batch seams. The x word lands in *out1, y in *out2.
// The 0xCB690 range test is the game's sub_6FC83C60; the unit's position reads are the CLIENTS_GetUnitX / Y seams.
static uint32_t b64_cb690(uint32_t eax, uint32_t ebx, uint32_t s0, uint32_t s1) {
    D2Seam<&CLIENTS_GetUnitX>::hook = [](D2UnitStrc* u) -> int32_t { return (int32_t)unitX((uint32_t)(uintptr_t)u); };
    D2Seam<&CLIENTS_GetUnitY>::hook = [](D2UnitStrc* u) -> int32_t { return (int32_t)unitY((uint32_t)(uintptr_t)u); };
    const int32_t v = sub_6FC83C60((D2UnitStrc*)(uintptr_t)eax, (uint16_t)s0, (uint16_t)s1, (int32_t)ebx);
    D2Seam<&CLIENTS_GetUnitX>::hook = nullptr; D2Seam<&CLIENTS_GetUnitY>::hook = nullptr;
    return (uint32_t)v;
}
static uint32_t b64_o10920(uint32_t u) { return g_b->o10920(u); }
static uint32_t b64_1130(uint32_t u) { return g_b->f1130(u); }
static uint32_t b64_1100(uint32_t u) { return g_b->f1100(u); }
static void b64_bf80(uint32_t, uint32_t id, uint32_t c, uint32_t a, uint32_t b, uint32_t f) { g_b->fBF80(0, id, c, (int16_t)a, (uint16_t)b, (uint8_t)f); }
int32_t d2game_cbf50(uint32_t pkt, uint32_t unit, uint32_t game, int32_t param2, uint16_t* out1, uint16_t* out2,
                     uint32_t) {
    static const D2GameCBF50Env env{ b64_o10920, b64_cb690, b64_1130, b64_1100, b64_bf80 };
    return (int32_t)d2game_cbf50_port(pkt, unit, game, (uint32_t)param2, (uint32_t)(uintptr_t)out1, (uint32_t)(uintptr_t)out2, &env);
}

// standalone port of D2Game 0xCE040 (marker on 1.10f Rcv0x03_Run) (fastcall ecx=param_1, edx=param_2; stack param_3, param_4; hidden regs of 0xCBF50 forwarded)
int32_t d2game_ce040(uint32_t p1, uint32_t p2, uint16_t p3, uint32_t p4, uint32_t pkt, uint32_t unit, uint32_t game, uint32_t slot5) {
    uint16_t o2; int32_t r = d2game_cbf50(pkt, unit, game, (int32_t)p4, &p3, &o2, slot5);
    if (r == 0) { g_b->f98550(p1, p2, 0, 3, p3, o2, 0); r = 0; }
    return r;
}

// standalone port of D2Game 0xCE0D0 (marker on 1.10f Rcv0x01_Walk) (same shape as 0xCE040 with mode 2)
int32_t d2game_ce0d0(uint32_t p1, uint32_t p2, uint16_t p3, uint32_t p4, uint32_t pkt, uint32_t unit, uint32_t game, uint32_t slot5) {
    uint16_t o2; int32_t r = d2game_cbf50(pkt, unit, game, (int32_t)p4, &p3, &o2, slot5);
    if (r == 0) { g_b->f98550(p1, p2, 0, 2, p3, o2, 0); r = 0; }
    return r;
}

// 1.13c D2Game+0xCE120 (EBX, ESI hidden inputs)
void d2game_ce120(uint32_t ebx, uint32_t esi) {
    g_b->f98430(0, 3, ebx, esi, 0);
    g_b->o10329();
}

// D2Game 1.13c 0xCE150: the retail-layout port is src/core/d2game/d2game_ce150.cpp (ECX = game, EDX = unit, stack client, table index).
