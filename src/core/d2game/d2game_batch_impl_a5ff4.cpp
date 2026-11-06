// Batch a5ff4: 21 D2Game 1.13c functions (0x96EA0..0xBCB20) from the cached decompiles. Spec per row in
// the private spec store (one sub_<RVA> file per row). Implementation + marker only; no vector.
// Conventions as d2game_batch_6d620.cpp: retail-shaped (32-bit) data read by raw offset through int handles.
// Register-only arguments the decompiler dropped (in_EAX, unaff_E??, unaff_retaddr, in_stack_*) are explicit
// parameters named after the register. Every callee is a FILE-LOCAL weak seam (d2game_a5ff4_ prefix, one
// fixed signature each) so the file links before the callee rows exist; a call to an unlinked seam traps.
// Where Ghidra shows an Ordinal call with fewer arguments than the callee takes, the register-carried
// argument is the unit the surrounding code obviously passes; each such case is noted at the function.
#include <cstdint>
#include <cstring>
#include <unistd.h>
#define W extern "C" __attribute__((weak))
typedef unsigned u32;

// --- D2Common / Fog ordinals ---
W int d2game_a5ff4_ord10920(int pUnit);
W int d2game_a5ff4_ord10511(int pUnit);
W int d2game_a5ff4_ord10392(int pPath);
W int d2game_a5ff4_ord11138(int pUnit);
W int d2game_a5ff4_ord10653(int pUnit, int a, int b, int c);
W int d2game_a5ff4_ord10786(int pUnit, int nField, int nSkill, int nLevel);
W u32 d2game_a5ff4_ord10459(int pUnit);
W int d2game_a5ff4_ord10717(int pList);
W int d2game_a5ff4_ord10826(int pUnit);
W int d2game_a5ff4_ord10830(int pUnit);
W void d2game_a5ff4_ord11103(int pUnit, int a);
W int d2game_a5ff4_ord10494(int pUnit);
W int d2game_a5ff4_ord11085(int pUnit);
W int d2game_a5ff4_ord10421(int pRec);
W int d2game_a5ff4_ord10034(int pRec);
W void d2game_a5ff4_ord10153(int a, int b, void* pOutByte);
W u32 d2game_a5ff4_ord10397(int pInfo);
W void d2game_a5ff4_ord10137(int pInfo, int a);
W void d2game_a5ff4_ord10542(int pUnit, int a);
W void d2game_a5ff4_ord10168(int pPath, int nValue);
W void d2game_a5ff4_ord10441(int pUnit);
W void d2game_a5ff4_ord10558(int pUnit);
W void d2game_a5ff4_ord10970(int pUnit);
W void d2game_a5ff4_ord10243(int pInfo, int a);
W void d2game_a5ff4_ord10554(int pInfo, int a);
W void d2game_a5ff4_ord10336(int pPath, int a);
W void d2game_a5ff4_ord10286(int pPath, int nX, int nY);
W int d2game_a5ff4_ord10750(int pPath);
W int d2game_a5ff4_ord10867(int pPath);
W int d2game_a5ff4_ord10331(int pUnit);
W int d2game_a5ff4_ord10057(int pRoom);
W void d2game_a5ff4_ord10565(int pRoom, int nX, int nY, int a, int b);
W int d2game_a5ff4_ord10764(int pPath);
W u32 d2game_a5ff4_ord10983(int pPath);
W int d2game_a5ff4_ord10174(int pUnit, int a, int b);
W int d2game_a5ff4_ord10436(int a);
W void d2game_a5ff4_ord11154(int a, int pUnit);
W int d2game_a5ff4_ord10265(int a);
W void d2game_a5ff4_ord10024(const void* pStr, int a);
W void d2game_a5ff4_ord11129(void);

// --- D2Game seams (1.13c RVA in the name) ---
W int  d2game_a5ff4_s967b0(void);
W void d2game_a5ff4_s97b50(int a, int b, int c);
W void d2game_a5ff4_se07a0(int pGame);
W int  d2game_a5ff4_se03a0(int a, u32 b);
W u32  d2game_a5ff4_sfe5a0(int a, int b, int pUnit);
W void d2game_a5ff4_sfc530(int a, int pUnit, int b, int c, int d);
W void d2game_a5ff4_sbf210(void);
W void d2game_a5ff4_sdddde0(int a, int b, int pUnit);
W void d2game_a5ff4_s95e10(int pGame, void* pEvent);
W int  d2game_a5ff4_sffa90(int pUnit);
W unsigned char d2game_a5ff4_s42a80(int n);
W void d2game_a5ff4_s21220(void);
W int  d2game_a5ff4_sbc9a0(int a, int b);
W void d2game_a5ff4_sc00b0(void);
W void d2game_a5ff4_sbd230(int pGame, int pUnit);
W void d2game_a5ff4_sae6f0(void);
W void d2game_a5ff4_saea80(int pGame, int pUnit, int a, int b, int c, int d, int e);
W void d2game_a5ff4_s41490(void* pIn, void* pOut);
W int  d2game_a5ff4_s473c0(int a, int pUnit, int nSkill);
W void d2game_a5ff4_s97240(int pUnit);
W int  d2game_a5ff4_sbee50(int a, void* pOut, int pUnit, int b, int nSkill, int c);
W void d2game_a5ff4_sefb80(int pThis, int pGame, int pRoom, int pUnit, int a, int b);
W int  d2game_a5ff4_seddb0(int a, int pUnit);
W void d2game_a5ff4_sdfeee0(int a, int pUnit, int pUnit2, int one);
W int  d2game_a5ff4_sb7090(int pGame, int pUnit, void* pA, void* pB, int zero, void* pCb);
W void d2game_a5ff4_sb6d30(void); // address only: callback handed to 0xB7090
W void d2game_a5ff4_s41290(int pUnit, void* pRec);
W int  d2game_a5ff4_sb79b0(int pUnit, int pGame, int a, int b, int c, int pEv);
W void d2game_a5ff4_sb8070(int pUnit, int nR, int pGame, int a, int pEv);
W void d2game_a5ff4_sffcc0(int pThis, int zero);
W void d2game_a5ff4_saf5c0(int pGame);
W int  d2game_a5ff4_sd19c0(int nX, int pRoom, int a, int pUnit, int pUnit2);

extern "C" char* sgptDataTables_exref;
static inline int dt(int off) { return *(int*)(*(intptr_t*)sgptDataTables_exref + off); }
static inline int rd(int p, int o) { return *(int*)(intptr_t)(p + o); }
static inline void wr(int p, int o, int v) { *(int*)(intptr_t)(p + o) = v; }
static inline unsigned char rd8(int p, int o) { return *(unsigned char*)(intptr_t)(p + o); }
static inline int skill_rec(int id)
{
    if (id < 0 || id >= dt(0xBA0)) return 0;
    return dt(0xB98) + id * 0x23C;
}
[[noreturn]] static void fatal(int id)
{
    d2game_a5ff4_ord10024((const void*)"", d2game_a5ff4_ord10265(id));
    _exit(-1);
}
// unit position via its path: types 2, 4, 5 keep it inline at path+0xC/+0x10, else 10867/10750.
static inline bool inl(int t) { return t == 2 || (t > 3 && t < 6); }
static inline int ux(int u)
{
    int p = rd(u, 0x2C);
    return inl(rd(u, 0)) ? rd(p, 0xC) : (p == 0 ? 0 : d2game_a5ff4_ord10867(p));
}
static inline int uy(int u)
{
    int p = rd(u, 0x2C);
    return inl(rd(u, 0)) ? rd(p, 0x10) : (p == 0 ? 0 : d2game_a5ff4_ord10750(p));
}
// queue the 0x95E10 kill event (arg block of 8 words: [1] unit, [5] = 0x6400) for every live monster
// of the unit's list that passes the 0xB0510 / 0xBB3B0 filter.
static inline void queue_kill(int pGame, int m)
{
    d2game_a5ff4_ord11103(m, 0);
    int ev[8] = {0, m, 0, 0, 0, 0x6400, 0, 0};
    d2game_a5ff4_s95e10(pGame, ev);
}
// the 0xAE6F0 sweep of a unit's +0xDC list (entries of type 2) shared by 0xB2120 and 0xB8610
static inline void sweep_ae6f0(int u)
{
    for (char* p = (char*)(intptr_t)rd(u, 0xDC); p;) {
        char c = *p;
        p = (char*)(intptr_t)rd((int)(intptr_t)p, 0x24);
        if (c == 2) d2game_a5ff4_sae6f0();
    }
}

// dup of 0x96EA0: the bound definition is d2game_monstat_pets_96af0.cpp; this copy is kept for reference
// eax = nPetType, ebx = pGame (97B50's second arg), retaddr_arg = 97B50's first arg; param_1 = unit.
extern "C" void d2game_a5ff4_fn_96ea0(int param_1, int param_2, int eax, int ebx, int retaddr_arg)
{
    int pd = d2game_a5ff4_ord10920(param_1);
    if (pd == 0) fatal(0x200);
    if ((eax != 1 || param_2 == 1) && rd(pd, 0x44) != 0 && eax >= 0 && eax < dt(0xBF0)) {
        int rec = rd(rd(pd, 0x44), 0) + eax * 0xC;
        if (rec != 0) {
            int cur = rd(rec, 4);
            wr(rec, 8, param_2);
            while (param_2 < cur && 0 < rd(rec, 4) && eax != 7) {
                int r = d2game_a5ff4_s967b0();
                d2game_a5ff4_s97b50(retaddr_arg, ebx, r);
                cur = rd(rec, 4);
            }
        }
    }
}

// dup of 0x98E90: the bound definition is d2game_batch_97340.cpp   __fastcall(ecx = pGame, edx = pUnit, nSkill)
// Ordinal_10511/10392/11138 carry their unit/path in a register (pUnit, pUnit+0x2C, pUnit). The zeroed
// 0x70-byte block covers uStack_18; uStack_c lies outside it (never written, passed as 0).
extern "C" int d2game_a5ff4_fn_98e90(int param_1, int param_2, int param_3)
{
    int rec = skill_rec(param_3);
    if (rec != 0 && d2game_a5ff4_ord10511(param_2) != 0) {
        d2game_a5ff4_se07a0(param_1);
        int tgt = d2game_a5ff4_ord10392(rd(param_2, 0x2C));
        if (tgt != param_2 && tgt != 0 && d2game_a5ff4_ord11138(param_2) != 0) {
            u32 f = d2game_a5ff4_sfe5a0(0, d2game_a5ff4_ord10653(param_2, param_3, 0, 0), param_2);
            if (f & 1) {
                d2game_a5ff4_ord10786(param_2, rd(rec, 0x138), param_3, 0);
                d2game_a5ff4_sfc530(0, param_2, 1, 0, 0);
                if (rd8(rec, 0x1DC) != 0) d2game_a5ff4_ord10786(param_2, rd(rec, 0x144), param_3, 0);
                d2game_a5ff4_sbf210();
            }
            d2game_a5ff4_sdddde0(0x80, 0, tgt);
            return 1;
        }
    }
    return 0;
}

// 1.13c D2Game+0xB0510
// ebx = pGame (list table at +0xBC), retaddr_arg / stack4 = the two accepted monster class ids.
// param_1 replaces the owner filter after the first list entry (kept from the decompile).
extern "C" void d2game_a5ff4_fn_b0510(int param_1, int param_2, int param_3, int ebx, int retaddr_arg, int stack4)
{
    (void)param_2;
    u32 cls = d2game_a5ff4_ord10459(param_3);
    int head = rd(ebx, 0xBC + (cls & 0xFF) * 4);
    if (head == 0) return;
    for (int u = d2game_a5ff4_ord10717(head); u != 0; u = rd(u, 0x7C)) {
        if (param_3 == 0 || d2game_a5ff4_ord10826(u) == param_3) {
            int m = rd(u, 0x74);
            while (m != 0) {
                int next = rd(m, 0xE8);
                if (rd(m, 0) == 1 && (rd(m, 4) == stack4 || rd(m, 4) == retaddr_arg) && (rd8(m, 0xC6) & 1) == 0 &&
                    rd(m, 0x10) != 0 && rd(m, 0x10) != 0xC && d2game_a5ff4_ord10830(m) == 0)
                    queue_kill(ebx, m);
                m = next;
            }
        }
        param_3 = param_1;
    }
}

// 1.13c D2Game+0xB0840   __fastcall(ecx, edx = nId), eax = pUnit
// tbl = *sgptDataTables_exref (the monstats-2 table pointers live at +0xA78 rows and +0xA80 count).
extern "C" int d2game_a5ff4_b0840_tbl(int tbl, int param_2, int eax)
{
    if (eax == 0 || rd(eax, 0) != 1) return 0;
    int c = rd(eax, 4);
    if (c >= 0 && c < rd(tbl, 0xA80)) {
        int rec = c * 0x1A8 + rd(tbl, 0xA78);
        if (rec != 0) {
            for (int i = 0; i < 8; ++i)
                if (*(short*)(intptr_t)(rec + 0x170 + i * 2) == param_2) return 1;
        }
    }
    return 0;
}

extern "C" int d2game_a5ff4_fn_b0840(int param_1, int param_2, int eax)
{
    (void)param_1;
    return d2game_a5ff4_b0840_tbl(*(int*)sgptDataTables_exref, param_2, eax);
}

// 1.13c D2Game+0xB0B20   esi = pUnit, edi = nIndex. Ordinal_10494/11085/FFA90 take the unit in a register.
extern "C" int d2game_a5ff4_fn_b0b20(int esi, int edi)
{
    if (esi != 0 && rd(esi, 0) == 1 && edi < 0x14 && d2game_a5ff4_ord10494(esi) == 0 &&
        d2game_a5ff4_ord11085(esi) != 0) {
        if (((u32)rd(esi, 0xC4) >> 2 & 1) == 0 && d2game_a5ff4_sffa90(esi) == 0) return 0;
        if (d2game_a5ff4_s42a80(10) == 0) {
            if (edi != 0x13) {
                d2game_a5ff4_s21220();
                if (d2game_a5ff4_sbc9a0(edi, 1) == 0) return 0;
            }
            return 1;
        }
    }
    return 0;
}

// 1.13c D2Game+0xB1320   esi = pUnit (a player). 0xE03A0 takes (pd+0x28, pd+0x2C) of the player data.
extern "C" int d2game_a5ff4_fn_b1320(int param_1, int param_2, int param_3, int esi)
{
    (void)param_2;
    if (esi != 0 && rd(esi, 0) == 1 && rd(esi, 0x14) != 0) {
        int pd = rd(rd(esi, 0x14), 0x28);
        if (pd != 0 && rd(pd, 0x28) != 0) {
            int t = d2game_a5ff4_se03a0(rd(pd, 0x28), (u32)rd(pd, 0x2C));
            if (t != 0 && rd(t, 0) == 1) {
                int in[2] = {param_3, param_1};
                int out = 0;
                d2game_a5ff4_s41490(in, &out);
                return out;
            }
        }
    }
    return 0;
}

// 1.13c D2Game+0xB2120   __fastcall(ecx = pGame), esi = pUnit
extern "C" void d2game_a5ff4_fn_b2120(int param_1, int esi)
{
    if (esi == 0 || (rd8(esi, 0xC6) & 1) != 0) return;
    int t = rd(esi, 0);
    if (t == 0) {
        if (rd(esi, 0x10) == 0 || rd(esi, 0x10) == 0x11) return;
    } else {
        if (t != 1) return;
        if (rd(esi, 0x10) == 0 || rd(esi, 0x10) == 0xC) return;
    }
    if (t == 1 && d2game_a5ff4_ord10494(esi) == 0 && d2game_a5ff4_ord11085(esi) != 0 &&
        (((u32)rd(esi, 0xC4) >> 2 & 1) != 0 || d2game_a5ff4_sffa90(esi) != 0) && d2game_a5ff4_s42a80(10) == 0 &&
        (rd(esi, 0) != 1 || rd(esi, 0x14) == 0 || (rd8(rd(esi, 0x14), 0x16) & 2) == 0) &&
        d2game_a5ff4_ord11085(esi) != 0) {
        d2game_a5ff4_sc00b0();
        d2game_a5ff4_ord11129();
        d2game_a5ff4_sbd230(param_1, esi);
        sweep_ae6f0(esi);
        d2game_a5ff4_saea80(param_1, esi, 2, rd(param_1, 0xA8) + 1, 0, 0, 0);
    }
}

// 1.13c D2Game+0xB2E00   eax = pRec. Ordinal_10421/10034 take pRec in a register. unaff_EBP in the
// decompile is the out byte of Ordinal_10153 (a stack local), read back after each call.
extern "C" u32 d2game_a5ff4_fn_b2e00(int eax)
{
    if (d2game_a5ff4_ord10421(eax) == 0) {
        u32 hi = (u32)(rd(eax, 0x48) >> 8);
        int cur = rd(eax, 0x44) >> 8;
        int tab = rd(eax, 0x50);
        if (hi <= (u32)rd(tab, 8) && cur + 1 < (int)hi) {
            unsigned char b = rd8(tab, 0x10 + cur);
            for (int i = cur + 1; i < (int)hi; ++i)
                if (b != 0) return b;
            return 0;
        }
    } else {
        int end = d2game_a5ff4_ord10034(eax);
        int base = rd(eax, 0x38) >> 8;
        for (int i = base + 1; i < (end >> 8); ++i) {
            unsigned out = 0;
            d2game_a5ff4_ord10153(rd(eax, 0x30), base << 8, &out);
            if (out != 0) return out;
        }
    }
    return 0;
}

// 1.13c D2Game+0xB2FC0   __fastcall(ecx, edx = pUnit, nSkill), retaddr_arg = 10786's level argument
extern "C" int d2game_a5ff4_fn_b2fc0(int param_1, int param_2, int param_3, int retaddr_arg)
{
    (void)param_1;
    int rec = skill_rec(param_3);
    if (rec == 0) return 0;
    int info = d2game_a5ff4_ord10511(param_2);
    if (info == 0) return 0;
    int v;
    if ((d2game_a5ff4_ord10397(info) & 2) == 0) {
        v = rd(rec, 0x138);
    } else {
        d2game_a5ff4_ord10137(info, 0);
        v = rd(rec, 0x13C);
    }
    d2game_a5ff4_ord10542(param_2, d2game_a5ff4_ord10786(param_2, v, param_3, retaddr_arg));
    return 1;
}

// 1.13c D2Game+0xB32E0   eax = nSkill, ebx = pMonster. Ghidra shows 10786 with two arguments; skill and
// level were register-carried and are not recoverable, passed as 0.
extern "C" int d2game_a5ff4_fn_b32e0(int param_1, int param_2, int eax, int ebx)
{
    int rec = skill_rec(eax);
    if (rec != 0 && ebx != 0) {
        int c = rd(ebx, 4);
        if (c >= 0 && c < dt(0xB6C)) {
            int m = c * 0x1A4 + dt(0xB64);
            if (m != 0) {
                int lvl = d2game_a5ff4_ord10786(param_1, rd(rec, 0x138), 0, 0);
                if (lvl < 1) lvl = rd(m, 0x3C) - 1 + param_2;
                if (lvl < 2) lvl = 1;
                else if (lvl > 0xFE) lvl = 0xFF;
                d2game_a5ff4_ord10168(rd(ebx, 0x2C), lvl);
                d2game_a5ff4_ord10441(ebx);
                d2game_a5ff4_ord10558(ebx);
                d2game_a5ff4_ord10970(ebx);
                return 1;
            }
        }
    }
    return 0;
}

// 1.13c D2Game+0xB36A0   __fastcall(ecx = pGame, edx = pUnit)
extern "C" int d2game_a5ff4_fn_b36a0(int param_1, int param_2)
{
    int info = d2game_a5ff4_ord10511(param_2);
    if (info != 0) {
        d2game_a5ff4_se07a0(param_1);
        int m = d2game_a5ff4_ord10392(rd(param_2, 0x2C));
        if (m != param_2 && m != 0) {
            d2game_a5ff4_ord10137(info, 0);
            d2game_a5ff4_ord10243(info, rd(m, 0xC));
            d2game_a5ff4_ord10554(info, rd(m, 0));
            d2game_a5ff4_ord10336(rd(param_2, 0x2C), 0);
            int y = uy(m);
            int x = ux(m);
            d2game_a5ff4_ord10286(rd(param_2, 0x2C), x, y);
            return 1;
        }
    }
    return 0;
}

// 1.13c D2Game+0xB41B0   __fastcall(ecx = pGame, edx = pUnit, nSkill); ebx = first arg of 473C0,
// ebp = client arg of FE5A0/DDDE0, retaddr_arg = second arg of Ordinal_10653.
extern "C" int d2game_a5ff4_fn_b41b0(int param_1, int param_2, int param_3, int ebx, int ebp, int retaddr_arg)
{
    int rec = skill_rec(param_3);
    if (rec == 0) return 0;
    d2game_a5ff4_se07a0(param_1);
    int m = d2game_a5ff4_ord10392(rd(param_2, 0x2C));
    if (m == param_2 || m == 0) return 0;
    if (rd(param_2, 0) == 0) return d2game_a5ff4_s473c0(ebx, param_2, param_3);
    u32 f = d2game_a5ff4_sfe5a0(ebp, d2game_a5ff4_ord10653(param_2, retaddr_arg, param_3, 0), param_2);
    if (f & 1) {
        // rec+0x134, when non-zero, was stored into a local of the zeroed event block (dead store)
        d2game_a5ff4_s97240(param_2);
    }
    d2game_a5ff4_sdddde0(0x80, ebp, m);
    return 1;
}

// 1.13c D2Game+0xB4410   __fastcall(ecx = pGame, edx = pUnit, nSkill, param_4)
extern "C" int d2game_a5ff4_fn_b4410(int param_1, int param_2, int param_3, int param_4)
{
    if (skill_rec(param_3) == 0) return 0;
    int skill = param_3;
    int self = d2game_a5ff4_sbee50(0, &skill, param_2, 0, param_3, param_4);
    if (self < 0) return 0;
    d2game_a5ff4_se07a0(param_1);
    int tgt = d2game_a5ff4_ord10392(rd(param_2, 0x2C));
    if (tgt == param_2 || tgt == 0) {
        int path = rd(param_2, 0x2C);
        if (path == 0 || d2game_a5ff4_ord10764(path) != 2) return 0;
        u32 id = d2game_a5ff4_ord10983(path);
        if (param_1 == 0) fatal(0x2D3);
        tgt = rd(param_1, 0x1520 + (id & 0x7F) * 4);
        while (tgt != 0 && (u32)rd(tgt, 0xC) != id) tgt = rd(tgt, 0xE4);
        if (tgt == 0) return 0;
    }
    int room = d2game_a5ff4_ord10331(tgt);
    if (room != 0 && d2game_a5ff4_ord10057(room) == 0) {
        d2game_a5ff4_sefb80(self, param_1, d2game_a5ff4_ord10331(tgt), tgt, 0, 0);
        return 1;
    }
    return 0;
}

// 1.13c D2Game+0xB4760   __fastcall(ecx = pGame-like arg, edx = pUnit)
// The first Ordinal_10750 call's result is unused in retail; Ordinal_10331's extra stack arguments in the
// decompile are not part of it (it takes the unit); Ordinal_10565 takes (room, x, y, 1, 0x1000).
extern "C" int d2game_a5ff4_fn_b4760(int param_1, int param_2)
{
    int t = rd(param_2, 0);
    if (t != 2 && (t < 4 || 5 < t) && rd(param_2, 0x2C) != 0) d2game_a5ff4_ord10750(rd(param_2, 0x2C));
    int x = ux(param_2);
    int room = d2game_a5ff4_ord10331(param_2);
    if (d2game_a5ff4_sd19c0(x, room, param_1, param_2, param_2) != 0) {
        wr(param_2, 0xC4, rd(param_2, 0xC4) | 0xE);
        int y = uy(param_2);
        int x2 = ux(param_2);
        d2game_a5ff4_ord10565(d2game_a5ff4_ord10331(param_2), x2, y, 1, 0x1000);
        return 1;
    }
    return 0;
}

// 1.13c D2Game+0xB65E0   __fastcall(ecx, edx = pUnit, nSkill, param_4); ebp = first DDB0 argument
extern "C" int d2game_a5ff4_fn_b65e0(int param_1, int param_2, int param_3, int param_4, int ebp)
{
    (void)param_1;
    int rec = skill_rec(param_3);
    if (rec == 0) return 0;
    char scratch[4];
    if (d2game_a5ff4_sbee50(0, scratch, param_2, 1, param_3, param_4) >= 0 && d2game_a5ff4_ord10511(param_2) != 0) {
        wr(param_2, 0x3C, 0);
        int n = d2game_a5ff4_ord10786(param_2, rd(rec, 0x138), param_3, param_3);
        if (n > 0) {
            int s = d2game_a5ff4_seddb0(ebp, param_2);
            if (s != 0) wr(s, 0xC4, rd(s, 0xC4) | 0x4000000);
            int made = s != 0;
            int fail = 1;
            for (int i = 0; made < n && fail < 5 && i < n * 2; ++i) {
                int c = d2game_a5ff4_seddb0(8, param_2);
                if (c == 0) {
                    ++fail;
                } else {
                    wr(c, 0xC4, rd(c, 0xC4) | 0x4000000);
                    ++made;
                }
            }
        }
        d2game_a5ff4_sdfeee0(param_2, param_2, param_2, 1);
        return 1;
    }
    return 0;
}

// 1.13c D2Game+0xB7300   weighted pick. eax = rng state (64-bit LCG word at +0x20, carry at +0x24),
// param_1 = weights, edi = count. Returns the bucket the roll lands in, or 1 past the end.
extern "C" int d2game_a5ff4_fn_b7300(const int* param_1, int eax, int edi)
{
    u32 total = 0;
    for (int i = 0; i < edi; ++i) total += (u32)param_1[i];
    u32 roll;
    if ((int)total < 1) {
        roll = 0;
    } else {
        uint64_t s = (uint64_t)(u32)rd(eax, 0x20) * 0x6AC690C5ull + (uint64_t)(u32)rd(eax, 0x24);
        *(uint64_t*)(intptr_t)(eax + 0x20) = s;
        if ((total & (total - 1)) == 0) roll = (total - 1) & (u32)s;
        else roll = (u32)((s & 0xFFFFFFFFull) % total);
    }
    int acc = 0;
    for (int i = 0; i < edi; ++i) {
        acc += param_1[i];
        if ((int)roll < acc) return i;
    }
    return 1;
}

// 1.13c D2Game+0xB8610   AITHINK_Fn135_BaalCrab (AIBAAL_GetTarget, then the main skill handler)
// __fastcall(ecx = pGame, edx = pUnit, pAiTickParam)
extern "C" void d2game_a5ff4_fn_b8610(int param_1, int param_2, int* param_3)
{
    int a = 0, b = 0;
    // type-10 record in the monster's +0x14->+0x28 data list at +0x20 (circular, head = list pointer)
    auto find10 = [&]() -> int {
        if (rd(param_2, 0) == 1 && rd(param_2, 0x14) != 0) {
            int d = rd(rd(param_2, 0x14), 0x28);
            if (d != 0) {
                int head = rd(d, 0x20);
                if (head != 0) {
                    int p = rd(head, 0);
                    while (rd(p, 8) != 10 && p != head) p = rd(p, 0);
                    return rd(p, 8) == 10 ? p : 0;
                }
            }
        }
        return 0;
    };
    d2game_a5ff4_sb7090(param_1, param_2, &a, &b, 0, (void*)&d2game_a5ff4_sb6d30);
    int ev = find10();
    if (ev == 0) {
        int rec[3];
        rec[0] = 10;
        rec[1] = ux(param_2);
        rec[2] = uy(param_2);
        d2game_a5ff4_s41290(param_2, rec);
        ev = find10();
    }
    int cmd = *param_3;
    int r = d2game_a5ff4_sb79b0(param_2, param_1, cmd, a, b, ev);
    d2game_a5ff4_sb8070(param_2, r, param_1, cmd, ev);
    sweep_ae6f0(param_2);
    d2game_a5ff4_saea80(param_1, param_2, 2, rd(param_1, 0xA8) + 0x19, 0, 0, 0);
}

// 1.13c D2Game+0xB8AE0   __fastcall(ecx = pRec, edx = nSkill, param_3, param_4 = pUnit), eax supplies the
// high 24 bits of the false results.
extern "C" u32 d2game_a5ff4_fn_b8ae0(int param_1, int param_2, int param_3, int param_4, u32 eax)
{
    (void)param_3;
    u32 r = (eax & 0xFFFFFF00u) | rd8(param_1, 9);
    if (rd8(param_1, 9) != 0) {
        r = (u32)d2game_a5ff4_ord10174(param_4, 0x19, 0);
        if (r == 0) {
            if (param_2 == 0x16F) {
                r = (u32)d2game_a5ff4_ord10174(param_4, 0x19, 1);
                if (r == 1) return 1;
                if (rd8(param_1, 0xC) == 1) return (r & 0xFFFFFF00u) | 1;
            } else if (param_2 == 0x196) {
                r = (u32)d2game_a5ff4_ord10174(param_4, 0x19, 5);
                if (r == 0) return 1;
            }
        }
    }
    return r & 0xFFFFFF00u;
}

// 1.13c D2Game+0xB9030   tail fragment of the 0xBB550 frame: retaddr_unit = unaff_retaddr (the unit),
// stack69 = byte at stack+0x69 (slot index).
extern "C" void d2game_a5ff4_fn_b9030(int retaddr_unit, unsigned char stack69)
{
    if (d2game_a5ff4_ord10436(0x6D) != 0) {
        int pd = d2game_a5ff4_ord10920(retaddr_unit);
        d2game_a5ff4_ord11154(rd(pd, 0x1C + stack69 * 4), retaddr_unit);
    }
}

// 1.13c D2Game+0xBB3B0   __fastcall(ecx = pGame), eax = quest record (flag byte at +0x13)
extern "C" void d2game_a5ff4_fn_bb3b0(int param_1, int eax)
{
    unsigned char first = rd8(eax, 0x13);
    if (first != 0) return;
    *(unsigned char*)(intptr_t)(eax + 0x13) = 1;
    *(unsigned char*)(intptr_t)(rd(param_1, 0x10F4) + 0x20) = 0xC;
    d2game_a5ff4_sffcc0((int)((u32)param_1 & 0xFFFFFF00u) | first, 0);
    for (int u = d2game_a5ff4_ord10717(rd(param_1, 200)); u != 0; u = rd(u, 0x7C)) {
        if (d2game_a5ff4_ord10826(u) != 0x6C) continue;
        int m = rd(u, 0x74);
        while (m != 0) {
            int next = rd(m, 0xE8);
            if (rd(m, 0) == 1 && rd(m, 4) != 0xF3 && (rd8(m, 0xC6) & 1) == 0 && rd(m, 0x10) != 0 &&
                rd(m, 0x10) != 0xC && d2game_a5ff4_ord10830(m) == 0)
                queue_kill(param_1, m);
            m = next;
        }
    }
}
