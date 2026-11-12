// src/core/d2game/d2game_batch_d8ba0.cpp -- twelve 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000.
//
// Retail state is opaque host records read by retail byte offsets. Register-only arguments (Ghidra's
// in_EAX / unaff_*) are explicit parameters, named per function; their roles are GUESSES from use.
// Callees are seams on D2GameBatchD8baEnv in retail argument order; a seam whose decompile shows no
// arguments (register-passed) takes the obvious register value as a leading parameter where noted.
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

static inline uint8_t  d8_rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline int32_t  d8_rds32(const void* p, uint32_t o) { int32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline uint32_t d8_rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline uint16_t d8_rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline void d8_wr32(void* p, uint32_t o, int32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline const void* d8_ptr(const void* p, uint32_t o) { const void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }

[[noreturn]] static void d8_fatal(uint32_t) { _exit(-1); }  // Fog #10265 -> #10024 -> _exit(-1)

#include "d2game_batch_d8ba0.h"
const D2GameBatchD8baEnv* d2game_d8ba_env = nullptr; // unbound until a probe sets it
#define E (*d2game_d8ba_env)

// 1.13c D2Game+0xD8BA0 -- EAX = int* out (set 0, then 1 when the node is missing or not yet usable), ECX = unit id,
// stack: game, unit, a2, a3, a4 (ret 0x14). Walks game+0x1720 (hash of 0x7f chains, next at +0xE4) for the node with
// the id, runs the use checks and the 0xAD0F0 gate, then queues the follow-up (0xD17B0 result) on the unit.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2game_d8ba0(int32_t* outEax, uint32_t thisId, const void* game, void* unit,
                                                     uint32_t a2, uint32_t a3, int32_t a4) {
    if (!game) d8_fatal(0x1326);
    if (!unit) d8_fatal(0x1327);
    *outEax = 0;
    E.sub29F0(unit);
    uint8_t* node = (uint8_t*)d8_ptr(game, 0x1720 + (thisId & 0x7f) * 4);
    for (;; node = (uint8_t*)d8_ptr(node, 0xe4)) {
        if (!node) { *outEax = 1; return 0; }
        if (d8_rd32(node, 12) == thisId) break;
    }
    if (d8_rd32(node, 0) != 4) return 0;
    if (E.ord11017(d8_rds32(unit, 0x60)) != 0) return 0;
    if (d8_rd32(node, 0x10) != 2 || !E.ord10478(node)) { *outEax = 1; return 0; }
    if (E.sub70D60(unit, (void*)game) != 0) return 0;
    void* target = unit;
    if (a4 != 0 && d8_rd32(unit, 0) == 0 && d8_rd32(game, 0x70) != 0) {
        if (!E.hasSkill(node, 0x4c) && !E.hasSkill(node, 0x51) && !E.hasSkill(node, 0x50)) return 0;
        uint8_t* p = (uint8_t*)E.ord10920(unit);
        if (p && d8_rds32(d8_ptr(E.dataTables, 0), 0xbf0) > 7 && d8_ptr(p, 0x44)) {
            const uint8_t* q = (const uint8_t*)d8_ptr(d8_ptr(p, 0x44), 0);
            q = (const uint8_t*)d8_ptr(q, 0x54);
            while (q && (d8_rd8(q, 0) & 1)) q = (const uint8_t*)d8_ptr(q, 0x14);
            if (q) {
                uint32_t id = d8_rd32(q, 4);
                const uint8_t* m = (const uint8_t*)d8_ptr(game, 0x1320 + (id & 0x7f) * 4);
                while (m && d8_rd32(m, 0xc) != id) m = (const uint8_t*)d8_ptr(m, 0xe4);
                if (m) target = (void*)m;
            }
        }
    }
    if (!E.subAD0F0((void*)game, node, a2, a3, node, target)) return 0;
    uint32_t r21100 = E.sub21100(node);
    int32_t c = E.sub17B0();
    if (c != -1) {
        void* h = E.ord10708(unit, (uint32_t)c, -1);
        if (h) {
            int32_t n = E.ord10353(h) - 1;
            if (n < 1) { if (E.ord10507(unit) == h) E.sub2100(unit, 0, 0, -1); n = 0; }
            E.ord10252(h, n);
            uint32_t id = d8_rd32(unit, 12), ty = d8_rd32(unit, 0), esiV = 0;
            if (ty == 0) esiV = d8_rd32(E.ord10920(unit), 0x9c);
            E.sub8D570(ty, (uint32_t)c, id, esiV, (uint32_t)n);
        }
    }
    E.sub29F0(unit);
    E.sub3950(node, unit, (void*)game);
    E.sub2540(r21100, unit);
    return 1;
}

// 1.13c D2Game+0xD8DE0 -- EAX = unit (may be 0), stack: the index (ret 4). Drops one use of the handle 0xA3D0 finds
// for (unit, index), then reports through 0x8D570.
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_d8de0(void* unit, uint32_t idx) {
    void* h = E.ord10708(unit, idx, -1);
    if (!h) d8_fatal(0xfc5);
    int32_t n = E.ord10353(h) - 1;
    if (n < 1) { if (E.ord10507(unit) == h) E.sub2100(unit, 0, 0, -1); n = 0; }
    E.ord10252(h, n);
    if (!unit) { E.sub8D570(6, idx, 0xffffffffu, 0, (uint32_t)n); return; }
    uint32_t ty = d8_rd32(unit, 0), id = d8_rd32(unit, 12);
    if (ty == 0) { void* p = E.ord10920(unit); E.sub8D570(0, idx, id, d8_rd32(p, 0x9c), (uint32_t)n); return; }
    E.sub8D570(ty, idx, id, 0, (uint32_t)n);
}

// 1.13c D2Game+0xDA7B0 -- unaff_ESI = ctx record (+0xc unit, +0x18 stat block).
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_da7b0(void* esi) {
    uint8_t* b = (uint8_t*)d8_ptr(esi, 0x18);
    void* u = (void*)d8_ptr(esi, 0xc);
    if (d8_rds32(b, 0x30) > 0 || d8_rds32(b, 0x34) > 0) {
        if (E.statBase(u, 0x99, 0)) { d8_wr32(b, 0x30, 0); d8_wr32(b, 0x34, 0); }
        else if (E.statBase(u, 0x76, 0)) {
            d8_wr32(b, 0x30, d8_rds32(b, 0x30) / 2);
            d8_wr32(b, 0x34, d8_rds32(b, 0x34) / 2);
        }
    }
    if (d8_rds32(b, 0x2c) > 0 && E.hasState(u, 0x85)) d8_wr32(b, 0x2c, 0);
    if (d8_rds32(b, 0x18) > 0 && E.hasState(u, 0x83)) d8_wr32(b, 0x18, 0);
}

// 1.13c D2Game+0xDA8C0 -- unaff_EDI = unit (passed to 0x372C0), unaff_EBX = accumulator record (+0xc).
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_da8c0(void* edi, void* ebx, void* unitForTest) {
    if (!E.isPlayer(unitForTest)) return;  // GUESS: #10239 arg is a register value
    int32_t add = 0;
    void* p = E.sub372C0(edi);
    if (p && E.hasSkill(p, 0x39)) add = 0x32;
    int32_t s = E.statBase(unitForTest, 0, 0);  // GUESS: #10910 args register-passed, not shown
    if (add + s > 0) d8_wr32(ebx, 0xc, d8_rds32(ebx, 0xc) + add + s);
}

// The mana and life spend helpers are d2game_da930.h.

// 1.13c D2Game+0xDAC10 -- in_EAX = ctx (+4 skill row, +8 attacker, +0xc defender).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2game_dac10(const void* ctx) {
    auto P = [&](uint32_t o) { return (void*)(uintptr_t)d8_rd32(ctx, o); };   // ctx holds 32-bit guest pointers, not wasm64 words
    void* a = P(8);
    void* d = P(0xc);
    if (a && d && a != d) {
        if (d8_rd32(d, 0) == 0) {
            if (d8_rd32(a, 0) != 0 && !E.isMonsterLike(a) &&
                (a == nullptr || d8_rds32(a, 0xc4) >= 0) && !E.isMonsterLike(a))
                return 100;
            return 0x11;
        }
        if (E.isMonsterLike(d) && E.isMonsterLike(a)) return 0x19;
        if (E.isMonsterLike(a) && E.unitTest10064(0, d)) {
            const void* sk = P(4);
            return sk ? d8_rds32(sk, 0x38) : 100;
        }
        if (E.unitTest10278(a) && d && d8_rds32(d, 0xc4) < 0)
            return (E.isMonsterLike(d) ? 0xffffff38 : 0) + 400;  // 200 for a monster-like defender, else 400
    }
    return 100;
}

// 1.13c D2Game+0xDAD70 -- in_EAX = skill-row index holder (int[2]: kind, id), param_1 = unit, param_2 = accumulator.
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_dad70(const int32_t* eax, void* unit, void* acc) {
    if (!eax || eax[0] != 1) return;
    int32_t i = eax[1];
    const uint8_t* dt = E.dataTables;
    if (i < 0 || i >= d8_rds32(dt, 0xa80)) return;
    const uint8_t* row = (const uint8_t*)d8_ptr(dt, 0xa78) + (size_t)i * 0x1a8;
    if (!row || (int16_t)d8_rd16(row, 0x1c) <= 0) return;
    int32_t skill = (int16_t)d8_rd16(row, 0x1c);
    uint8_t buf[1024];
    int32_t n = E.stat10702(unit, 0xb4, buf, 0x80);
    for (int32_t k = 0; k < n; k++)
        if (E.sub21620(skill, d8_rd16(buf, k * 8)))
            d8_wr32(acc, 0xc, d8_rds32(acc, 0xc) + d8_rds32(buf, k * 8 + 4));
}

// 1.13c D2Game+0xDB1F0 -- EBX = attacker unit, EDI = target unit, stack = (int* accumulator, int* value) (ret 8).
// Stat ids 0x73 (zero the value), 0x74 (percent cut, halved for some targets), 0x7b / 0x7c (flat adds).
// EAX at return: 0 without stat 0x7c, else the player test's result.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2game_db1f0(void* attacker, int32_t* acc, int32_t* val, int32_t* edi) {
    if (E.statBase(attacker, 0x73, 0) && edi && edi[0] == 1 &&
        (edi[5] == 0 || (d8_rd8((void*)(intptr_t)edi[5], 0x16) & 10) == 0) &&
        !E.unitTest10064(0, edi) && !E.isMonsterLike(edi))
        *val = 0;
    int32_t p = E.statBase(attacker, 0x74, 0);
    if (p > 0) {
        if (edi && (edi[0] == 0 ||
                    (edi[0] == 1 && (E.unitTest10064(0, edi) || E.sub42A80(edi, 2) || E.isMonsterLike(edi)))))
            p /= 2;
        p = p < 1 ? 0 : (p > 99 ? 100 : p);
        int32_t v = *val, t;
        if (v <= 0x100000) t = (int32_t)((uint32_t)v * (uint32_t)p) / 100;
        else if ((v & ~0xf) < 0x640) t = (int32_t)E.alldiv((int64_t)v * p, 100);
        else t = (v / 100) * p;
        *val = v - t;
    }
    int32_t s = E.statBase(attacker, 0x7b, 0);
    if (s && E.isPlayerLike(edi)) *acc += s;
    s = E.statBase(attacker, 0x7c, 0);
    if (!s) return 0;
    const int32_t pl = E.isPlayer(edi);
    if (pl) *acc += s;
    return pl;
}

// 1.13c D2Game+0xDB3C0 -- unaff_EBX = row (+4,+8,+0xc stat ids), unaff_EDI = ctx (+0 mon rec, +4 int*, +8/+0xc units, +0x14 flag).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2game_db3c0(const int32_t* row, const int32_t* ctx) {
    void* u8 = (void*)(intptr_t)ctx[2];
    void* u12 = (void*)(intptr_t)ctx[3];
    int32_t r = 0;
    if (row[1] != -1) r = E.stat(u12, row[1], 0);
    if (row[3] != -1 && (r < 100 || ctx[5] == 0)) {
        int32_t s = E.stat(u8, row[3], 0);
        if (s) r -= s;
    }
    if (ctx[5] == 0 && row[1] != 0x24 && row[1] != 0x25) {
        const uint8_t* m = (const uint8_t*)(intptr_t)ctx[0];
        if (d8_rds32(m, 0x70) == 0) {
            int8_t c = (int8_t)d8_rd8(m, 0x6d);
            if (c == 1) r -= 0x14; else if (c == 2) r -= 0x32;
        } else r += *(const int32_t*)(intptr_t)ctx[1];
    }
    if (r < 1) return r < -99 ? -100 : r;
    if (ctx[5] == 0) {
        int32_t cap = 0x4b;
        if (row[2] == -1) { if (row[1] == 0x24) cap = 0x32; }
        else { cap = E.stat(u12, row[2], 0) + 0x4b; if (cap > 0x5e) cap = 0x5f; }
        if (r < -99) r = -100;
        if (cap <= r) r = cap;
    }
    if (row[1] == 0x24 && E.hasState(u8, 0x2f) && E.isPlayer(u12)) r = 0;
    return r;
}
