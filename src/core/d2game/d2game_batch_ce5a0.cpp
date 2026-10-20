// src/core/d2game/d2game_batch_ce5a0.cpp -- Stage A batch lane (OBJECTIVE v8.3): implementations for
// twelve 1.13c D2Game.dll rows transcribed from the cached Ghidra decompiles
// (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Implementation + marker only: no spec, no vector.
//
// Same host model as d2game_batch_D2D90.cpp: unit/game records are opaque host objects addressed by
// retail byte offsets; callees src/ does not own are seams on D2GameCE5A0Env in retail argument order
// and named by their 1.13c ordinal. Register-only arguments Ghidra could not name (in_EAX, unaff_*,
// extraout_ECX, unaff_retaddr) are explicit parameters. The fatal path Fog #10265(id) -> #10024 ->
// _exit(-1) is d2game_fatal(id).
#include <cstdint>
#include <cstring>
#include <unistd.h>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline void*    rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

struct D2GameCE5A0Env {
    void*    dataTables;      // sgptDataTables
    uint32_t bitMask8;        // gdwBitMasks_exref[8]
    // unit-type 0..5 field helpers
    int32_t  (*ord10828)(void* pUnit, int32_t a, int32_t b, int32_t c);
    int32_t  (*ord10507)(void* pUnit, int32_t a, int32_t b, int32_t c);
    int32_t  (*ord10587)(void* pUnit, int32_t a, int32_t b, int32_t c);
    void     (*ord10887)(void* pUnit, int32_t a, int32_t b);
    int32_t  (*fnBF50)(int32_t a1, int32_t a4, uint16_t* o1, uint16_t* o2);
    int32_t  (*fnD630)(void* p4, int32_t a1, void* pUnit, uint32_t* pOut);
    void     (*fnDF30)(void* pUnit);
    void     (*fnE620)(void* pUnit);
    // 0xCF5B0
    bool     (*fnCDB0)(void* pThis, void* pUnit);
    int32_t  (*fnB690)(uint16_t a, uint16_t b);
    void     (*fn8BA0)(void* pThis, int32_t game, void* pUnit, int32_t a);
    int32_t  (*ord10867)(void* p);
    int32_t  (*ord10750)(void* p);
    // 0xD16B0 .. 0xD1B20 (unit / skill compare helpers)
    int16_t  (*ord10257)();
    int16_t  (*ord11028)();
    int16_t  (*ord11098)();
    int32_t  (*ord10744)();
    void*    (*ord11065)(uint32_t id);
    int32_t  (*ord10460)(int32_t list);
    void*    (*ord11147)(int32_t node);
    int32_t  (*ord10464)(int32_t node);
    int32_t  (*ord11088)(void* pUnit);
    int8_t   (*ord10020)(void* pUnit);
    int32_t  (*ord10973)(void* pUnit, int32_t a, int32_t b);
    int32_t  (*ord10463)(void* pUnit);
    bool     (*fnD16B0)();
    // 0xD2070
    int32_t  (*ord10106)(void* pUnit);
    int32_t  (*ord10620)(void* pUnit);
    void*    (*ord10695)(int32_t id);
    int32_t  (*fnD1A00)(int32_t ecx, int32_t b);
    // 0xD2CF0
    void*    (*ord10646)(int32_t id);
    void     (*ord10267)(int32_t id, int32_t v);
    void     (*fnFFC10)(void* pUnit);
    void     (*fn20A0)(void* pEax, void* pUnit);
    // 0xD31D0 / 0xD34B0
    int32_t  (*fnD1AA0)();
    void*    (*ord10708)();
    int32_t  (*ord10353)(void* p);
    void     (*ord10252)(void* p, int32_t v);
    void     (*ord10920)(void* pUnit);
    void     (*fn8D570)(int32_t a, uint32_t b, int8_t c);
    void     (*fnD29F0)(void* pUnit);
    int32_t  (*fnD2C70)();
    void*    (*ord10331)(void* pUnit);
    void     (*ord10885)(void* p, int32_t a, int32_t b);
    void     (*ord10408)(void* pUnit);
    void     (*ord11016)(void* pUnit);
    void     (*fnEE70)();
    void     (*ord10466)(int32_t a, int32_t b);
    void     (*ord10551)(void* pUnit, int32_t a, int32_t b, int32_t c);
    void     (*fnB010)();
    void     (*fnD31D0)(void* pUnit, void* pEax, int32_t retaddr);
};

// Shared tail of the four "bump 0x148 counter" handlers.
static int bump148(void* unit, const D2GameCE5A0Env* e)
{
    int32_t n = e->ord10587(unit, 0x148, 0, 0);
    e->ord10887(unit, 0x148, n + 1);
    return n;
}

// 1.13c D2Game+0xCE5A0 (a3/a4 double as the two out-words of FUN_CBF50)
int d2game_CE5A0_113c(int32_t a1, void* unit, uint32_t a3, uint32_t a4, const D2GameCE5A0Env* e)
{
    int r = e->fnBF50(a1, (int32_t)a4, (uint16_t*)&a3, (uint16_t*)&a4);
    if (r != 0) return r;
    if (e->ord10828(unit, 0, 0, 0) == 0) return 3;
    bump148(unit, e);
    e->ord10828(unit, 0x148, 0, 0);
    e->fnDF30(unit);
    return 0;
}

// 1.13c D2Game+0xCE7F0
int d2game_CE7F0_113c(int32_t a1, void* unit, uint32_t a3, void* a4, const D2GameCE5A0Env* e)
{
    int r = e->fnD630(a4, a1, unit, &a3);
    if (r != 0) return r;
    if (e->ord10507(unit, 0, 0, 0) == 0) return 3;
    bump148(unit, e);
    e->ord10507(unit, 0x148, 0, 0);
    e->fnE620(unit);
    return 0;
}

// 1.13c D2Game+0xCE950
int d2game_CE950_113c(int32_t a1, void* unit, uint32_t a3, void* a4, const D2GameCE5A0Env* e)
{
    int r = e->fnD630(a4, a1, unit, &a3);
    if (r != 0) return r;
    if (e->ord10828(unit, 0, 0, 0) == 0) return 3;
    bump148(unit, e);
    e->ord10828(unit, 0x148, 0, 0);
    e->fnE620(unit);
    return 0;
}

// 1.13c D2Game+0xCE9D0
int d2game_CE9D0_113c(int32_t a1, void* unit, uint32_t a3, void* a4, const D2GameCE5A0Env* e)
{
    int r = e->fnD630(a4, a1, unit, &a3);
    if (r != 0) return r;
    if (e->ord10828(unit, 0, 0, 0) == 0) return 3;
    bump148(unit, e);
    e->ord10828(unit, 0x148, 0, 1);
    e->fnE620(unit);
    return 0;
}

// 1.13c D2Game+0xCF5B0 (param_3 = packet, param_4 = packet id)
int d2game_CF5B0_113c(int32_t game, int32_t* unit, const uint8_t* pkt, int32_t id, const D2GameCE5A0Env* e)
{
    if (id != 0xd) return 3;
    void* self = rdptr(pkt, 1);
    int32_t t = unit[0];
    void* sub = rdptr(unit, 0x2c);
    bool plain = t == 2 || (t > 3 && t < 6);
    int32_t a = plain ? (int32_t)rd32(sub, 0xc) : sub ? e->ord10867(sub) : 0;
    uint16_t b = plain ? (uint16_t)rd32(sub, 0x10) : sub ? (uint16_t)e->ord10750(sub) : 0;
    int32_t r = e->fnCDB0(self, unit);
    if (r == 0 && (r = e->fnB690((uint16_t)a, b)) == 0) {
        e->fn8BA0(self, game, unit, a);
        r = 0;
    }
    return r;
}

// 1.13c D2Game+0xD16B0
bool d2game_D16B0_113c(const D2GameCE5A0Env* e)
{
    for (int i = 0; i < 3; i++) {
        if (e->ord10257() != e->ord10257()) return false;
        if (e->ord11028() != e->ord11028()) return false;
    }
    return e->ord11098() == e->ord11098();
}

// 1.13c D2Game+0xD17B0
uint32_t d2game_D17B0_113c(const D2GameCE5A0Env* e)
{
    uint32_t r = 0xffffffff;
    if (e->ord10744() != 0) {
        uint32_t id = (uint16_t)e->ord11028();
        void* rec = e->ord11065(id);
        if (rec) return rd32(rec, 0xc);
        d2game_fatal(0xfa5);
    }
    if (e->ord10744() != 0) {
        void* rec = e->ord11065((uint16_t)e->ord11028());
        if (!rec) d2game_fatal(0xfac);
        r = rd32(rec, 8);
    }
    return r;
}

// 1.13c D2Game+0xD1B20 (EDI = in/out record: [0x18] list head, [0] receives the surplus)
void* d2game_D1B20_113c(int32_t* edi, const D2GameCE5A0Env* e)
{
    if (e->ord10744() == 0 || edi[0x18] == 0) return nullptr;
    int32_t node = e->ord10460(edi[0x18]);
    for (;;) {
        if (node == 0) return nullptr;
        void* u = e->ord11147(node);
        node = e->ord10464(node);
        if (!u || e->ord11088(u) != 0x12) continue;
        if (e->ord10020(u) != 0) continue;
        if (!e->fnD16B0()) continue;
        int32_t a = e->ord10973(u, 0x46, 0);
        if (a < 0) a = 0;
        int32_t b = e->ord10463(u);
        if (b <= a) continue;
        edi[0] = b - a;
        return u;
    }
}

static bool pairIs(uint32_t x, uint32_t y, uint32_t a, uint32_t b) { return x == a && y == b; }

// 1.13c D2Game+0xD2070 (EAX = list head node, EBX = unit-record host with +0x12a / +0x80)
uint32_t d2game_D2070_113c(int32_t* unit, int32_t eax, const uint8_t* ebx, int32_t ecx1, int32_t ecx2,
                           const D2GameCE5A0Env* e)
{
    int32_t want = -1;
    if (unit && unit[0] == 4 && e->ord10106(unit) == 7) {
        int32_t k = e->ord10620(unit);
        if (k >= 0 && k < (int32_t)rd32(rdptr(e->dataTables, 0), 0xc28)) {
            const uint8_t* rec = (const uint8_t*)rdptr(rdptr(e->dataTables, 0), 0xc24) + (size_t)k * 0x14c;
            if (rec && (e->bitMask8 & rec[0x2c]) != 0) {
                int16_t v; std::memcpy(&v, rec, 2); want = v;
            }
        }
    }
    while (eax != 0) {
        int32_t* p = (int32_t*)e->ord11147(eax);
        eax = e->ord10464(eax);
        if (p == unit) break;
        if (!p || p[0] != 4) continue;
        if (want > -1 && e->ord10020(p) != 1 && e->ord10106(p) == 7 && e->ord10620(p) == want)
            return 1;
        const uint8_t* o = (const uint8_t*)e->ord10695(p[1]);
        if (rd8(o, 0x12a) == 0 || e->ord10020(p) == 1 || rd8(o, 0x12a) != rd8(ebx, 0x12a)) continue;
        uint32_t i = rd32(ebx, 0x80), j = rd32(o, 0x80);
        if (i == j ||
            pairIs(i, j, 0x2034336a, 0x20343367) || pairIs(i, j, 0x20343367, 0x2034336a) ||
            pairIs(i, j, 0x20736b62, 0x20646b62) || pairIs(i, j, 0x20646b62, 0x20736b62) ||
            pairIs(i, j, 0x20333364, 0x20333367) || pairIs(i, j, 0x20333367, 0x20333364) ||
            pairIs(i, j, 0x20747368, 0x2066736d) || pairIs(i, j, 0x2066736d, 0x20747368) ||
            pairIs(i, j, 0x20747368, 0x20706976) || pairIs(i, j, 0x20706976, 0x20747368) ||
            pairIs(i, j, 0x20326671, 0x20316671) || pairIs(i, j, 0x20316671, 0x20326671) ||
            e->fnD1A00(ecx1, 0x20726871) != 0 || e->fnD1A00(ecx2, 0x20796571) != 0 ||
            e->fnD1A00(ecx2, 0x20726271) != 0)
            return 1;
    }
    return 0;
}

// 1.13c D2Game+0xD2CF0 (param_1 = game, EAX = this for FUN_D20A0, ESI = unit)
void d2game_D2CF0_113c(const uint8_t* game, void* eax, int32_t* esi, const D2GameCE5A0Env* e)
{
    if (esi) esi[0x31] &= ~2;
    int32_t id = (int32_t)rd32(game, 0x60);
    int32_t* found = (int32_t*)e->ord10646(id);
    if (!found) d2game_fatal(0x12ea);
    if (esi != found) d2game_fatal(0x12eb);
    int32_t t = esi[0];
    void* sub = rdptr(esi, 0x2c);
    int32_t v = (t == 2 || (t > 3 && t < 6)) ? (int32_t)rd32(sub, 0xc) : sub ? e->ord10867(sub) : 0;
    e->ord10267(id, v);
    e->fnFFC10(esi);
    e->fn20A0(eax, esi);
}

// 1.13c D2Game+0xD31D0 (param_1 = unit, EAX = nonzero guard, retaddr = stack word read as an addend)
void d2game_D31D0_113c(int32_t* unit, int32_t eax, int32_t retaddr, const D2GameCE5A0Env* e)
{
    if (!unit) d2game_fatal(0x203);
    if (eax == 0) d2game_fatal(0x204);
    if (e->fnD1AA0() == -1) return;
    void* s = e->ord10708();
    if (!s) return;
    int32_t b = e->ord10353(s);
    e->ord10252(s, b + retaddr);
    uint32_t id = (uint32_t)unit[3];
    if (unit[0] == 0) e->ord10920(unit);
    e->fn8D570(-1, id, (int8_t)(b + retaddr));
}

// 1.13c D2Game+0xD34B0 (param_1 = game, EAX = unit id, param_2 = player record, param_4 = out flag)
uint32_t d2game_D34B0_113c(uint8_t* game, uint32_t eax, int32_t* player, uint32_t* out, int32_t edi,
                           const D2GameCE5A0Env* e)
{
    if (!game) d2game_fatal(0x144f);
    if (!player) d2game_fatal(0x1450);
    e->fnD29F0(player);
    *out = 0;
    auto find = [&](uint32_t id) -> int32_t* {
        int32_t* p = (int32_t*)rdptr(game, 0x1720 + (id & 0x7f) * 8);   // pointer-sized slot
        while (p && (uint32_t)p[3] != id) p = (int32_t*)rdptr(p, 0xe4);
        return p;
    };
    int32_t* a = find(eax);
    if (!a) { *out = 1; return 0; }
    if (a[0] != 4) return 0;
    if (!((a[4] == 3 || a[4] == 4) && e->ord10744() != 0)) { *out = 1; return 0; }
    out = (uint32_t*)player;                       // decompile reuses param_4 for the player word
    int32_t* b = find((uint32_t)(uintptr_t)game);  // retail looks up its own param_1 value
    if (!b) { *(uint32_t*)player = 1; return 0; }
    if (b[0] != 4) return 0;
    if (!(b[4] == 0 && e->ord11088(b) == 0x12)) { *(uint32_t*)player = 1; return 0; }
    if (player[0x18] == 0) d2game_fatal(0x1482);
    if (e->ord11028() != e->ord11028()) d2game_fatal(0x149d);
    int32_t cur = e->ord10973(b, 0x46, 0);
    if (e->ord10463(b) <= cur) return 0;
    if (e->fnD2C70() == 0) {
        void* h = e->ord10331(a);
        if (h) { e->ord10885(h, a[0], a[3]); e->ord10408(a); e->ord11016(a); }
        a[0x31] &= ~2;
        e->fnEE70();
        e->ord10466(edi, 0);
    }
    e->ord10551(b, 0x46, 1, 0);
    if (player[0] == 0) e->ord10920(player);
    e->ord10973(b, 0x46, 0);
    e->fnB010();
    e->fnD31D0(player, nullptr, 0);
    return 1;
}
