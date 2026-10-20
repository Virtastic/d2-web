// src/core/d2game/d2game_batch_93d60.cpp -- eight 1.13c D2Game.dll functions transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c): the two ring-spawn loops 0x93D60/0x94370,
// their wrappers 0x94E60/0x94F50, and 0x95040, 0x95370, 0x954F0, 0x95FE0. Batch lane (OBJECTIVE v8.3):
// implementation + marker only, no spec, no vector.
//
// Host model: every callee outside this batch is reached through one seam, Env::call(id, a..e), where id is
// the D2 ordinal (>= 10000) or the D2Game RVA of the callee, in retail argument order. Retail's
// register-only arguments (Ghidra unaff_*) are explicit trailing parameters named at each function. Where the
// decompile reads an uninitialised stack slot (local_264, local_10) the value is a parameter or 0, marked
// ponytail. Data-table reads go through the same seam (id kDT_*) so struct layout stays out of this file.
#include <cstdint>
#include <cstddef>

namespace d2gb93d60 {
typedef intptr_t W;
struct Env { W (*call)(uint32_t id, W a, W b, W c, W d, W e); };
static Env g_env;
static inline W C(uint32_t id, W a = 0, W b = 0, W c = 0, W d = 0, W e = 0) { return g_env.call(id, a, b, c, d, e); }
enum { kDT_Count = 1, kDT_Rec = 2 /*(index) -> record ptr (0x23c stride) or 0*/, kDT_MaxLvl = 3 /*sgptDataTables+0xbd4*/,
       kDT_BaseLevel = 4 /*(count<0x119 || base==-0x271a0) ? 0 : *(base+0x272ec)*/,
       kSeedStep = 5 /*(unit): seed = lo*0x6ac690c5 + hi, 64-bit store at +0x20*/,
       kRec_0x64 = 6, kRec_0x138 = 7, kRec_0x13c = 8, kRec_0x140 = 9, kRec_0x144 = 10, kRec_0x56 = 11,
       kUnit_0x2c = 12, kUnit_0x2c_field = 13, kUnit_flags31 = 14 /*(unit, and-mask) &= mask*/, kUnit_0x20 = 15, kUnit_0x2c_b = 16 };

static const int32_t kRingTab[128] = {0,2,5,8,11,14,16,19,21,23,24,26,27,28,29,29,30,29,29,28,27,26,24,23,21,19,16,14,11,8,5,2,0,-2,-5,-8,-11,-14,-16,-19,-21,-23,-24,-26,-27,-28,-29,-29,-30,-29,-29,-28,-27,-26,-24,-23,-21,-19,-16,-14,-11,-8,-5,-2,30,29,29,28,27,26,24,23,21,19,16,14,11,8,5,2,0,-2,-5,-8,-11,-14,-16,-19,-21,-23,-24,-26,-27,-28,-29,-29,-30,-29,-29,-28,-27,-26,-24,-23,-21,-19,-16,-14,-11,-8,-5,-2,0,2,5,8,11,14,16,19,21,23,24,26,27,28,29,29};

// ring loop shared by 0x93D60 and 0x94370. level0 = 0x93D60/0x94370 `Ordinal_10637(unit, baseLevel+1)`.
static void ringLoop(W game, W unit, W p3, W p4, W p5, W p6, W p7, int32_t step, uint32_t cb, bool gate238, W seed) {
    C(10614);
    W spec[0x17] = {};
    spec[0] = 3; spec[1] = unit; spec[4] = p3; spec[5] = p4; spec[6] = p5; spec[0xb] = p6; spec[0xc] = p7;
    spec[0x15] = cb;
    uint64_t rng = (uint64_t)seed;   // ponytail: retail local_264 is uninitialised stack; caller supplies it
    for (int32_t o = 0; o < 0x100; o += step * 4) {
        spec[7] = kRingTab[0x40 + o / 4];
        spec[8] = kRingTab[o / 4];
        rng = (rng & 0xffffffffull) * 0x6ac690c5ull + (rng >> 32);
        spec[0x16] = (W)(uint32_t)rng;
        W m = C(0x8F930, game, (W)spec);
        if (gate238 ? (m != 0 && p3 == 0x238) : true) C(10637, m, C(kDT_BaseLevel) + 1);
    }
}



// 1.13c D2Game+0x93D60
void sub_93D60(W game, W unit, W p3, W p4, W p5, W p6, W p7, int32_t step, W seed = 0) {
    ringLoop(game, unit, p3, p4, p5, p6, p7, step, 0x31040, true, seed);   // cb = LAB 0x6fcb1040 (rva 0x31040)
}
// 1.13c D2Game+0x94370
void sub_94370(W game, W unit, W p3, W p4, W p5, W p6, W p7, int32_t step, W seed = 0) {
    ringLoop(game, unit, p3, p4, p5, p6, p7, step, 0x30EA0, false, seed);  // cb = LAB 0x6fcb0ea0 (rva 0x30EA0)
}

// Shared body of 0x94E60 / 0x94F50 (identical but for the ring callee). local_10 is an uninitialised slot: 0.
static W spawnWrap(W game, W unit, W id, W p4, void (*ring)(W, W, W, W, W, W, W, int32_t, W)) {
    C(kSeedStep, unit);
    if (id < 0 || id >= C(kDT_Count)) return 0;
    W rec = C(kDT_Rec, id);
    if (!rec) return 0;
    W n = C(0x91340, unit);
    if (n <= 0) return 0;
    W idv = id;
    if (!C(0x9FA30, game, (W)&idv)) return 0;
    W step = C(0x91240);
    if (!step) step = C(10786, unit, C(kRec_0x64, rec), id, p4);
    ring(game, unit, n, 0 /*local_10*/, idv, id, p4, (int32_t)step, 0);
    return 1;
}
// 1.13c D2Game+0x94E60
W sub_94E60(W game, W unit, W id, W p4) { return spawnWrap(game, unit, id, p4, sub_93D60); }
// 1.13c D2Game+0x94F50
W sub_94F50(W game, W unit, W id, W p4) { return spawnWrap(game, unit, id, p4, sub_94370); }

// 1.13c D2Game+0x95040 -- (game=ecx, unit=edx, p3); id = stack arg (unaff_retaddr), arg4 = uStack_4, ebx = unaff_EBX
W sub_95040(W game, W unit, W p3, W id, W arg4, W ebx) {
    C(0xE07A0, game);
    W t = C(10392, C(kUnit_0x2c, unit));
    if (t == unit || !t) return 0;
    if (p3 < 1) return 0;
    if (id < 0 || id >= C(kDT_Count)) return 0;
    W rec = C(kDT_Rec, id);
    if (!rec) return 0;
    W n = C(0x91340, unit);
    if (n <= 0) return 0;
    C(11169, n, p3);
    C(10786, unit, C(kRec_0x138, rec), id, arg4);
    C(0xA2800, n, unit, ebx);
    return 1;
}

// 1.13c D2Game+0x95370 -- ebx = unaff_EBX (position/params struct or 0), edi = unaff_EDI (stat-id holder; +0xa8 added)
W sub_95370(W game, W id, W ebx, W edi, W ediA8, W statId) {
    if (id < 0 || id >= C(kDT_Count)) return 0;
    W rec = C(kDT_Rec, id);
    if (!rec) return 0;
    uint32_t v = (uint32_t)(ediA8 + C(10786));
    W u = C(10871);
    if (!u) {
        W a = ebx ? C(kUnit_0x2c_b, ebx) : -1, k = ebx ? C(kUnit_0x20, ebx) : 6;
        u = C(11013, C(kUnit_0x2c, game), 2, v, k, a);
        if (!u) return 0;
        C(10785, u, ebx);
        C(10911, u, 0x30F00);            // LAB 0x6fcb0f00
        C(10807);
        C(10188, u, 0x15e, u, 0);
        C(10188, u, 0x15f, 1, 0);
    }
    C(11043, u, v);
    C(0xAEA80, ebx, ebx, 0xc, v, 0);  // FUN_6fcaea80
    W cur = C(10680, u, statId, 0);
    W nv = cur + 1 > 3 ? 3 : cur + 1;
    if (cur != nv) {
        C(10188, u, statId, nv, 0);
        W s = C(kRec_0x56, rec);
        if (s > -1 && s < C(kDT_MaxLvl)) C(10245, u, s, C(10786), 0);
        C(10945);
        C(11039);
    }
    return 1;
}

// 1.13c D2Game+0x954F0 -- p = {[0]=ctx a, [1]=ctx b, [3]=ptr to 0x1c-dword block}; game = param_2
W sub_954F0(W* p, W game) {
    uint32_t blk[0x1c];
    uint32_t* src = (uint32_t*)p[3];
    if (!C(0x95120, game, p[0], p[1])) {
        for (int i = 0; i < 0x1c; i++) blk[i] = src[i];
        C(0xDE0C0, p[0], p[1], 1, (W)blk);
        C(0xDCF20, p[0], p[1], (W)blk);
    }
    return 1;
}

// 1.13c D2Game+0x95FE0 -- (game=ecx [also scale], unit=edx, id); ebx/ebp/edi = unaff_EBX/EBP/EDI
W sub_95FE0(W game, W unit, W id, W ebx, W ebp, W edi, W unitField0xb) {
    if (id < 0 || id >= C(kDT_Count)) return 0;
    W rec = C(kDT_Rec, id);
    if (!rec) return 0;
    C(0xE07A0, game);
    W iVar5 = unitField0xb;
    W t = C(10392);
    if (t == unit || !t) return 0;
    C(0xDAA90, t);
    C(0x91B70, unit, game);
    C(0xDED30, game, unit, t);
    W a = C(10511, unit);
    if (!a) return 0;
    W lvl = C(10980, a) - 1;
    if (lvl < 0) return 0;
    C(10243, ebx, lvl);
    W iv7 = game;
    bool skip = ebp > 0;
    if (!skip) {
        W u4 = C(10973, t, 0xc, 0);
        W st = (iVar5 + game) - C(10973, t, 0xc, 0);
        if (st < 2) st = 1;
        if (game <= st) st = game;
        iv7 = edi;
        W which = 0; bool go = false;
        if (*(W*)t == 0 || C(11104, t, ebx, u4) != 0) { which = kRec_0x144; go = true; }
        else if (C(10064, 0, t) != 0) { which = kRec_0x140; go = true; }
        else if (C(0x42A80, 8) != 0) { which = kRec_0x13c; go = true; }
        if (go) {
            W r = C(10786, unit, C(which, rec), lvl, st);
            if (r >= 1) skip = true;
        }
        if (!skip) C(0x21180, unit /*param_2+8 qwords*/);
    }
    C(0x92BB0, iv7, unit, t, rec, game);
    C(0xDED30, iv7, unit, t);
    if (ebp > 0) { C(kUnit_flags31, unit, ~0x40); C(0xA35D0, 100); }
    return 1;
}
}  // namespace d2gb93d60
