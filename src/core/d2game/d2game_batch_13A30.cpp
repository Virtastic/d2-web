// src/core/d2game/d2game_batch_13A30.cpp -- twelve 1.13c D2Game.dll functions (RE-DONE, alpha lane)
// transcribed from the cached decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c) and pinned against
// retail 1.13c D2Game.dll disassembly (image base 0x6FC20000). The set is the item-generation helpers
// 0x13A30/0x13C20/0x137F0, the table-roll pair 0x14210/0x14370, the monster-state transition 0xE73C0,
// the monster-state read 0xE2CD0, the two monster packets 0xCB950/0x8D120, the packet forwarders
// 0x8AA10/0x8C010, and the action trampoline 0x8DFD0. Register-only arguments (Ghidra's in_EAX/in_ECX/
// in_EDX/in_ESI/in_EDI) are explicit parameters. Unmapped callees are seams on D2GameBatchB87Env in
// retail argument order. The fatal path is Fog Ordinal 10265(code) -> Ordinal 10024(text) -> _exit(-1);
// only the exit is observable and is modelled.
// Prior art: D2MOO 1.10f ITEMS/ItemMod (affix rolls), ITEMS/ItemDrop (tier table walk), the 0x1180
// ITEMS_RollLimitedRandomNumber LCG, the 0x8A3E0 packet-append helper and the 0x8D3D0 action dispatcher
// spec. The 1.13c data-table slot numbers (+0xC0C/+0xC10/+0xC18/+0xC1C)
// and the record strides 0x128/0x1B8/0x144 are read from the retail disassembly.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr8 (void* p, uint32_t o, uint8_t v)  { ((uint8_t*)p)[o] = v; }
static inline void wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void* rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

// 0x1180 / 0x11D0 ITEMS_RollLimitedRandomNumber: state = lo*0x6AC690C5 + hi (64-bit), the low dword is
// the new state, and the roll is state % nMax (state & (nMax-1) when nMax is a power of two). The state
// pointer is the first argument; the hi dword at +4 is carried through the 64-bit add.
static uint32_t d2_b87_roll(void* pSeed, uint32_t nMax) {
    // Both 0x1180 and 0x11D0 return 0 without touching the state when the bound is <= 0 (signed).
    if ((int32_t)nMax <= 0) return 0;
    uint32_t lo = rd32(pSeed, 0), hi = rd32(pSeed, 4);
    uint64_t prod = (uint64_t)lo * 0x6AC690C5u + hi;
    uint32_t nlo = (uint32_t)prod, nhi = (uint32_t)(prod >> 32);
    wr32(pSeed, 0, nlo);
    wr32(pSeed, 4, nhi);
    return (nMax & (nMax - 1)) ? (nlo % nMax) : (nlo & (nMax - 1));
}

struct D2GameBatchB87Env {
    // D2Common data tables: the dword at 0x6FD1829C dereferenced. +0xC0C/+0xC10 are the tier table and
    // its row count (0x13A30, stride 0x128); +0xC18/+0xC1C the drop table and count (0x13C20, stride 0x1B8).
    void*    dataTableBase;
    // 0x6FD22CE4: the dword array 0xCB950 indexes with tableIndex*12.
    void*    table6fd22ce4;
    // 0x6FD05D20 / 0x6FD03A80: the two state callbacks 0xE73C0 invokes through 0x6FCFFCC0. Retail
    // checks the literal address for null and fatals when it is; a null member models that.
    void*    cbStateChange;
    void*    cbAfterState;
    // 0x8A3E0: append `len` bytes of `buf` to the client's outgoing queue (client in EAX, buf/len pushed).
    void     (*send8A3E0)(void* client, const void* buf, uint32_t len);
    // 0x6FCFFCC0: event dispatch; EAX=0, EBX=callback, EDI=unit, one stack argument.
    void     (*invokeCallback)(uint32_t eaxArg, void* callback, uint32_t stackArg, void* ediArg);
    // 0xE73C0's sprintf("changing state to %d"-shaped retail text, state) + trace when the log global is
    // set. The retail format string is retail content and is not reproduced; only the state passed on.
    void     (*traceState)(void* unit, uint32_t state);
    // 0x8D3D0: EBX=client, ESI=item, stack (nAction, dwFlag, bGamble); ret 0xC.
    void     (*sub8D3D0)(void* client, void* item, uint32_t nAction, uint32_t dwFlag, uint32_t bGamble);
    // 0x101D0: ESI=unit, EDI=playerUnit; returns the party/owner word, 1 for the unit itself, else 0.
    uint16_t (*sub101D0)(void* unit, void* playerUnit);
    // 0xCBBE0: EAX=unit; returns the monster's display word used in the 0x75 packet.
    uint32_t (*subCBBE0)(void* unit);
    // D2Common ordinals. Signatures follow the retail stack order seen at each call site.
    uint32_t (*ordinal10086)(void* item);                                    // #10086 item level, min 1
    int32_t  (*ordinal10110)(void* unit, void* entry);                       // #10110 entry filter
    int32_t  (*ordinal10174)(uint32_t questPtr, uint32_t a, uint32_t b);     // #10174 quest bit read
    void*    (*ordinal10220)(void);                                          // #10220 fixed table struct
    void*    (*ordinal10411)(void* item);                                    // #10411 item seed pointer
    uint32_t (*ordinal10513)(void* pSeed);                                   // #10513 seed low dword
    void     (*ordinal10536)(void* item, uint32_t value);                    // #10536 set item +0x28
    uint32_t (*ordinal10587)(void* item, uint32_t stat, uint32_t sub);       // #10587 stat get
    void     (*ordinal10614)(void* pSeed, uint32_t value);                   // #10614 reseed (ECX,EDX)
    void*    (*ordinal10695)(uint32_t unitId);                               // #10695 item data record
    int32_t  (*ordinal10711)(void* item);                                    // #10711 item flag
    uint32_t (*ordinal10718)(void* item);                                    // #10718 item format word
    int32_t  (*ordinal10744)(void* item, uint32_t itemTypeId);               // #10744 item type check
    uint32_t (*ordinal10750)(void* rec);                                     // #10750 word at rec+6
    int32_t  (*ordinal10865)(void* item);                                    // #10865 item flag
    uint32_t (*ordinal10867)(void* rec);                                     // #10867 word at rec+2
    void     (*ordinal10887)(void* item, uint32_t stat, uint32_t value, uint32_t flag); // #10887 set stat
    uint32_t (*ordinal10910)(void* item, uint32_t stat, uint32_t sub);       // #10910 stat base get
    void*    (*ordinal10920)(void* unit);                                    // #10920 player data
    void*    (*ordinal10926)(void);                                          // #10926 fixed table ptr
    uint32_t (*ordinal10973)(void* item, uint32_t stat, uint32_t sub);       // #10973 stat get
    void     (*ordinal10977)(void* item, uint32_t bit, uint32_t set);        // #10977 item flag bit
    void     (*ordinal10996)(uint32_t a, uint32_t b, void* item, uint32_t c,
                             uint32_t d, uint32_t e);                        // #10996 item update
    uint32_t (*ordinal11027)(void* unit);                                    // #11027 unit word
};

// Retail scales with a 32-bit imul (it wraps) and then a signed divide by 100.
static int32_t d2_b87_pct(int32_t v) {
    int32_t prod = (int32_t)((uint32_t)v * 0x4Bu);
    return prod / 100;
}

// 0x13230 GAME_PercentOfWithFloor: EAX=value, ESI=floor, [esp+4]=percent.
extern int32_t GAME_PercentOfWithFloor(int32_t nValue, int32_t nFloor, int32_t nPercent);

// 0x13A30: magic/affix roll over the leading tier table. Resolves the item's data record, takes the
// level prefix slot (0x48/0x49, doubled and capped at 0xFF), counts the table rows whose +4 word is
// below 100, rolls one of them with the item seed, and scans every candidate row's entry list
// (row+0x110, count row+0xC) for the entry whose +0x28 equals the item id (data+0x80). The last match
// wins. A match sets the item's +0x28 word, clears bit 0x10 and runs the item update; no match reseeds
// the item (seed restored) and answers 0. Terms whose row level is >= 100 are skipped unless the item's
// format word (#10718) is non-zero.
// 1.13c D2Game+0x13A30
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_13A30(void* item, const D2GameBatchB87Env* env) {
    void*   rec = env->ordinal10695(item ? rd32(item, 4) : 0xFFFFFFFFu);
    if (!rec) return 0;
    uint32_t seed = env->ordinal10513(env->ordinal10411(item));
    uint32_t exp  = env->ordinal10718(item);
    int32_t a = (int32_t)env->ordinal10973(item, 0x48, 0);
    int32_t b = (int32_t)env->ordinal10587(item, 0x49, 0);
    int32_t da = a + a; if (da >= 0xFF) da = 0xFF;
    int32_t db = b + b; if (db >= 0xFF) db = 0xFF;
    env->ordinal10887(item, 0x48, (uint32_t)da, 0);
    env->ordinal10887(item, 0x49, (uint32_t)db, 0);

    void*    base  = env->dataTableBase;
    uint8_t* table = (uint8_t*)rdptr(base, 0xC0C);
    int32_t  rows  = (int32_t)rd32(base, 0xC10);
    int32_t  m = 0;
    while (m < rows && rd16(table, (uint32_t)m * 0x128 + 4) < 100) m++;

    uint32_t roll = d2_b87_roll(env->ordinal10411(item), (uint32_t)m);
    if (m <= 0) roll = 0;
    int32_t chosen = -1;
    for (int32_t i = 0; i < m; i++) {
        uint32_t idx = ((uint32_t)i + roll) % (uint32_t)m;
        uint8_t* row = table + (size_t)idx * 0x128;
        if (rd16(row, 4) >= 100 && (uint16_t)exp < 100) continue;
        uint32_t n = rd32(row, 0xC);
        for (uint32_t j = 0; j < n; j++) {
            uint8_t* e = (uint8_t*)rdptr(row, 0x110 + 4 * j);
            if (rd32(e, 0x28) == rd32(rec, 0x80)) chosen = (int16_t)rd16(e, 0);
        }
    }
    if (chosen == -1) {
        env->ordinal10614(env->ordinal10411(item), seed);
        return 0;
    }
    env->ordinal10536(item, (uint32_t)chosen);
    env->ordinal10977(item, 0x10, 0);
    env->ordinal10996(4, 0, item, 0, 0, 0);
    return 1;
}

// 0x13C20: weighted drop-table roll. Per row: drop level (row+0x22) >= 100 requires the item format
// word; the signed level cap (row+0x30) must not exceed the item level (#10086); row+0x28 is the item
// id; a row whose +0x2C word is 0x1D requires client +0x80 bit 0; and client+0x40-1 picks the row
// outright (that is the "already chosen" index). Otherwise the row weight (row+0x34, 0 becomes 1) is
// accumulated and the item seed rolls the weighted pick. The pick sets +0x28, clears bit 0x10 and runs
// the item update; an empty table answers 0.
// 1.13c D2Game+0x13C20
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_13C20(void* item, void* client, const D2GameBatchB87Env* env) {
    void*   rec = env->ordinal10695(item ? rd32(item, 4) : 0xFFFFFFFFu);
    if (!rec) return 0;
    uint32_t exp   = env->ordinal10718(item);
    uint32_t level = env->ordinal10086(item);
    void*    base  = env->dataTableBase;
    uint8_t* table = (uint8_t*)rdptr(base, 0xC18);
    uint32_t rows  = rd32(base, 0xC1C);
    int32_t  chosen = -1;
    uint32_t idxs[0x200], wts[0x200], n = 0, total = 0;
    for (int32_t i = 0; i < (int32_t)rows; i++) {
        uint8_t* row = table + (size_t)i * 0x1B8;
        if (rd16(row, 0x22) >= 100 && (uint16_t)exp < 100) continue;
        if ((int32_t)(int16_t)rd16(row, 0x30) > (int32_t)level) continue;
        if (rd32(row, 0x28) != rd32(rec, 0x80)) continue;
        if (rd16(row, 0x2C) == 0x1D && !(rd8(client, 0x80) & 1)) continue;
        if (rd32(client, 0x40) != 0 && rd32(client, 0x40) - 1 == (uint32_t)i) chosen = i;
        uint32_t w = rd32(row, 0x34);
        idxs[n] = (uint32_t)i;
        wts[n] = w ? w : 1;
        total += wts[n];
        n++;
    }
    if (chosen < 0) {
        if (!total) return 0;
        uint32_t roll = d2_b87_roll(env->ordinal10411(item), total);
        uint32_t k = 0;
        for (; k < n; k++) {
            if (roll < wts[k]) break;
            roll -= wts[k];
        }
        if (k >= n) return 0;
        chosen = (int32_t)idxs[k];
    }
    env->ordinal10536(item, (uint32_t)chosen);
    env->ordinal10977(item, 0x10, 0);
    env->ordinal10996(4, 0, item, 0, 0, 0);
    return 1;
}

// 0x137F0: the level prefix / stat roll. The fixed #10926 table's first dword caps the roll that is
// written to item +0x28. With #10865 set, the data record's +0x112 byte / 3 (minimum 1) becomes the
// prefix value and its half is added to a second roll on the item's own seed (+0x20), written to stat
// 0x48, with the prefix value in 0x49. With item type 0x2D, stats 0x16/0x15/0x18/0x17 are set to 75% of
// their old value floored at 2/1/2/1, and for #10711 the 0x9F/0xA0 stats are recomputed from #10910 and
// 0x13230. Item type 0x32 instead sets stat 0x1F to 75% floored at 1. Answers 1 when a type matched,
// else 0.
// 1.13c D2Game+0x137F0
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_137F0(void* item, const D2GameBatchB87Env* env) {
    void* g   = env->ordinal10926();
    void* rec = item ? env->ordinal10695(rd32(item, 4)) : env->ordinal10695(0xFFFFFFFFu);
    if (!g || !rec) return 0;
    uint32_t roll = d2_b87_roll(env->ordinal10411(item), rd32(g, 0));
    env->ordinal10536(item, roll);
    if (env->ordinal10865(item)) {
        void* rec2 = env->ordinal10695(rd32(item, 4));
        if (rec2) {
            int32_t v = (int32_t)rd8(rec2, 0x112) / 3;
            if (v == 0) v = 1;
            int32_t half  = (int32_t)((uint32_t)v >> 1);
            int32_t extra = (int32_t)d2_b87_roll((uint8_t*)item + 0x20, (uint32_t)half) + half;
            if (extra == 0) extra = 1;
            env->ordinal10887(item, 0x48, (uint32_t)extra, 0);
            env->ordinal10887(item, 0x49, (uint32_t)v, 0);
        }
    }
    if (env->ordinal10744(item, 0x2D)) {
        static const uint32_t stat[4] = { 0x16, 0x15, 0x18, 0x17 };
        static const uint32_t floorv[4] = { 2, 1, 2, 1 };
        for (int i = 0; i < 4; i++) {
            int32_t q = d2_b87_pct((int32_t)env->ordinal10587(item, stat[i], 0));
            if (q < (int32_t)floorv[i]) q = (int32_t)floorv[i];
            env->ordinal10887(item, stat[i], (uint32_t)q, 0);
        }
        if (env->ordinal10711(item)) {
            env->ordinal10887(item, 0x9F,
                (uint32_t)GAME_PercentOfWithFloor((int32_t)env->ordinal10910(item, 0x9F, 0), 2, 0x4B), 0);
            env->ordinal10887(item, 0xA0,
                (uint32_t)GAME_PercentOfWithFloor((int32_t)env->ordinal10910(item, 0xA0, 0), 1, 0x4B), 0);
        }
        return 1;
    }
    if (env->ordinal10744(item, 0x32)) {
        int32_t q = d2_b87_pct((int32_t)env->ordinal10587(item, 0x1F, 0));
        if (q <= 0) q = 1;
        env->ordinal10887(item, 0x1F, (uint32_t)q, 0);
        return 1;
    }
    return 0;
}

// 0x14210 / 0x14370: identical bodies, the only difference is the caller-selected cursor semantics.
// #10220 returns a fixed struct: +0 a count, +4 the 0x48-byte element array, +8 and +0xC two cursors.
// The flag selects which cursor starts the walk and how many elements are scanned; every element
// #10110 accepts is collected as {position, element} up to 0x1FF. The item seed then picks one and the
// answer is position + 1 (0 when the projection is empty or the roll is out of range).
static uint32_t d2_b87_pick(void* unit, uint32_t flag, const D2GameBatchB87Env* env) {
    void* p = env->ordinal10220();
    if (!p) return 0;
    uint32_t base = rd32(p, 4);
    uint32_t cur;
    int32_t n, idxBase;
    if (flag) {
        cur = rd32(p, 0xC);
        if (!cur) return 0;
        idxBase = (int32_t)(cur - base) / 0x48;
        n = (int32_t)(rd32(p, 0) * 0x48u - cur + base) / 0x48;
    } else {
        cur = rd32(p, 8);
        if (!cur) return 0;
        idxBase = (int32_t)(cur - base) / 0x48;
        n = (int32_t)(rd32(p, 0xC) - cur) / 0x48;
    }
    uint32_t pos[0x1FF];
    uint8_t* ent[0x1FF];
    uint32_t got = 0;
    uint8_t* e = (uint8_t*)(uintptr_t)cur;
    for (int32_t j = 0; j < n; j++) {
        if (env->ordinal10110(unit, e) && got < 0x1FF) {
            pos[got] = (uint32_t)idxBase + (uint32_t)j;
            ent[got] = e;
            got++;
        }
        e += 0x48;
    }
    if (!got) return 0;
    uint32_t roll = d2_b87_roll(env->ordinal10411(unit), got);
    if ((int32_t)roll >= (int32_t)got) return 0;
    (void)ent;
    return (uint32_t)pos[roll] + 1;
}

// 1.13c D2Game+0x14210
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_14210(void* unit, uint32_t flag, const D2GameBatchB87Env* env) {
    return d2_b87_pick(unit, flag, env);
}

// 1.13c D2Game+0x14370
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_14370(void* unit, uint32_t flag, const D2GameBatchB87Env* env) {
    return d2_b87_pick(unit, flag, env);
}

// 0xE73C0: monster state transition driven by the game's 0x10F4 list (node type 0x13). Requires the
// node's +9 gate, marks record +5, appends the unit id (unit+0xC, -1 for no unit) to the record's
// +0x10 array (deduplicated, count at +0x30/+0x34, at most six), sets node +0xB to 1 (or 3 when the
// node's +0xC is above 1) and fires the state callback from 0x6FD05D20. The tail reads the node game's
// +0x10F4 structure, fatal 0x82C when +8 is null, probes quest 0x11/0xD through #10174 to pick state 4
// or 5, traces it, and fires the 0x6FD03A80 callback. A null callback fatals 0x5B8.
// 1.13c D2Game+0xE73C0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_E73C0(void* game, void* unit, const D2GameBatchB87Env* env) {
    (void)game;
    // EAX=game, ECX=unit.
    if (!game) return;
    void* p = (void*)(uintptr_t)rd32(game, 0x10F4);
    if (!p) return;
    uint8_t* node = (uint8_t*)rdptr(p, 0);
    if (!node) return;
    while (rd32(node, 0) != 0x13) {
        node = (uint8_t*)rdptr(node, 0xF4);
        if (!node) return;
    }
    if (!rd8(node, 9)) return;
    uint8_t* rec = (uint8_t*)rdptr(node, 0x18);
    uint32_t count = rd32(rec, 0x30);
    wr8(rec, 5, 1);
    if ((int32_t)count >= 6) return;
    uint32_t unitId = unit ? rd32(unit, 0xC) : 0xFFFFFFFFu;
    for (uint32_t i = 0; i < count; i++)
        if (rd32(rec, 0x10 + 4 * i) == unitId) return;
    wr32(rec, 0x10 + 4 * count, unitId);
    wr32(rec, 0x30, count + 1);
    wr32(rec, 0x34, count + 1);

    void* u = rdptr(node, 4);
    uint8_t target = (rd8(node, 0xC) > 1) ? 3 : 1;
    if (rd8(node, 0xB) != target) {
        wr8(node, 0x14, 0);
        wr8(node, 0xB, target);
        if (!env->cbStateChange) d2game_fatal(0x5B8);
        env->invokeCallback(0, env->cbStateChange, 0, u);
    }
    void* t = (void*)(uintptr_t)rd32(u, 0x10F4);
    uint32_t quest = rd32(t, 0xC);
    if (!rd32(t, 8)) d2game_fatal(0x82C);
    uint32_t state = (env->ordinal10174(quest, 0x11, 0xD) == 1) ? 4 : 5;
    wr8(node, 0xC, (uint8_t)state);
    env->traceState(u, state);
    env->invokeCallback(0, env->cbAfterState, 0, u);
}

// 0xE2CD0: read the state byte the 0x10F4 list's first type-0x11 node carries at record +4. EAX=game.
// 1.13c D2Game+0xE2CD0
extern "C" EMSCRIPTEN_KEEPALIVE uint8_t d2_fn_d2game_E2CD0(void* game) {
    if (!game) return 0;
    void* p = (void*)(uintptr_t)rd32(game, 0x10F4);
    if (!p) return 0;
    uint8_t* node = (uint8_t*)rdptr(p, 0);
    while (node) {
        if (rd32(node, 0) == 0x11) {
            uint8_t* rec = (uint8_t*)rdptr(node, 0x18);
            return rd8(rec, 4);
        }
        node = (uint8_t*)rdptr(node, 0xF4);
    }
    return 0;
}

// 0xCB950: monster packet 0x0D. EDX=unit, stack (client, tableIndex). The two payload words come from
// the unit's data record: +0xC and +0x10 for unit types 2/4/5, otherwise #10750 and #10867 of the
// record. The dword array at 0x6FD22CE4 supplies the byte, #11027 the trailing byte, and unit +0xB0
// the other.
// 1.13c D2Game+0xCB950
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_CB950(void* unit, void* client, uint32_t tableIndex, const D2GameBatchB87Env* env) {
    uint8_t  b0 = rd8(unit, 0xB0);
    uint32_t tbl = rd32(env->table6fd22ce4, tableIndex * 0xC);
    uint8_t  al = (uint8_t)env->ordinal11027(unit);
    uint32_t type = rd32(unit, 0);
    void* rec = rdptr(unit, 0x2C);
    uint16_t w7, w9;
    if (type == 2 || type == 4 || type == 5) {
        w7 = (uint16_t)rd32(rec, 0xC);
        w9 = (uint16_t)rd32(rec, 0x10);
    } else {
        w7 = rec ? (uint16_t)env->ordinal10867(rec) : 0;
        w9 = rec ? (uint16_t)env->ordinal10750(rec) : 0;
    }
    uint8_t buf[0xD];
    std::memset(buf, 0, sizeof buf);
    buf[0] = 0x0D;
    buf[1] = 0;
    wr32(buf, 2, rd32(unit, 0xC));
    buf[6] = (uint8_t)tbl;
    wr16(buf, 7, w7);
    wr16(buf, 9, w9);
    buf[0xB] = b0;
    buf[0xC] = al;
    env->send8A3E0(client, buf, 0xD);
}

// 0x8AA10: fixed 0x20-byte packet forwarder. EAX=buf, client pushed. The bytes are copied to a stack
// block before the append.
// 1.13c D2Game+0x8AA10
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8AA10(const void* buf32, void* client, const D2GameBatchB87Env* env) {
    uint8_t copy[0x20];
    std::memcpy(copy, buf32, sizeof copy);
    env->send8A3E0(client, copy, 0x20);
}

// 0x8C010: 0x10-byte packet forwarder. CL and EDX are the register payload; the eight stack arguments
// follow (client, byte, word, word, byte, byte, word, word). Layout: [0]=CL, [1..4]=EDX, [5]=arg2,
// [6..7]=arg3, [8..9]=arg4, [0xA]=arg5, [0xB]=arg6, [0xC..0xD]=arg7, [0xE..0xF]=arg8.
// 1.13c D2Game+0x8C010
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8C010(uint8_t cl, uint32_t edx, void* client, uint8_t a2,
                                                         uint16_t a3, uint16_t a4, uint8_t a5, uint8_t a6,
                                                         uint16_t a7, uint16_t a8, const D2GameBatchB87Env* env) {
    uint8_t buf[0x10];
    std::memset(buf, 0, sizeof buf);
    buf[0] = cl;
    wr32(buf, 1, edx);
    buf[5] = a2;
    wr16(buf, 6, a3);
    wr16(buf, 8, a4);
    buf[0xA] = a5;
    buf[0xB] = a6;
    wr16(buf, 0xC, a7);
    wr16(buf, 0xE, a8);
    env->send8A3E0(client, buf, 0x10);
}

// 0x8D120: monster packet 0x75. EAX=unit, EBX=playerUnit. The client is player data(playerUnit)+0x9C
// when playerUnit is a type-0 unit. Payload: unit id, the unit's display word (0xFFFF unless the unit
// is a type-0 unit with +0x80), stat 0x0C word, the party word of the entry whose +0 matches the unit id
// in player data +0x38 (0 when the units are the same or no entry exists), and the 0x101D0 word.
// 1.13c D2Game+0x8D120
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8D120(void* unit, void* playerUnit, const D2GameBatchB87Env* env) {
    void* client = (playerUnit && rd32(playerUnit, 0) == 0)
        ? (void*)(uintptr_t)rd32(env->ordinal10920(playerUnit), 0x9C) : nullptr;
    uint8_t buf[0xD];
    std::memset(buf, 0, sizeof buf);
    buf[0] = 0x75;
    wr32(buf, 1, unit ? rd32(unit, 0xC) : 0xFFFFFFFFu);
    wr16(buf, 5, (uint16_t)((unit && rd32(unit, 0x80) && rd32(unit, 0) == 0)
                            ? env->subCBBE0(unit) : 0xFFFFu));
    wr16(buf, 7, (uint16_t)env->ordinal10973(unit, 0xC, 0));
    uint32_t found = 0;
    if (playerUnit != unit) {
        void* pd = env->ordinal10920(playerUnit);
        if (pd) {
            uint32_t unitId = unit ? rd32(unit, 0xC) : 0xFFFFFFFFu;
            uint8_t* e = (uint8_t*)rdptr(pd, 0x38);
            while (e && rd32(e, 0) != unitId) e = (uint8_t*)rdptr(e, 0x10);
            if (e) found = rd16(e, 4);
        }
    }
    wr16(buf, 9, (uint16_t)found);
    wr16(buf, 0xB, env->sub101D0(unit, playerUnit));
    env->send8A3E0(client, buf, 0xD);
}

// 0x8DFD0: action trampoline. EAX=unit, ECX=client, ESI=item, stack dwFlag. The client must be player
// data(unit)+0x9C or the process fatals 0x7E6; then 0x8D3D0(client, item, nAction=3, dwFlag, bGamble=0).
// 1.13c D2Game+0x8DFD0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8DFD0(void* unit, void* client, void* item, uint32_t dwFlag, const D2GameBatchB87Env* env) {
    uint32_t id = (unit && rd32(unit, 0) == 0)
        ? rd32(env->ordinal10920(unit), 0x9C) : 0;
    if (id != (uint32_t)(uintptr_t)client) d2game_fatal(0x7E6);
    env->sub8D3D0(client, item, 3, dwFlag, 0);
}

// 0x13E60: the 137F0 sibling for the second item layout. Same roll/prefix/type-0x2D/0x32 flow, but the
// prefix value is data+0x112 * 33 / 100 (min 1) and the type-0x2D stats are scaled to 75% only
// (0x9F/0xA0 via 0x13230 floors 2/1). Every path, matched or not, ends in 0x13DE0(item, arg) and answers
// 1 when the prefix pass or a type matched, else 0. Seam sub13DE0 is the unmapped 0x13DE0 (EAX=item).
// 1.13c D2Game+0x13E60
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_13E60(void* item, uint32_t arg,
        void (*sub13DE0)(void* item, uint32_t arg), const D2GameBatchB87Env* env) {
    void* g   = env->ordinal10926();
    void* rec = env->ordinal10695(item ? rd32(item, 4) : 0xFFFFFFFFu);
    if (!g || !rec) return 0;
    env->ordinal10536(item, d2_b87_roll(env->ordinal10411(item), rd32(g, 0)));
    int32_t ret = 0;
    if (env->ordinal10865(item)) {
        void* rec2 = env->ordinal10695(item ? rd32(item, 4) : 0xFFFFFFFFu);
        if (rec2) {
            int32_t v = (int32_t)(rd8(rec2, 0x112) * 0x21u) / 100;
            if (v < 1) v = 1;
            int32_t half  = (int32_t)((uint32_t)v >> 1);
            int32_t extra = (int32_t)d2_b87_roll((uint8_t*)item + 0x20, (uint32_t)half) + half;
            if (extra == 0) extra = 1;
            env->ordinal10887(item, 0x48, (uint32_t)extra, 0);
            env->ordinal10887(item, 0x49, (uint32_t)v, 0);
        }
    }
    if (env->ordinal10744(item, 0x2D)) {
        static const uint32_t stat[4] = { 0x16, 0x15, 0x18, 0x17 };
        static const int32_t floorv[4] = { 2, 1, 2, 1 };
        for (int i = 0; i < 4; i++) {
            int32_t q = d2_b87_pct((int32_t)env->ordinal10587(item, stat[i], 0));
            if (q < floorv[i]) q = floorv[i];
            env->ordinal10887(item, stat[i], (uint32_t)q, 0);
        }
        if (env->ordinal10711(item)) {
            env->ordinal10887(item, 0x9F,
                (uint32_t)GAME_PercentOfWithFloor((int32_t)env->ordinal10910(item, 0x9F, 0), 2, 0x4B), 0);
            env->ordinal10887(item, 0xA0,
                (uint32_t)GAME_PercentOfWithFloor((int32_t)env->ordinal10910(item, 0xA0, 0), 1, 0x4B), 0);
        }
        ret = 1;
    } else if (env->ordinal10744(item, 0x32)) {
        int32_t q = d2_b87_pct((int32_t)env->ordinal10587(item, 0x1F, 0));
        if (q <= 0) q = 1;
        env->ordinal10887(item, 0x1F, (uint32_t)q, 0);
        ret = 1;
    }
    sub13DE0(item, arg);
    return ret;
}
