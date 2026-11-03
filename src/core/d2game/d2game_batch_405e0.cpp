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
#include <emscripten.h>
#include "../fn/hookenv.h"

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline int16_t  rd16(const void* p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline int32_t  rd32(const void* p, uint32_t o) { int32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
// retail structs hold 32-bit pointers (the FN blobs patch 4-byte words): read 4 bytes, widen
static inline void*    rdptr(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return (void*)(uintptr_t)v; }
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
    const uint8_t* rec = unitRecord(unit);
    if (!rec) return 2;
    if (E.ord10203(rd32(unit, 0x2c))) {
        uint32_t pkt[0x17] = {};
        int32_t step = rd32(rec, 0x38);
        pkt[0] = 1;
        if (step > 0) { pkt[0] = 9; pkt[13] = (uint32_t)step; }
        pkt[1] = (uint32_t)E.sub0A20(game);
        pkt[5] = (uint32_t)E.sub21100(unit);
        pkt[6] = (uint32_t)E.sub21130(unit);
        pkt[11] = (uint32_t)E.ord10240(unit);
        pkt[12] = (uint32_t)E.ord11029(unit);
        pkt[4] = (uint32_t)(int32_t)rd16(rec, 0x18);
        E.sub8F930(game, pkt);
    }
    return E.sub5F320(game);
}

// 1.13c D2Game+0x406D0 now lives in d2game_406d0.cpp (retail convention: ECX = game, EDX = unit).

// 1.13c D2Game+0x41FB0 now lives in d2game_41fb0.cpp (the old transcription took the wrong stdcall arguments and missed the attacker block).

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
        if (!D2GAME_56640(game, nullptr)) return;
        // retail loads edx from [obj+0x18] before the call and 56640 leaves it alone: the +0x90 flag is on the obj's own pointer.
        bool off = rd8(rdptr(obj, 0x18), 0x90) == 0;
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

// function probe seam (src/core/fn/d2game_405e0.cpp): ACC4(path) gate, E0A20(eax unit, game), 1100/1130, B26A, B0FC, 6F930(ecx game, edx pkt), 3F320(game).
static HookEnv* g5H; static void* g5U;
uint32_t D2Game_405E0_probe(void* game, void* unit, const uint8_t* dt, uint32_t* lg, const uint32_t* sc)
{
    HookEnv h(lg, sc); g5H = &h; g5U = unit;
    D2GameBatch405E0Env e{};
    e.dataTables = dt;
    e.ord10203 = [](int32_t v) { return (int32_t)g5H->h(0xACC4, 0, 0, 0, 0, 0, (uint32_t)v); };
    e.sub0A20 = [](void* g) { return (int32_t)g5H->h(0xE0A20, (uint32_t)(uintptr_t)g5U, 0, 0, 0, 0, (uint32_t)(uintptr_t)g); };
    e.sub21100 = [](void* u) { return (int32_t)g5H->h(0x1100, 0, (uint32_t)(uintptr_t)u); };
    e.sub21130 = [](void* u) { return (int32_t)g5H->h(0x1130, 0, (uint32_t)(uintptr_t)u); };
    e.ord10240 = [](void* u) { return (int32_t)g5H->h(0xB26A, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)u); };
    e.ord11029 = [](void* u) { return (int32_t)g5H->h(0xB0FC, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)u); };
    e.sub8F930 = [](void* g, uint32_t* pkt) { g5H->h(0x6F930, 0, (uint32_t)(uintptr_t)g, g5H->stk(pkt)); g5H->blk(0x6F930, pkt, 23); };
    e.sub5F320 = [](void* g) { return (int32_t)g5H->h(0x3F320, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)g); };
    d2game_batch_405e0_bind(&e);
    uint32_t r = D2GAME_405E0(game, unit);
    d2game_batch_405e0_bind(nullptr);
    return r;
}

// ---- function probes for 0x56A00 / 0x56A40 / 0x56BD0 / 0x56D10 / 0x57000 / 0x57320 (src/core/fn/d2game_<rva>.cpp).
// Each seam logs through HookEnv exactly as the capture's hook recorder does and returns the scripted value; `arr`
// (the quest array 10920 returns) and `nd` (the node 33F30 returns) are blobs the descriptor passes as extra args.
static uint32_t U32(const void* p) { return (uint32_t)(uintptr_t)p; }
static void* g6Arr; static void* g6Node; static void* g6Game; static const uint32_t* g6Tbl;
static D2GameBatch405E0Env mkQuestEnv(HookEnv& h)
{
    g5H = &h;
    D2GameBatch405E0Env e{};
    e.tbl57000 = g6Tbl;
    e.ord10141 = [](void* s, void* d) { g5H->h(0xA544, 0, 0, 0, 0, 0, U32(s), U32(d)); };
    e.ord10920 = [](int32_t v) -> int32_t { g5H->h(0xA4C0, 0, 0, 0, 0, 0, (uint32_t)v); return (int32_t)(intptr_t)g6Arr; };
    e.ord10174 = [](int32_t q, int32_t t, int32_t f) -> int32_t { return (int32_t)g5H->h(0xA568, 0, 0, 0, 0, 0, (uint32_t)q, (uint32_t)t, (uint32_t)f); };
    e.ord10003 = [](int32_t q, int32_t t, int32_t v) { g5H->h(0xA57A, 0, 0, 0, 0, 0, (uint32_t)q, (uint32_t)t, (uint32_t)v); };
    e.sub53EB0 = [](void* o, int32_t a, int32_t m) { g5H->h(0x33EB0, 0, U32(o), 0, 0, 0, (uint32_t)a, (uint32_t)m); };
    e.ord10331 = [](int32_t v) -> void* { return (void*)(uintptr_t)g5H->h(0xA478, 0, 0, 0, 0, 0, (uint32_t)v); };
    e.ord10826 = [](void* p) -> void* { return (void*)(uintptr_t)g5H->h(0xA556, 0, 0, 0, 0, 0, U32(p)); };
    e.ord10459 = [](void* p) -> int8_t { return (int8_t)g5H->h(0xA56E, 0, 0, 0, 0, 0, U32(p)); };
    e.sub53F30 = [](int32_t id) -> void* { uint32_t v = g5H->h(0x33F30, U32(g6Game), (uint32_t)id); return v ? g6Node : nullptr; };
    e.isBadCodePtr = [](void*) -> int32_t { return 0; };
    e.callHandler = [](void* handler, void*) -> int32_t { return (int32_t)g5H->h(HookEnv::handlerRva(U32(handler)), 0, U32(g6Node)); };
    return e;
}
uint32_t D2Game_56A00_probe(void* game, void* pt, uint32_t* lg, const uint32_t* sc)
{
    HookEnv h(lg, sc); D2GameBatch405E0Env e = mkQuestEnv(h); d2game_batch_405e0_bind(&e);
    uint32_t r = D2GAME_56A00(game, pt); d2game_batch_405e0_bind(nullptr); return r;
}
uint32_t D2Game_56A40_probe(void* game, void* pt, int32_t* out, uint32_t* lg, const uint32_t* sc)
{
    HookEnv h(lg, sc); D2GameBatch405E0Env e = mkQuestEnv(h); d2game_batch_405e0_bind(&e);
    uint32_t r = D2GAME_56A40(game, pt, out); d2game_batch_405e0_bind(nullptr); return r;
}
uint32_t D2Game_56BD0_probe(void* obj, void* nd, uint32_t* lg, const uint32_t* sc, bool second)
{
    HookEnv h(lg, sc); g6Node = nd; g6Game = rdptr(obj, 4); D2GameBatch405E0Env e = mkQuestEnv(h); d2game_batch_405e0_bind(&e);
    uint32_t r = second ? D2GAME_56D10(obj) : D2GAME_56BD0(obj); d2game_batch_405e0_bind(nullptr); return r;
}
void D2Game_57000_probe(void* obj, void* msg, uint32_t* lg, const uint32_t* sc, void* arr, const uint32_t* tbl)
{
    HookEnv h(lg, sc); g6Arr = arr; g6Tbl = tbl; D2GameBatch405E0Env e = mkQuestEnv(h); d2game_batch_405e0_bind(&e);
    D2GAME_57000(obj, msg, rdptr(obj, 4)); d2game_batch_405e0_bind(nullptr);
}
uint32_t D2Game_57320_probe(void* game, int32_t handle, uint32_t* lg, const uint32_t* sc, void* arr)
{
    HookEnv h(lg, sc); g6Arr = arr; D2GameBatch405E0Env e = mkQuestEnv(h); d2game_batch_405e0_bind(&e);
    uint32_t r = D2GAME_57320(game, handle); d2game_batch_405e0_bind(nullptr); return r;
}
