// src/core/d2game/d2game_batch_76a50.cpp -- Stage A batch lane (OBJECTIVE v8.3): implementations for
// twelve 1.13c D2Game.dll rows transcribed from the cached Ghidra decompiles
// (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Implementation + marker only: no spec, no vector.
//
// Host model as in d2game_batch_B24D0.cpp: retail records are opaque host objects addressed by retail
// byte offsets; every callee src/ does not own is a seam on D2GameBatch76Env, named by its 1.13c
// ordinal (Ord<n>) or retail RVA (Fn<rva>) and given in retail argument order. Registers Ghidra
// failed to name (EAX/EBX/ESI/EDI) are explicit parameters. Callee argument lists Ghidra dropped are
// kept dropped (seams take none); the host binds them. Fatal path Fog #10265/#10024/_exit(-1) is
// d2game_fatal(id). 0x76A50 and 0x76B20 are byte-identical copies.
#include <cstdint>
#include <cstring>
#include <unistd.h>

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void*    rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void wr64(void* p, uint32_t o, uint64_t v) { std::memcpy((uint8_t*)p + o, &v, 8); }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

struct D2GameBatch76Env {
    void* dataTables;                                   // sgptDataTables; +0xBC4 records (0xC4 each), +0xBC8 count
    void (*hook)(int, int);                             // PTR_FUN_6fd1bc1c, may be null
    uint32_t (*bitMask)(uint32_t idx);                  // gdwBitMasks[idx]

    // D2Common ordinals
    void*    (*Ord11103)(void* unit, int);
    void*    (*Ord10392)(void* a);
    int32_t  (*Ord10494)(void* unit, int);
    int32_t  (*Ord10973)();
    int32_t  (*Ord10973s)(void* unit, uint32_t stat, uint32_t idx);
    int32_t  (*Ord10578)();
    void     (*Ord10887)();
    void     (*Ord10887p)(void* p, int v);
    void*    (*Ord10331)();
    int32_t  (*Ord10057)(void* room);
    void*    (*Ord11139)(void* a, int b);
    void     (*Ord10695)(void* a);
    void     (*Ord10910)();
    void     (*Ord10551)();
    uint32_t (*Ord11134)(void* a);
    void*    (*Ord10284)(void* a);
    void*    (*Ord10348)(void* a);
    uint32_t (*Ord10155)(void* a);
    void     (*Ord10157)(void* a, uint32_t b, int c);
    void     (*Ord11004)();
    void     (*Ord10627)();
    void*    (*Ord10920)();
    void     (*Ord10084)();
    void*    (*Ord10460)(void* inv);
    void*    (*Ord10464)(void* node);
    int32_t  (*Ord10698)(void* node);
    void*    (*Ord11147)(void* node);
    int32_t  (*Ord10744)(void* item, uint32_t type);
    int32_t  (*Ord10865)(void* item);
    int32_t  (*Ord10202)(void* item, uint32_t flags, uint32_t line, const char* file);
    void     (*Ord11090)();
    void     (*Ord10141)();
    int32_t  (*Ord11088)(void* a);
    void     (*Ord10049)(void* a);
    void     (*Ord10386)(void* a, uint32_t b);
    void*    (*Ord10045)(const char* file, uint32_t line, uint32_t zero);
    void*    (*Ord10867)(void* a);
    int8_t   (*Ord10750)(void* a);
    int32_t  (*Ord10002)();
    void     (*Ord10136)(void* a, int b);
    void*    (*Ord10708)(int32_t a);

    // D2Game-internal callees
    void*    (*Fn21320)(void* unit);                    // 0x6FC21320
    void     (*Fn95E10)(void* game, void* buf);         // 0x6FC95E10
    void     (*Fn102A0)();                              // 0x6FD102A0 (returns via Fn102A0r)
    uint32_t (*Fn102A0r)();
    void     (*FnFFA90)();
    int32_t  (*FnFFA90r)();
    void     (*Fn007A0)(void* game);                    // 0x6FD007A0
    void     (*Fn00750)();
    void     (*FnFE5A0)(void* game, int a, int b);
    void     (*FnFDDE0)(uint32_t op, void* game, void* unit);
    void     (*FnAE6F0)();                              // 0x6FCAE6F0
    void     (*Fn5740)(void* unit, int p);              // 0x6FCF5740
    void*    (*Fn31CA0)(void* room, void* a, int b, void* c);
    int32_t  (*Fn12540)(void* game, void* unit, void* a, int b, int c, int d, int e);
    void     (*Fn12380)(void* unit);
    void     (*Fn120C0)(int v);
    void     (*Fn122C0)(int v);
};

// 1.13c D2Game+0x76A50
void D2Game_76A50(D2GameBatch76Env& e, void* game, void* unit)
{
    e.Ord11103(unit, 0);
    if (unit) e.Fn21320(unit);
    e.Fn007A0(game);
    e.Ord10392(rdptr(unit, 0x2c));
    uint32_t buf[10] = {};
    e.Fn95E10(game, buf);
    if ((e.Ord10494(unit, 1) == 0 || (e.FnFFA90(), e.FnFFA90r()) != 0) && e.hook) e.hook(0, 0);
}

// 1.13c D2Game+0x76B20 (identical to 0x76A50)
void D2Game_76B20(D2GameBatch76Env& e, void* game, void* unit) { D2Game_76A50(e, game, unit); }

// 1.13c D2Game+0x77790
void D2Game_77790(D2GameBatch76Env& e, void* game, void* esiUnit)
{
    e.Fn007A0(game);
    void* p = e.Ord10392(rdptr(esiUnit, 0x2c));
    if (p != esiUnit && p) {
        uint32_t zero[0x1c] = {};
        (void)zero;
        e.FnFE5A0(game, 0, 0);
        e.FnFDDE0(0x80, game, p);
    }
}

// 1.13c D2Game+0x77A50: ediObj type at +0x10 selects the threshold test.
void D2Game_77A50(D2GameBatch76Env& e, const void* ediObj)
{
    uint32_t a = (uint32_t)e.Ord10973();
    int32_t b = e.Ord10973();
    uint32_t t = ediObj ? rd32(ediObj, 0x10) : 0;
    if (t == 1 || t == 5 || t == 6) {}
    else if (t == 2) { if ((a & 0xffffff00u) == 0) return; }
    else if (b < 1000) return;
    if ((int32_t)a < e.Ord10578()) e.Ord10887();
}

// 1.13c D2Game+0x77BB0
uint32_t D2Game_77BB0(D2GameBatch76Env& e, const void* ediUnit)
{
    if (!ediUnit) return 0;
    if (e.Ord10057(e.Ord10331()) != 0) return 1;
    int32_t idx = (int32_t)rd32(ediUnit, 4);
    const void* dt = e.dataTables;
    if (!(idx >= 0 && idx < (int32_t)rd32(dt, 0xbc8))) return 0;
    if (!((const uint8_t*)rdptr(dt, 0xbc4) + (size_t)idx * 0xc4)) return 0;
    void* inv = rdptr(ediUnit, 0x60);
    if (inv) {
        void* n = e.Ord11139(inv, 3);
        if (n) e.Ord10695(rdptr(n, 4));
        e.Ord10910();
        e.Ord10551();
        if (e.Ord10973() > 0) return 1;
        e.Ord10887();
    }
    return 0;
}

// 1.13c D2Game+0x77E70: a = param_1, unit = EAX, game = param_2
uint32_t D2Game_77E70(D2GameBatch76Env& e, const void* a, const void* unit, const void* game)
{
    if (!a) d2game_fatal(0x38c);
    if (!unit) d2game_fatal(0x38d);
    if (rd32(a, 0) != 0) d2game_fatal(0x38e);
    uint32_t id = e.Ord11134(rdptr(unit, 0x60));
    if (rd32(a, 0xc) == id) return 1;
    if (!game) d2game_fatal(0x2d3);
    const void* n = rdptr(game, 0x1120 + (id & 0x7f) * 4);
    while (n) {
        if (rd32(n, 0xc) == id) return e.Fn102A0r();
        n = rdptr(n, 0xe4);
    }
    return 0;
}

// 1.13c D2Game+0x77F00: unit = EAX, game = EBX, count = EBP (caller zero)
int32_t D2Game_77F00(D2GameBatch76Env& e, const void* unit, const void* game, int32_t count)
{
    void* inv = rdptr(unit, 0x60);
    if (!inv) return 0;
    void* it = e.Ord10284(inv);
    while (it) {
        void* next = e.Ord10348(it);
        uint32_t id = e.Ord10155(it);
        if (!game) d2game_fatal(0x2d3);
        const void* n = rdptr(game, 0x1120 + (id & 0x7f) * 4);
        bool found = false;
        for (; n; n = rdptr(n, 0xe4)) if (rd32(n, 0xc) == id) { found = true; break; }
        if (found) ++count; else e.Ord10157(inv, id, 1);
        it = next;
    }
    return count;
}

// 1.13c D2Game+0x78230: unit = EAX
void D2Game_78230(D2GameBatch76Env& e, void* unit)
{
    e.Ord10887();
    e.Ord11004();
    e.Ord10627();
    if (unit && rd32(unit, 0) == 0) {
        void* pd = e.Ord10920();
        void* c = rdptr(pd, 0x9c);
        if (c) {
            wr32(c, 0x3d4, rd32(c, 0x3d4) & 0xfffffffdu);
            e.Ord10084();
            for (const uint8_t* p = (const uint8_t*)rdptr(unit, 0xdc); p; p = (const uint8_t*)rdptr(p, 0x24))
                if (*p == 8) e.FnAE6F0();
            for (const uint8_t* p = (const uint8_t*)rdptr(unit, 0xdc); p; p = (const uint8_t*)rdptr(p, 0x24))
                if (*p == 9) e.FnAE6F0();
            e.Fn00750();
            return;
        }
    }
    d2game_fatal(0x7ba);
}

// 1.13c D2Game+0x78A90: p = ECX, unit = EBX, flag = EDI
void D2Game_78A90(D2GameBatch76Env& e, int p, void* unit, int flag)
{
    if (!unit || rd32(unit, 0) != 0) return;
    void* inv = rdptr(unit, 0x60);
    void* node = e.Ord10460(inv);
    while (node) {
        if (e.Ord10698(node) != 3) { node = e.Ord10464(node); continue; }
        void* item = e.Ord11147(node);
        if (e.Ord10744(item, 0x2d) == 0 || e.Ord10865(item) == 0 ||
            e.Ord10973s(item, 0x48, 0) > 0 ||
            e.Ord10202(item, 0x100, 0x5e8, "..\\Source\\D2Game\\PLAYER\\PlrModes.cpp") != 0) {
            node = e.Ord10464(node);
            continue;
        }
        if (flag == 0) { e.Ord11090(); flag = 1; }
        e.Fn5740(unit, p);
        node = e.Ord10460(inv);
    }
}

// 1.13c D2Game+0x78C50: a = EAX, b = EBX, n = EDI
void D2Game_78C50(D2GameBatch76Env& e, void* a, const void* b, int32_t n)
{
    if (n <= 0) return;
    e.Ord10141();
    void* room = e.Ord10331();
    void* r = e.Fn31CA0(room, a, 4, (void*)(uintptr_t)1);
    if (!r) return;
    if (e.Ord11088(r) != 4) d2game_fatal(0x24b);
    if (rd32(r, 0) == 0) e.Ord10049(r);
    e.Ord10887p(r, 0xe);
    e.Ord10386(r, b ? rd32(b, 0xc) : 0xffffffffu);
}

// 1.13c D2Game+0xA6AB0: arg = ECX/param_1, eax = EAX, ebx = EBX, esi/edi units
int32_t D2Game_A6AB0(D2GameBatch76Env& e, int arg, const void* eax, uint32_t ebx, void* esi, void* edi)
{
    if (esi && (esi == eax || esi == edi)) return 0;
    void* r = e.Ord10708(-1);
    if (!r) return 0;
    if (esi) {
        ebx = rd32(esi, 0xc4) >> 2 & 1;
        if (ebx == 0) return 0;
        if (e.Ord10057(e.Ord10331()) != 0) return 0;
    }
    if (e.Ord10002() == 1 && arg == 0)
        return e.Fn12540((void*)(uintptr_t)ebx, edi, esi, 0xf, 0, 0, 1);
    e.Ord10136(r, arg);
    e.Fn12380(edi);
    return 0;
}

// 1.13c D2Game+0xA66E0: game = ECX, unit = EDX, rec = stack param_3
void D2Game_A66E0(D2GameBatch76Env& e, void* game, void* unit, const void* rec)
{
    struct TargetNode { void* pUnit; void* a; void* next; void* b; };
    auto step = [](uint64_t s) { return (s & 0xffffffffull) * 0x6ac690c5ull + (s >> 32); };
    auto seed = [&]() { return (uint64_t)rd32(unit, 0x20) | ((uint64_t)rd32(unit, 0x24) << 32); };
    const int32_t iVar9 = (int32_t)rd32(rec, 0x18);
    const int32_t iVar1 = (int32_t)rd32(rec, 0x14);
    const void* tbl = rdptr(rec, 0x1c);
    void* tgt = rdptr(rec, 8);
    const uint32_t type = rd32(unit, 0);

    if (rd32(unit, 0xd0) == 0xb && game && (type == 0 || type == 1)) {
        void* head = rdptr(game, 0x1118);
        auto* n = (TargetNode*)e.Ord10045("..\\Source\\D2Game\\GAME\\Targets.cpp", 0xa1, 0);
        if (n) {
            *n = TargetNode{};
            n->pUnit = unit;
            std::memcpy((uint8_t*)game + 0x1118, &n, sizeof n);
            if (head) { n->next = head; ((TargetNode*)head)->b = n; }
            wr32(unit, 0xd0, 8);
        }
    }
    const uint32_t lvl = rd8(game, 0x6d);
    if (tgt) {
        if (iVar9 != 0) {
            e.Ord11103(unit, 0);
            uint32_t buf[8] = {};
            buf[0] = 4;
            uint8_t v = 100;
            void* r = e.Fn21320(unit);
            if (r && (rd8(r, 0x104) & e.bitMask(4))) v = 0x65;
            buf[5] = (uint32_t)v << 8;
            e.Fn95E10(game, buf);
            e.Fn120C0((int16_t)rd16(tbl, 0x56 + lvl * 2));
            return;
        }
        if (iVar1 < (int16_t)rd16(tbl, 0x62 + lvl * 2)) {
            uint64_t s = step(seed());
            wr64(unit, 0x20, s);
            if ((int32_t)((uint32_t)s % 100) < (int16_t)rd16(tbl, 0x5c + lvl * 2)) {
                void* o = (type == 1 && rdptr(unit, 0x14)) ? rdptr(rdptr(unit, 0x14), 0x2c) : nullptr;
                wr32(o, 0x1c, 100);
                e.Fn12540(game, unit, tgt, 0xf, 0, 0, 1);
                return;
            }
        }
    }
    int32_t px, py;
    void* sub = rdptr(unit, 0x2c);
    if (type == 2 || (type > 3 && type < 6)) px = (int32_t)rd32(sub, 0xc);
    else if (!sub) px = 0;
    else px = (int32_t)(uintptr_t)e.Ord10867(sub);
    if (type == 2 || (type > 3 && type < 6)) py = (int8_t)rd32(sub, 0x10);
    else if (!sub) py = 0;
    else py = e.Ord10750(sub);

    uint64_t u6 = step(seed()), u5 = step(u6);
    wr64(unit, 0x20, u5);
    if (e.Fn12540(game, unit, nullptr, 2, (int32_t)((uint32_t)u6 % 0x14) - 0x28 + px,
                  (int8_t)((uint32_t)u5 % 0x14) - 10 + py, 1) != 0) return;

    u5 = step(seed()); u6 = step(u5);
    uint64_t l7 = step(u6);
    wr64(unit, 0x20, l7);
    if (e.Fn12540(game, unit, nullptr, 2, (int32_t)((uint32_t)u6 % 0x14) - 10 + px,
                  (int8_t)((uint32_t)l7 % 0x14) + (((u5 & 1) != 0) * 2 - 1) * -0x1e - 10 + py, 1) != 0) return;

    u6 = step(seed()); l7 = step(u6);
    wr64(unit, 0x20, l7);
    if (e.Fn12540(game, unit, nullptr, 2, (int32_t)((uint32_t)u6 % 0x14) - 10 + px,
                  (int8_t)((uint32_t)l7 % 0x14) + (((u5 & 1) == 0) * 2 - 1) * -0x1e - 10 + py, 1) != 0) return;
    e.Fn122C0(0xf);
}
