// src/core/d2game/d2game_batch_e2c40.cpp -- twelve 1.13c D2Game.dll functions (0xE2C40..0xE4890)
// transcribed from the cached Ghidra decompiles. Base 0x6FC20000. Implementation batch: no vector.
// Ghidra's in_EAX / ECX are explicit parameters. Unmapped callees are seams on D2GameBatchE2C40Env.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline uint8_t  rd8(const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline void     wr8(void* p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline void     wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }

struct D2GameBatchE2C40Env {
    uint32_t (*d2common)(uint32_t ordinal, uint32_t a, uint32_t b, uint32_t c); // D2Common #ordinal, stdcall/fastcall args as listed
    void*    (*sub33F30)(uint32_t id);              // FUN_6fc53f30(id) -> record with callback at +0xF0
    int      (*isBadCodePtr)(const void* fn);       // kernel32 IsBadCodePtr
    uint32_t (*callCb)(void* rec);                  // (*(rec+0xF0))()
    void     (*fatal)(uint32_t line);               // Ordinal_10265(line); Ordinal_10024(msg,...); _exit(-1)
    // ITEMS_FindQuestItem (0xE660): 4CC code; the unit is a live register not shown in the decompile.
    int      (*findQuestItem)(uint32_t code);
};

// Walk the unit's stat-list chain at game+0x10F4 for the node whose type (+0) == type; node+0x18 is its data.
static void* findNode(const void* game, uint32_t type) {
    void* head = (void*)(uintptr_t)rd32(game, 0x10f4);
    if (!head) return nullptr;
    for (uint32_t n = rd32(head, 0); n; n = rd32((void*)(uintptr_t)n, 0xf4))
        if (rd32((void*)(uintptr_t)n, 0) == type) return (void*)(uintptr_t)n;
    return nullptr;
}
#define P(x) ((void*)(uintptr_t)(x))

// 1.13c D2Game+0xE2C40: find node type 0x11; out pair = data+0x10/+0x14 if data[6]!=0.
EMSCRIPTEN_KEEPALIVE int d2game_E2C40(const void* game, uint32_t* out) {
    void* n = findNode(game, 0x11);
    if (!n) return 0;
    void* d = P(rd32(n, 0x18));
    if (rd8(d, 6) == 0) return 0;
    out[0] = rd32(d, 0x10); out[1] = rd32(d, 0x14);
    return 1;
}

// Shared body of E2D40/E2E80/E2F00. setMode: E2D40/E2E80 set +0xC from 0 to 1 on the early-out.
static uint32_t dispatchCb(D2GameBatchE2C40Env& e, void* p, uint32_t line, bool setMode) {
    if (rd8(p, 0xc) != 5 && rd8(p, 9) != 0) {
        if (setMode && rd8(p, 0xc) == 0) wr8(p, 0xc, 1);
        return 1;
    }
    void* rec = e.sub33F30(rd32(p, 0x10));
    if (!rec) return 0;
    if (e.isBadCodePtr(P(rd32(p, 0xf0)))) e.fatal(line);
    return e.callCb(rec); // retail: (*(rec[0x3c]))()
}
// 1.13c D2Game+0xE2D40
EMSCRIPTEN_KEEPALIVE uint32_t d2game_E2D40(D2GameBatchE2C40Env& e, void* p) { return dispatchCb(e, p, 0x2dc, true); }
// 1.13c D2Game+0xE2E80
EMSCRIPTEN_KEEPALIVE uint32_t d2game_E2E80(D2GameBatchE2C40Env& e, void* p) { return dispatchCb(e, p, 0x21b, true); }
// 1.13c D2Game+0xE2F00 (no mode write)
EMSCRIPTEN_KEEPALIVE uint32_t d2game_E2F00(D2GameBatchE2C40Env& e, void* p) { return dispatchCb(e, p, 0x328, false); }

// 1.13c D2Game+0xE2DB0: node type 0x10: data[0x27]=1, data[0x1c]+=amount.
EMSCRIPTEN_KEEPALIVE void d2game_E2DB0(const void* game, uint32_t amount) {
    void* n = findNode(game, 0x10);
    if (!n) return;
    void* d = P(rd32(n, 0x18));
    wr8(d, 0x27, 1);
    wr32(d, 0x1c, rd32(d, 0x1c) + amount);
}

// 1.13c D2Game+0xE3290: p = {game, unit, target}. #10826(target)==0x68 -> #11090(unit,2).
EMSCRIPTEN_KEEPALIVE void d2game_E3290(D2GameBatchE2C40Env& e, uint32_t* p) {
    void* n = findNode(P(p[0]), 0x14);
    if (e.d2common(10826, p[2], 0, 0) == 0x68) { e.d2common(11090, p[1], 2, 0); return; }
    if (!n) return;
    void* d = P(rd32(n, 0x18));
    wr8(d, 1, 1);
    if (p[1] == 0) {
        wr32(d, 4, 0xffffffff);
    } else {
        wr32(d, 4, rd32(P(p[1]), 0xc));
    }
    e.d2common(11090, p[1], rd32(d, 0xc), 0);
}

// 1.13c D2Game+0xE34D0: three #10003 calls (no args visible, results discarded); game = in_EAX.
// unit = param_1 (type must be 0) -> #10920(unit)->+0x9c; GUESS: the three #10003 calls are
// side-effect-free in the decompile and are issued with no args.
EMSCRIPTEN_KEEPALIVE void d2game_E34D0(D2GameBatchE2C40Env& e, const void* game, const uint32_t* unit) {
    e.d2common(10003, 0, 0, 0); e.d2common(10003, 0, 0, 0); e.d2common(10003, 0, 0, 0);
    void* info = nullptr;
    if (unit && unit[0] == 0) info = P(rd32(P(e.d2common(10920, (uint32_t)(uintptr_t)unit, 0, 0)), 0x9c));
    uint32_t u2 = ((rd16(info, 10) & 0x20) | 0x80) >> 5;
    uint32_t v = u2 * rd8(game, 0x6d) + 3;
    if (((rd16(info, 10) & 0x1f00) >> 8) <= v)
        wr16(info, 10, (uint16_t)((v * 0x100) | (rd16(info, 10) & 0xe0ff)));
}

// 1.13c D2Game+0xE36A0: game=ECX, unit=param_2. 4CCs: 'qf1 ' 0x20316671, 'qf2 ', 'xob '.
EMSCRIPTEN_KEEPALIVE uint32_t d2game_E36A0(D2GameBatchE2C40Env& e, const void* game, uint32_t unit) {
    uint32_t a = e.d2common(10331, unit, 0, 0);
    if (!a) return 0;
    a = e.d2common(10826, a, 0, 0);
    if (!a || (char)e.d2common(10459, a, 0, 0) != 2) return 0;
    void* n = findNode(game, 0x13);
    if (!n) return 0;
    void* d = P(rd32(n, 0x18));
    uint32_t pl = e.d2common(10920, unit, 0, 0);
    uint32_t item = rd32(P(pl), 0x10 + rd8(game, 0x6d) * 4);
    if (!e.d2common(10174, item, 0x12, 0) && !e.findQuestItem(0x20316671) && !e.findQuestItem(0x20326671))
        wr32(d, 0x3c, rd32(d, 0x3c) + 1);
    if (!e.findQuestItem(0x20786f62)) wr32(d, 0x40, rd32(d, 0x40) + 1);
    return 0;
}

// 1.13c D2Game+0xE3B40: param_1 = node (data at +0x18), param_2 = {game,?,?,unit}.
// GUESS: the decompile's trailing writes through iVar4 (=0x14, a skill id) and the absolute
// uRam0000001d are register mislabels; read as the data block (iVar3) at +0xc/+0xb and +0x1d.
EMSCRIPTEN_KEEPALIVE void d2game_E3B40(D2GameBatchE2C40Env& e, void* node, const uint32_t* a) {
    uint32_t pl = e.d2common(10920, a[3], 0, 0);
    uint32_t sk = rd32(P(pl), 0x10 + rd8(P(a[0]), 0x6d) * 4);
    void* d = P(rd32(node, 0x18));
    if (e.d2common(10174, sk, 0x14, 0) || e.d2common(10174, sk, 0, 0)) { // 2nd call arg list truncated in decompile
        wr8(node, 9, 0); wr8(d, 1, 0); wr8(d, 0xc, 0);
        return;
    }
    e.findQuestItem(0x20343367); e.findQuestItem(0x2034336a);
    if (e.d2common(10174, sk, 0x14, 1)) {
        wr8(d, 0x1d, 0); wr8(d, 1, 0); wr8(d, 0xc, 0); // 0x1d: GUESS
        return;
    }
    wr8(d, 1, 0); wr8(d, 0xc, 0);
    bool four = e.d2common(10174, sk, 0x14, 4) != 0;
    wr8(d, 0xc, 3); wr8(d, 0xb, four ? 4 : 3); // GUESS: base is d
}

// E47D0 / E4830 / E4890: 4CC gate, then node 0x10 data[0xc]++.
static uint32_t questBump(D2GameBatchE2C40Env& e, const void* game, uint32_t cc) {
    if (e.findQuestItem(cc) || e.findQuestItem(0x20326671)) return 0;
    void* n = findNode(game, 0x10);
    if (n) { void* d = P(rd32(n, 0x18)); wr32(d, 0xc, rd32(d, 0xc) + 1); }
    return 0;
}
// 1.13c D2Game+0xE47D0 ('qhr ')
EMSCRIPTEN_KEEPALIVE uint32_t d2game_E47D0(D2GameBatchE2C40Env& e, const void* g) { return questBump(e, g, 0x20726871); }
// 1.13c D2Game+0xE4830 ('qbr ')
EMSCRIPTEN_KEEPALIVE uint32_t d2game_E4830(D2GameBatchE2C40Env& e, const void* g) { return questBump(e, g, 0x20726271); }
// 1.13c D2Game+0xE4890 ('qey ')
EMSCRIPTEN_KEEPALIVE uint32_t d2game_E4890(D2GameBatchE2C40Env& e, const void* g) { return questBump(e, g, 0x20796571); }
