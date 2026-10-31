// src/core/d2game/d2game_batch_d9bf0.cpp -- 1.13c D2Game.dll item-use / unit-damage helpers transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c), register inputs recovered from the retail disassembly.
// Base 0x6FC20000.
//
// Style as d2game_batch_d62e0.cpp: register inputs (eax / ecx / edx / esi / edi / ebx at entry) are explicit
// parameters named for the register; guest addresses are uint32_t and go through env->at(). Callees we do not own
// are seams in retail argument order (register operands first, marked in the comment).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GameBatchD9BF0Env {
    void*    (*at)(uint32_t guest);                                   // guest address -> host pointer
    void     (*fatal)(uint32_t line);                                 // Ordinal_10265(line); Ordinal_10024; _exit(-1)
    // D2Common ordinals
    int32_t  (*o10973)(uint32_t unit, uint32_t stat, uint32_t layer); // stat get
    void     (*o10551)(uint32_t unit, uint32_t stat, int32_t v, uint32_t layer); // stat add
    int32_t  (*o10910)(uint32_t unit, uint32_t stat, uint32_t layer); // base-stat get
    void     (*o10887)(uint32_t unit, uint32_t stat, int32_t v, uint32_t layer);
    int32_t  (*o10174)(uint32_t unit, uint32_t a, uint32_t b);
    void     (*o10593)(uint32_t unit, uint32_t a, uint32_t b);
    void     (*o10003)(uint32_t unit, uint32_t a, uint32_t b);
    int32_t  (*o11017)(uint32_t inv);
    int32_t  (*o10478)(uint32_t item);
    int32_t  (*o11088)(uint32_t item);
    uint32_t (*o10920)(uint32_t unit);                                // player data of a player unit
    uint32_t (*o10695)(uint32_t classId);                             // item record by class
    uint32_t (*o10708)(uint32_t unit, uint32_t idx, int32_t m1);      // ecx = unit, edx = idx, stack = -1
    int32_t  (*o10353)(uint32_t item);
    uint32_t (*o10507)(uint32_t unit);
    void     (*o10252)(uint32_t item, int32_t v);
    uint32_t (*o10331)(uint32_t eaxArg);                              // register argument (eax at entry of DAB00)
    void     (*o10141)(uint32_t unit, int32_t* outXY);                // writes {x,y} (8 bytes)
    void     (*o11090)(uint32_t unit, uint32_t v);
    void     (*o10408)(uint32_t unit);
    uint32_t (*o10867)(uint32_t p);
    uint32_t (*o10750)(uint32_t p);
    uint32_t (*o10927)(uint32_t unit);
    uint32_t (*o10195)(uint32_t unit, uint32_t v);
    uint32_t (*o11131)(uint32_t v);
    uint32_t (*o10473)(uint32_t v);
    int32_t  (*o10212)(uint32_t esiUnit, uint32_t v);
    int32_t  (*o10143)(uint32_t unit);
    uint32_t (*o10237)(uint32_t classByte);
    int32_t  (*o11104)(uint32_t unit);
    void     (*o10350)(uint32_t unit, int32_t* pA, int32_t* pB, int32_t* pC, uint32_t file, uint32_t line);
    int32_t  (*o10913)(uint32_t unit);
    void     (*o10689)(uint32_t unit, uint32_t ev, uint32_t z);
    uint32_t fileSUnitDmg;                                            // guest address of "..\\Source\\D2Game\\UNIT\\SUnitDmg.cpp"
    // D2Game internals named by RVA
    int32_t  (*sub1100)(uint32_t unitEcx);                            // 0x1100
    int32_t  (*sub1130)(uint32_t unitEcx);                            // 0x1130
    int32_t  (*sub14D0)(uint32_t eaxV, uint32_t edxV, uint32_t ecx100); // 0x14D0 (eax, edx, ecx)
    int32_t  (*sub1180)(uint32_t eaxN, uint32_t ecxSeedPtr);          // 0x1180
    uint32_t (*sub1020)(uint32_t eaxUnit, uint32_t game);             // 0x1020 (eax = unit, stack = game)
    void     (*sub29F0)(uint32_t unit);                               // 0xD29F0
    void     (*sub39B0)(uint32_t eaxItem, uint32_t ecxUnit, uint32_t game); // 0xD39B0
    void     (*subDFFD0)(uint32_t code, uint32_t unit);               // 0xDFFD0
    int32_t  (*sub70D60)(uint32_t ecxUnit, uint32_t game);            // 0x70D60
    int32_t  (*subAD0F0)(uint32_t game, uint32_t item, uint32_t a1, uint32_t a2); // 0xAD0F0
    int32_t  (*subD17B0)(uint32_t esiItem);                           // 0xD17B0 (esi = item)
    void     (*sub8B010)(uint32_t eaxItem, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4); // 0x8B010
    void     (*sub8C3B0)(uint32_t alCode, uint32_t clType, uint32_t edxId, uint32_t stackV);    // 0x8C3B0
    void     (*subD8DE0)(uint32_t eaxUnit, uint32_t esiV);            // 0xD8DE0 (eax = unit, stack = value)
    void     (*sub8D570)(uint32_t eaxType, uint32_t ecxIdx, uint32_t edxId, uint32_t esiV, uint32_t stackS); // 0x8D570
    void     (*subA2100)(uint32_t ecxUnit, uint32_t edxZero, uint32_t z, int32_t m1); // 0xA2100
    void     (*sub34B20)(uint32_t eaxUnit, uint32_t code);            // 0x34B20
    void     (*sub5B0B0)(uint32_t eaxUnit, uint32_t game);            // 0x5B0B0
    void     (*subA1280)(uint32_t eaxUnit, uint32_t game);            // 0xA1280
    void     (*subEAF80)(uint32_t eaxUnit);                           // 0xEAF80
    uint32_t (*sub3A0)(uint32_t eaxV, uint32_t edxV, uint32_t ecxV);  // RVA 0xE03A0 (eax, edx, ecx)
    void     (*subDB030)(uint32_t eaxUnit, uint32_t arg);             // 0xDB030
    void     (*subDAE70)(uint32_t v);                                 // 0xDAE70
    void     (*subDB570)(uint32_t v);                                 // 0xDB570
};

template <typename E> static inline uint32_t R32(const E* e, uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (uint8_t*)e->at(a) + o, 4); return v; }
template <typename E> static inline void     W32(const E* e, uint32_t a, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)e->at(a) + o, &v, 4); }
template <typename E> static inline uint8_t  R8 (const E* e, uint32_t a, uint32_t o) { return *((uint8_t*)e->at(a) + o); }

typedef D2GameBatchD9BF0Env Env_D9;

// 1.13c D2Game+0xD9BF0 -- use an inventory item (potion / scroll / tome / key) from the cursor-less item table.
// eax = item id, ecx = unit, edx = game; stack: a1, a2 (forwarded to AD0F0), outFlag (set 0, then 1 on bad item).
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_D9BF0(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t a1, uint32_t a2, uint32_t outFlag, const Env_D9* e)
{
    const uint32_t itemId = eax, unit = ecx, game = edx;
    if (game == 0) e->fatal(0x105b);
    if (unit == 0) e->fatal(0x105c);
    W32(e, outFlag, 0, 0);
    e->sub29F0(unit);
    uint32_t item = R32(e, game, 0x1720 + (itemId & 0x7f) * 4);
    for (;;) {
        if (item == 0) { W32(e, outFlag, 0, 1); return 0; }
        if (R32(e, item, 0xC) == itemId) break;
        item = R32(e, item, 0xE4);
    }
    if (R32(e, item, 0) != 4) return 0;
    if (e->o11017(R32(e, unit, 0x60)) != 0) return 0;
    if (R32(e, item, 0x10) != 0 || e->o10478(item) == 0) { W32(e, outFlag, 0, 1); return 0; }
    if (e->o11088(item) == 0x12 && e->o10973(item, 0x46, 0) <= 0) return 0;
    if (e->sub70D60(unit, game) != 0) return 0;

    if (e->subAD0F0(game, item, a1, a2) != 0) {
        if (e->o11088(item) == 0x12) {
            int32_t idx = e->subD17B0(item);
            if (idx != -1 && e->o10973(item, 0x46, 0) > 0) {
                e->o10551(item, 0x46, -1, 0);
                uint32_t pd = (R32(e, unit, 0) == 0) ? R32(e, e->o10920(unit), 0x9c) : 0;
                int32_t v = e->o10973(item, 0x46, 0);
                e->sub8B010(item, pd, 1, 0x46, (uint32_t)v);
                uint32_t type = R32(e, item, 0), id = R32(e, item, 0xC);
                uint32_t pd2 = (R32(e, unit, 0) == 0) ? R32(e, e->o10920(unit), 0x9c) : 0;
                e->sub8C3B0(0x7c, type & 0xff, id, pd2);
                e->subD8DE0(unit, (uint32_t)idx);
                return 1;
            }
            return 1;
        }
        int32_t idx = e->subD17B0(item);
        if (idx != -1) {
            uint32_t t = e->o10708(unit, (uint32_t)idx, -1);
            if (t != 0) {
                int32_t s = e->o10353(t) - 1;
                if (s <= 0) {
                    if (e->o10507(unit) == t) e->subA2100(unit, 0, 0, -1);
                    s = 0;
                }
                e->o10252(t, s);
                uint32_t uid = R32(e, unit, 0xC), utype = R32(e, unit, 0);
                uint32_t x = (utype == 0) ? R32(e, e->o10920(unit), 0x9c) : 0;
                e->sub8D570(utype, (uint32_t)idx, uid, x, (uint32_t)s);
            }
        }
        e->sub29F0(unit);
        e->sub39B0(item, unit, game);
        return 1;
    }

    uint32_t rec = e->o10695(R32(e, item, 4));
    uint32_t pl = e->sub1020(unit, game);
    uint32_t code = R32(e, rec, 0x80);
    uint32_t mode;
    if (code == 0x20737361) {                                  // "ass "
        e->sub29F0(unit);
        if (e->o10174(pl, 9, 5) == 0) goto fail;
        e->o10593(pl, 9, 5);
        e->o10551(unit, 5, 1, 0);
        mode = 8;
    } else if (code == 0x207a7978) {                           // "xyz "
        e->sub29F0(unit);
        if (e->o10174(pl, 0x14, 5) == 0) goto fail;
        e->o10593(pl, 0x14, 5);
        e->o10551(unit, 7, 0x1400, 0);
        mode = 0x12;
    } else if (code == 0x20327274) {                           // "tr2 "
        e->sub29F0(unit);
        if (e->o10174(pl, 0x25, 8) == 0 || e->o10174(pl, 0x25, 7) != 0) goto fail;
        e->o10003(pl, 0x25, 7);
        e->sub5B0B0(unit, game);
        mode = 0x21;
    } else if (code == 0x20616f74) {                           // "toa "
        e->sub29F0(unit);
        e->subA1280(unit, game);
        e->subEAF80(unit);
        e->sub39B0(item, unit, game);
        e->subDFFD0(2, unit);
        return 1;
    } else {
        return 0;
    }
    e->sub34B20(unit, mode);
    e->sub39B0(item, unit, game);
    return 1;
fail:
    e->subDFFD0(0x13, unit);
    return 1;
}

// 1.13c D2Game+0xDA890 -- add one unit's base stat 0x78 to another unit's stat 0x1f, clamped at zero.
// eax = source unit, edi = destination unit.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_DA890(uint32_t eax, uint32_t edi, const Env_D9* e)
{
    int32_t esi = e->o10910(eax, 0x78, 0);
    if (esi == 0) return;
    int32_t sum = e->o10973(edi, 0x1f, 0) + esi;
    if (sum < 0) sum = 0;
    e->o10887(edi, 0x1f, sum, 0);
}

// 1.13c D2Game+0xDAB00 -- find the object (type 2, class = param_1) in the room list standing on ebx's tile and
// switch it to mode 2. eax = the register argument of Ordinal_10331, ebx = the reference unit.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_DAB00(uint32_t param_1, uint32_t eax, uint32_t ebx, const Env_D9* e)
{
    uint32_t room = e->o10331(eax);
    if (room == 0) return;
    uint32_t u = R32(e, room, 0x74);
    while (u != 0) {
        uint32_t next = R32(e, u, 0xE8);
        if (R32(e, u, 0) == 2 && R32(e, u, 4) == param_1) {
            int32_t a[2], b[2];
            e->o10141(u, a);
            e->o10141(ebx, b);
            if (a[0] == b[0] && a[1] == b[1]) {
                e->o11090(u, 2);
                e->o10408(u);
                return;
            }
        }
        u = next;
    }
}

// 1.13c D2Game+0xDAED0 -- append a nearby candidate unit to the list at param_3 (max 8). edx = candidate,
// stack arg = list (+0 = reference unit, +4.. units, +0x24.. weights, +0x44 count, +0x48 weight sum).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_DAED0(uint32_t edx, uint32_t list, const Env_D9* e)
{
    const uint32_t cand = edx;
    if (cand == 0) return;
    if (R8(e, cand, 0xC6) & 1) return;
    uint32_t t = R32(e, cand, 0);
    if (t == 0) {
        uint32_t m = R32(e, cand, 0x10);
        if (m == 0 || m == 0x11) return;
    } else {
        if (t != 1) return;
        uint32_t m = R32(e, cand, 0x10);
        if (m == 0 || m == 0xc) return;
    }
    uint32_t ref = R32(e, list, 0);
    if (ref != 0) {
        uint32_t rt = R32(e, ref, 0), sub = R32(e, ref, 0x2c);
        uint32_t rx, ry;
        if (rt == 2 || (rt > 3 && rt < 6)) rx = R32(e, sub, 0xc);
        else rx = sub ? e->o10867(sub) : 0;
        if (rt == 2 || (rt > 3 && rt < 6)) ry = R32(e, sub, 0x10);
        else ry = sub ? e->o10750(sub) : 0;
        if (rx == 0) return;
        int32_t cx = e->sub1100(cand);
        int32_t cy = e->sub1130(cand);
        if (cx == 0) return;
        int32_t dx = (int32_t)(rx - (uint32_t)cx), dy = (int32_t)(ry - (uint32_t)cy);
        if ((uint32_t)(dy * dy + dx * dx) > 0x1900) return;
    }
    uint32_t n = R32(e, list, 0x44);
    if ((int32_t)n >= 8) e->fatal(0xf22);
    W32(e, list, 0x4 + n * 4, cand);
    W32(e, list, 0x24 + n * 4, (uint32_t)e->o10973(cand, 0xc, 0));
    n = R32(e, list, 0x44);
    W32(e, list, 0x48, R32(e, list, 0x48) + R32(e, list, 0x24 + n * 4));
    W32(e, list, 0x44, n + 1);
}

// 1.13c D2Game+0xDB5E0 -- resolve a unit from a pointer pair. ebx = unit, stack: p1, p2.
// Type 0 returns ebx itself; type 1 (monster) looks the target up through D003A0 (table pointer / value from the unit's
// AI data, or the Ordinal_10195/11131/10473 route); returns the found record only when its kind field is 0.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_DB5E0(uint32_t ebx, uint32_t p1, uint32_t p2, const Env_D9* e)
{
    uint32_t t = ebx ? R32(e, ebx, 0) : 6;
    if (t == 0) return ebx;
    if (t != 1) return 0;
    uint32_t edi = 0;
    if (ebx && R32(e, ebx, 0) == 1) {
        uint32_t ai = R32(e, ebx, 0x14);
        uint32_t a = ai ? R32(e, ai, 0x28) : 0;
        if (a != 0) {
            uint32_t ecx = R32(e, a, 0x28);
            if (ecx != 0) edi = e->sub3A0(R32(e, a, 0x30), R32(e, a, 0x2c), ecx);
        }
    }
    uint32_t esi = 0;
    if (e->o10927(ebx) != 0) esi = e->o10195(ebx, 0x800);
    if (p2 != 0 && e->o10927(p2) != 0) esi = e->o10195(p2, 0x800);
    if (esi != 0) {
        uint32_t a = e->o11131(esi);
        uint32_t d = e->o10473(esi);
        edi = e->sub3A0(a, d, p1);
    }
    if (edi != 0 && R32(e, edi, 0) == 0) return edi;
    return 0;
}

// 1.13c D2Game+0xDB790 -- percentage roll against the chance from Ordinal_10212; on failure (or no
// chance) falls through to DB030. eax = gate (0 -> straight to DB030), ecx = rng owner (must be non-null),
// esi = unit whose 64-bit seed at +0x20 is advanced, stack: a1 (record with +0x70), a2 (forwarded).
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_DB790(uint32_t eax, uint32_t ecx, uint32_t esi, uint32_t a1, uint32_t a2, const Env_D9* e)
{
    if (eax == 0) { e->subDB030(esi, a2); return 0; }
    int32_t chance = e->o10212(esi, R32(e, a1, 0x70));
    if (chance < 1) { e->subDB030(esi, a2); return 0; }
    if (esi != 0 && R32(e, esi, 0) == 0 && e->o10143(esi) != 0 && R32(e, esi, 0x10) != 2) chance = chance / 3;
    uint64_t seed = ((uint64_t)R32(e, esi, 0x20) * 0x6ac690c5ull) + R32(e, esi, 0x24);
    W32(e, esi, 0x24, (uint32_t)(seed >> 32));
    W32(e, esi, 0x20, (uint32_t)seed);
    uint32_t roll = (uint32_t)seed % 100;
    if (ecx == 0) e->fatal(0xa2a);
    if ((int32_t)roll < chance) return 1;
    e->subDB030(esi, a2);
    return 0;
}

// 1.13c D2Game+0xDBA40 -- apply a damage record's life/mana/stamina parts (dmg+0x38 / +0x3c / +8 / +0x40) to the
// attacker's pools. eax = damage record, ecx = attacker, edx = game, stack = target unit.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_DBA40(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t target, const Env_D9* e)
{
    const uint32_t dmg = eax, attacker = ecx, game = edx;
    if (R32(e, dmg, 0x38) == 0 && R32(e, dmg, 0x3c) == 0) return;
    int32_t pct;
    if (attacker == 0 || R32(e, attacker, 0) != 1) {
        pct = 100;
    } else {
        uint32_t ai = R32(e, attacker, 0x14);
        if (ai == 0) return;
        uint32_t y = R32(e, ai, 0);
        if (y == 0) return;
        pct = R8(e, y + (uint32_t)R8(e, game, 0x6d), 0xa0);
        if (pct <= 0) return;
    }
    W32(e, dmg, 0x38, R32(e, dmg, 0x38) << 6);
    W32(e, dmg, 0x3c, R32(e, dmg, 0x3c) << 6);

    auto norm = [](int32_t v) { return (v + ((v >> 31) & 0x3f)) >> 6; };
    bool common = false;
    int32_t A = 0, B = 0, C = 0;
    if (target == 0) {
        A = 6; B = -1; C = 0;
    } else {
        uint32_t tt = R32(e, target, 0);
        if (tt == 0) {
            uint32_t tbl = e->o10237(R8(e, R32(e, target, 0x80), 0x6d));
            int32_t a = (int32_t)R32(e, tbl, 0x28);
            if (a != 0) W32(e, dmg, 0x38, (uint32_t)((int32_t)R32(e, dmg, 0x38) / a));
            int32_t b = (int32_t)R32(e, tbl, 0x2c);
            if (b != 0) W32(e, dmg, 0x3c, (uint32_t)((int32_t)R32(e, dmg, 0x3c) / b));
            common = true;
        } else if (tt == 1 && e->o11104(target) != 0) {
            common = true;
        } else {
            A = (int32_t)tt; B = (int32_t)R32(e, target, 4); C = (int32_t)R32(e, target, 0x10);
        }
    }
    if (!common) {
        e->o10350(target, &A, &B, &C, e->fileSUnitDmg, 0x648);
        if (A != 0) {
            int32_t v8 = e->o10973(attacker, 8, 0);
            int32_t v10 = e->o10973(attacker, 10, 0);
            int32_t x = norm((int32_t)R32(e, dmg, 0x38)), y = norm((int32_t)R32(e, dmg, 0x3c));
            W32(e, dmg, 0x38, (uint32_t)x);
            W32(e, dmg, 0x3c, (uint32_t)y);
            int32_t d8 = (int32_t)R32(e, dmg, 8), d40 = (int32_t)R32(e, dmg, 0x40);
            if (x >= d8) x = d8;
            if (y >= v8) y = v8;
            if (d40 < v10) v10 = d40;
            int32_t tot = x + y + v10;
            if (tot == 0) return;
            if (pct != 100) tot = e->sub14D0((uint32_t)pct, (uint32_t)tot, 100);
            int32_t room = e->o10913(target) - e->o10973(target, 6, 0);
            if (tot >= room) tot = room;
            if (tot <= 0) return;
            e->o10551(target, 6, tot, 0);
            e->o10689(target, 0x97, 0);
            return;
        }
    }
    int32_t ebx = (int32_t)R32(e, dmg, 8);
    bool flag = false;
    if (ebx <= 0) return;
    if (R32(e, dmg, 0x3c) != 0) {
        int32_t r = e->sub14D0(R32(e, dmg, 0x3c), (uint32_t)ebx, 100);
        if (pct != 100) r = e->sub14D0((uint32_t)pct, (uint32_t)r, 100);
        e->subDAE70((uint32_t)norm(r));
        flag = true;
    }
    uint32_t v38 = R32(e, dmg, 0x38);
    if (v38 == 0) {
        if (flag) e->o10689(target, 0x98, 0);
        return;
    }
    int32_t r = e->sub14D0(v38, (uint32_t)ebx, 100);
    if (pct != 100) r = e->sub14D0((uint32_t)pct, (uint32_t)r, 100);
    e->subDB570((uint32_t)norm(r));
    if (!flag) { e->o10689(target, 0x97, 0); return; }
    if (e->sub1180(2, target + 0x20) == 0) e->o10689(target, 0x97, 0);
    else e->o10689(target, 0x98, 0);
}
