// src/core/d2game/d2game_batch_405e0.cpp -- twelve 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles, in RVA order. Batch lane: implementation + marker only, no spec/vector/
// harness. Same conventions as d2game_batch_10160.cpp: record fields are retail byte offsets, register-only
// inputs (in_EAX / unaff_E*) are explicit parameters, callees outside the batch are seams on
// D2GameBatch405E0Env in retail argument order.
//
// Contents: 0x405E0/0x406D0 send a unit's position packet (0x17-dword frame) after a data-table gate;
// 0x41FB0 drains eight stat pools into a durability-like stat; 0x51E70 spawns the twelve table entries
// via 0x70E60 and aims them; 0x56600..0x56A40 and 0x57000/0x57320 read the per-game node list at
// game+0x10F4 (type 0x21/0x23/0x24 nodes, next link at +0xF4); 0x56BD0/0x56D10 dispatch a node handler.
// Decompiler artifacts (register reuse in 0x41FB0, a null-record write in 0x51E70) are resolved to the
// sane reading and noted where they occur.
#include <cstdint>
#include <cstring>
#include <unistd.h>

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline int16_t  rd16(const void* p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline int32_t  rd32(const void* p, uint32_t o) { int32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void*    rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void     wr8 (void* p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }
static inline void     wr32(void* p, uint32_t o, int32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

static const char kModuleName[] = "D2Game.dll";  // 0x6FD19FF9

struct D2GameBatch405E0Env {
    const void* dataTables;                         // *sgptDataTables
    const int32_t* tbl41FB0;                        // 0x6FD1C3E8: 8 (offset, flag) pairs
    const int32_t* tbl51E70;                        // 0x6FD1C17C: 12 offsets
    const uint32_t* tbl57000;                       // 0x6FD2D8A4: 4 entries
    int32_t (*ord10203)(int32_t v);
    int32_t (*sub0A20)(void* game);                 // 0xD00A20
    int32_t (*sub21100)(void* unit);
    int32_t (*sub21130)(void* unit);
    int32_t (*ord10240)(void* unit);
    int32_t (*ord11029)(void* unit);
    int32_t (*ord11018)(void* unit);
    int32_t (*ord10818)(void* unit);
    void    (*sub8F930)(void* game, uint32_t* pkt);
    int32_t (*sub5F320)(void* game);
    int32_t (*ord10985)(void* unit);
    int32_t (*ord10983)(int32_t v);
    int32_t (*ord10764)(int32_t v);
    void    (*ord10637)(void* unit, int32_t v);
    void    (*ord10019)(void* unit, int32_t v);
    // 0x41FB0
    void*   (*ord10871)(void* unit, int32_t state);
    int32_t (*ord10786)(void* unit, int32_t stat, void* extra);
    int32_t (*ord10973)(void* unit, int32_t stat);
    int32_t (*ord11104)(void* unit);
    int32_t (*sub214D0)(int32_t a, int32_t b);
    void    (*ord10887)(void* unit, int32_t stat, int32_t v, int32_t z);
    void    (*ord11111)(void* unit, void* statList);
    void    (*ord11108)(void* statList);
    void    (*ord10945)(void* unit, int32_t state, int32_t z);
    // 0x51E70
    int32_t (*subBFA30)(void* game, int32_t* out);
    void*   (*sub70E60)(void* game, void* unit, int32_t entry, int32_t arg);
    void    (*sub40CF0)(void);
    void    (*ord10210)(void* spawned, void* unit);
    int32_t (*ord10750)(void* p);
    int32_t (*ord10867)(void* p);
    int32_t (*ord10493)(int32_t a, int32_t b, void* first, int32_t ebx);
    void    (*ord10706)(void* p, int32_t dir);
    // 0x56A00..0x57320
    void    (*ord10141)(void* src, void* dst);
    void*   (*sub53F30)(int32_t id);
    int32_t (*isBadCodePtr)(void* fn);
    int32_t (*callHandler)(void* handler, void* obj);
    uint32_t (*ord10265)(uint32_t id);
    void    (*ord10024)(const char* module, uint32_t id);
    int32_t (*ord10920)(int32_t v);
    int32_t (*ord10174)(int32_t quest, int32_t table, int32_t flag);
    void    (*ord10003)(int32_t quest, int32_t table, int32_t v);
    void    (*sub53EB0)(void* obj, int32_t arg, int32_t msg);
    void*   (*ord10331)(int32_t v);
    void*   (*ord10826)(void* p);
    int8_t  (*ord10459)(void* p);
};
static D2GameBatch405E0Env* g_env;
void d2game_batch_405e0_bind(D2GameBatch405E0Env* e) { g_env = e; }
#define E (*g_env)

static void* nodeOfType(void* game, int32_t type)
{
    void* list = rdptr(game, 0x10f4);
    if (!list) return nullptr;
    for (void* n = rdptr(list, 0); n; n = rdptr(n, 0xf4))
        if (rd32(n, 0) == type) return n;
    return nullptr;
}

static const uint8_t* unitRecord(void* unit)
{
    if (!unit) return nullptr;
    int32_t i = rd32(unit, 4);
    if (i < 0 || i >= rd32(E.dataTables, 0xb6c)) return nullptr;
    const uint8_t* r = (const uint8_t*)rdptr(E.dataTables, 0xb64) + (size_t)i * 0x1a4;
    return rd16(r, 0x18) ? r : nullptr;
}

// 1.13c D2Game+0x405E0 -- game in ECX, unit in EDX.
uint32_t D2GAME_405E0(void* game, void* unit)
{
    if (!unitRecord(unit)) return 2;
    if (E.ord10203(rd32(unit, 0x2c))) {
        uint32_t pkt[0x17] = {};
        E.sub0A20(game); E.sub21100(unit); E.sub21130(unit); E.ord10240(unit); E.ord11029(unit);
        E.sub8F930(game, pkt);
    }
    return E.sub5F320(game);
}

// 1.13c D2Game+0x406D0 -- ecxArg in ECX, unit in EDX, game = unaff_EBX, esiArg = unaff_ESI.
uint32_t D2GAME_406D0(uint32_t ecxArg, void* unit, void* game, int32_t esiArg)
{
    const uint8_t* rec = unitRecord(unit);
    if (!rec) return 2;
    uint32_t v = (uint32_t)rd32(unit, 0x2c);
    if (v == 0) return 2;
    int32_t a = E.sub21100(unit);
    E.sub21130(unit);
    if (E.ord10985(unit) < 2) {
        int32_t b = E.ord10983(rd32(unit, 0x2c));
        int32_t c = E.ord10764(rd32(unit, 0x2c));
        E.ord10637(unit, -(c - esiArg));
        E.ord10019(unit, b - a);
        v = ecxArg;
    }
    if (E.ord10203((int32_t)v)) {
        uint32_t p[0x17] = {};
        p[0] = 0xb;
        p[1] = (uint32_t)E.sub0A20(game);
        p[5] = ecxArg;
        E.ord10240(unit);
        p[11] = (uint32_t)E.ord11029(unit);
        p[2] = (uint32_t)(int32_t)rd16(rec, 0x18);
        p[12] = (uint32_t)rd32(rec, 0x38);
        E.ord11018(unit);
        p[4] = (uint32_t)E.ord10818(unit);
        E.sub8F930(game, p);
        p[2] = (uint32_t)-E.ord11018(unit);
        p[9] = (uint32_t)-E.ord10818(unit);
        E.sub8F930(game, p);
    }
    return E.sub5F320(game);
}

// 1.13c D2Game+0x41FB0 -- the second use of param_4 as a pointer is `attacker` (separate register in
// retail); the decompile's param_1 = iVar3 reassignments are register reuse and read as `unit`.
uint32_t D2GAME_41FB0(void* unit, int32_t, uint8_t* pools, int32_t recIdx, void* attacker)
{
    if (!unit || !pools) return 0;
    if (recIdx < 0 || recIdx >= rd32(E.dataTables, 0xba0)) return 0;
    const uint8_t* rec = (const uint8_t*)rdptr(E.dataTables, 0xb98) + (size_t)recIdx * 0x23c;
    int16_t state = rd16(rec, 0x80);
    if (state < 0 || state >= rd32(E.dataTables, 0xc4) || !E.ord10871(unit, state)) return 0;
    int32_t statA = rd32(rec, 0x138);
    if (E.ord10786(unit, statA, nullptr) <= 0) return 0;
    int32_t cur = E.ord10973(unit, 8);
    int32_t n = E.ord10786(unit, rd32(rec, 0x13c), attacker);
    if (n < 1) n = 1;
    bool all = true;
    if (attacker && (rd32(attacker, 0) != 1 || E.ord11104(attacker) != 0)) all = false;
    for (int i = 0; i < 8; ++i) {
        int32_t off = E.tbl41FB0[i * 2], flag = E.tbl41FB0[i * 2 + 1];
        if (flag != 0 && !all) continue;
        int32_t pool = rd32(pools, off);
        if (pool <= 0) continue;
        int32_t take = E.sub214D0(100, pool), cap = E.sub214D0(n, cur);
        if (cap < take) take = cap;
        wr32(pools, off, pool - take);
        cur -= E.sub214D0(0x10, take);
        if (cur < 1) cur = 0;
    }
    if (cur < 1) cur = 0;
    E.ord10887(unit, 8, cur, 0);
    if (cur < 1) {
        void* sl = E.ord10871(unit, rd16(rec, 0x80));
        if (sl) { E.ord11111(unit, sl); E.ord11108(sl); return 1; }
        E.ord10945(unit, rd16(rec, 0x80), 0);
    }
    return 1;
}

// 1.13c D2Game+0x51E70 -- game in ECX, unit in EDX, arg = stack param_4, ebx = unaff_EBX.
// Retail writes through a null spawned->+0x14 here (address 8..0x30); skipped as unreachable.
uint32_t D2GAME_51E70(void* game, void* unit, int32_t arg, int32_t ebx)
{
    int32_t base[2];
    if (!E.subBFA30(game, base)) return 0;
    if (unit) wr32(unit, 0xc4, rd32(unit, 0xc4) | 0x40);
    void* first = nullptr;
    for (int i = 0; i < 12; ++i) {
        void* s = E.sub70E60(game, unit, E.tbl51E70[i] + base[0], arg);
        if (!s) continue;
        int32_t type = rd32(s, 0);
        if (!first) {
            first = s;
            int32_t ownerType = unit ? rd32(unit, 0) : 6, ownerId = unit ? rd32(unit, 0xc) : -1;
            void* d = type == 1 ? rdptr(s, 0x14) : nullptr;
            if (d) { void* p = rdptr(d, 0x28); wr8(p, 8, rd8(p, 8) | 2); wr32(p, 0x2c, ownerId); wr32(p, 0x30, ownerType); }
        } else {
            int32_t ft = rd32(first, 0);
            void* d = type == 1 ? rdptr(s, 0x14) : nullptr;
            if (d) { void* p = rdptr(d, 0x28); wr32(p, 0x2c, rd32(first, 0xc)); wr32(p, 0x30, ft); }
            E.sub40CF0();
        }
        E.ord10210(s, unit);
        void* data = rdptr(s, 0x2c);
        bool direct = type == 2 || (type > 3 && type < 6);
        int32_t y = direct ? rd32(data, 0x10) : (data ? E.ord10750(data) : 0);
        int32_t x = direct ? rd32(data, 0xc) : (data ? E.ord10867(data) : 0);
        E.ord10706(data, E.ord10493(x, y, first, ebx));
    }
    return 1;
}

// 1.13c D2Game+0x56600 -- game in EAX.
bool D2GAME_56600(void* game)
{
    void* n = nodeOfType(game, 0x23);
    if (!n || !rd8(n, 9)) return false;
    return rd8(rdptr(n, 0x18), 0) == 0;
}

// 1.13c D2Game+0x56640 -- game in EAX; EDX on return is the node's data (57000 reads +0x90).
uint32_t D2GAME_56640(void* game, void** dataOut)
{
    void* n = nodeOfType(game, 0x23);
    if (!n) return 0;
    void* d = rdptr(n, 0x18);
    if (dataOut) *dataOut = d;
    return rd8(d, 0x48) != 0 && rd32(d, 0x44) == 0;
}

// 1.13c D2Game+0x56A00 -- game in ECX, point in EDX.
uint32_t D2GAME_56A00(void* game, void* pt)
{
    void* n = nodeOfType(game, 0x21);
    if (!n) return 0;
    void* d = rdptr(n, 0x18);
    E.ord10141(pt, (uint8_t*)d + 0xc4);
    wr8(d, 0xe0, 1);
    return 1;
}

// 1.13c D2Game+0x56A40 -- game in ECX, point in EDX, out = param_3.
uint32_t D2GAME_56A40(void* game, void* pt, int32_t* out)
{
    void* n = nodeOfType(game, 0x21);
    if (!n) return 0;
    void* d = rdptr(n, 0x18);
    if (rd32(d, 0x84) == 0) return 0;
    if (rd8(d, 0xb1) == 0) { E.ord10141(pt, out); return 1; }
    out[0] = rd32(d, 0xc4);
    out[1] = rd32(d, 200);
    return 1;
}

static int32_t dispatchNode(void* obj, uint32_t fatalCode, bool promote)
{
    if (rd8(obj, 0xc) != 5 && rd8(obj, 9) != 0) {
        if (promote && rd8(obj, 0xc) == 0) wr8(obj, 0xc, 1);
        return 1;
    }
    void* n = E.sub53F30(rd32(obj, 0x10));
    if (!n) return 0;
    if (E.isBadCodePtr(rdptr(obj, 0xf0))) {
        E.ord10024(kModuleName, E.ord10265(fatalCode));
        _exit(-1);
    }
    return E.callHandler(rdptr(n, 0xf0), obj);
}

// 1.13c D2Game+0x56BD0
uint32_t D2GAME_56BD0(void* obj) { return (uint32_t)dispatchNode(obj, 0x2cd, true); }

// 1.13c D2Game+0x56D10
uint32_t D2GAME_56D10(void* obj) { return (uint32_t)dispatchNode(obj, 0x272, false); }

// 1.13c D2Game+0x57000 -- obj in ECX (+4 player data, +0xC mode), msg in EDX, game = EAX at the call.
void D2GAME_57000(void* obj, void* msg, void* game)
{
    void* pdata = rdptr(obj, 4);
    int32_t arr = E.ord10920(rd32(msg, 0xc));
    int32_t quest = rd32((const void*)(intptr_t)arr, 0x10 + rd8(pdata, 0x6d) * 4);
    void* m8 = rdptr(msg, 8);
    int32_t id = m8 ? rd32(m8, 4) : -1;
    int32_t a14 = rd32(msg, 0x14);
    auto send = [&] { E.sub53EB0(obj, a14, id); };
    uint8_t mode = rd8(obj, 0xc);
    if (id > 0x218 && id < 0x21c) {
        void* d = nullptr;
        if (!D2GAME_56640(game, &d)) return;
        bool off = rd8(d, 0x90) == 0;
        if ((mode == 1 && off) || (mode == 2 && !off)) send();
        return;
    }
    if (E.ord10174(quest, 0x28, 0) == 0) {
        if (mode > 3 || E.tbl57000[mode] == 0xffffffffu || E.tbl57000[mode] > 8) return;
        send();
        return;
    }
    auto q = [&](int f) { return E.ord10174(quest, 0x28, f) != 0; };
    switch (id) {
    case 0x1ff: if (!q(4) || q(0xd)) send(); break;
    case 0x200: if (!q(9) || q(0xd)) send(); break;
    case 0x201: if (!q(6) || q(0xd)) send(); break;
    case 0x203: if (!q(8) || q(0xd)) send(); break;
    case 0x208:
        if (E.ord10174(quest, rd32(obj, 0xe0), 10) == 0 && (!q(5) || q(0xd))) send();
        break;
    case 0x209: if (q(0xd)) send(); break;
    }
}

// 1.13c D2Game+0x57320 -- obj in ECX (both the +0x6D byte and the +0x10F4 node list hang off it), handle in EDX.
uint32_t D2GAME_57320(void* pdata, int32_t unitHandle)
{
    void* game = pdata;
    int32_t arr = E.ord10920(unitHandle);
    int32_t quest = rd32((const void*)(intptr_t)arr, 0x10 + rd8(pdata, 0x6d) * 4);
    if (E.ord10174(quest, 0x28, 0) == 1 || E.ord10174(quest, 0x28, 1) == 1) return 0;
    void* r = E.ord10331(unitHandle);
    if (!r) return 0;
    void* item = E.ord10826(r);
    if (!item || E.ord10459(item) != 4) return 0;
    void* list = rdptr(game, 0x10f4);
    if (!list) return 0;
    void* n = nodeOfType(game, 0x24);
    if (!n) return 0;
    if (rd8(n, 0xc) == 2) { E.ord10003(quest, 0x28, 2); return 0; }
    if (rd8(n, 0xc) == 3) E.ord10003(quest, 0x28, 3);
    return 0;
}
