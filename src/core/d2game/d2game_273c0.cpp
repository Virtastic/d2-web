// 1.13c D2Game+0x273C0: the Bash / Stun / Concentrate / Bear-smite skill handler, read off the 1.13c disassembly.
// __fastcall(ecx = pGame, edx = pUnit, nSkill, nLevel); ret 8. Called by 0xB41B0 (unit type 0) and 0x27B90.
// Retail-shaped 32-bit handles as d2game_batch_impl_a5ff4.cpp: data read by raw offset; every callee is a weak seam.
// The logged seams (E07A0, 10392, 10653, DE5A0, DDDE0) are the d2game_a5ff4_* seams of that file; the rest are d2game_273c0_*.
#include <cstdint>
#include <cstring>
#define W extern "C" __attribute__((weak))
typedef unsigned u32;

W void d2game_a5ff4_se07a0(int pGame);
W int  d2game_a5ff4_ord10392(int pPath);
W int  d2game_a5ff4_ord10653(int pUnit, int a, int b, int c);
W u32  d2game_a5ff4_sde5a0(int m, int pUnit, int pGame, int o);
W void d2game_a5ff4_sddde0(int pUnit, int cl, const u32* pEvent, int pGame, int m);

W int  d2game_273c0_room(int pUnit);                                    // 10331
W int  d2game_273c0_inTown(int pRoom);                                  // 10057
W int  d2game_273c0_eval(int pUnit, int nCalc, int nSkill, int nLvl);   // 10786
W void d2game_273c0_s9e7b0(int pUnit, int v);                           // 0x9E7B0 (edi = unit, stack = the calc-2 value)
W void d2game_273c0_sbf210(int pUnit, int nSkill, u32* pDmg, int nLvl); // 0x9F210 (eax = skill, edi = unit, stack D, level)
W int  d2game_273c0_sfaa90(int pUnit, int pGame, int m);                // 0xFAA90: the damage record of unit vs m
W int  d2game_273c0_stateList(int pUnit, int nState);                   // thunk 0xA5D4
W int  d2game_273c0_allocList(int pPool, int nType, int nGuid);         // thunk 0xA6A0 (pool, 4, 0, type, guid)
W void d2game_273c0_setState(int pList, int nState);                    // thunk 0xA5B6
W void d2game_273c0_postStat(int pUnit, int pList);                     // thunk 0xA60A (unit, list, 1)
W void d2game_273c0_setCb(int pList);                                   // thunk 0xA75A (list, 0x6FCBF7A0)
W void d2game_273c0_aura(int pUnit, int nState);                        // thunk 0xA3CA (unit, state, 1)
W void d2game_273c0_s648e0(int pRec, int pUnit, int pList, int nSkill, int nLvl); // 0x648E0 (eax = rec)

extern "C" char* sgptDataTables_exref;
static inline int dt(int off) { return *(int*)((intptr_t)*(unsigned*)sgptDataTables_exref + off); }   // the table pointer is one dword, as in retail
static inline int rd(int p, int o) { return *(int*)(intptr_t)(p + o); }
static inline int rds(int p, int o) { return *(short*)(intptr_t)(p + o); }
static inline unsigned char rd8(int p, int o) { return *(unsigned char*)(intptr_t)(p + o); }

// 1.13c D2Game+0x273C0
extern "C" int d2game_273c0_fn(int pGame, int pUnit, int nSkill, int nLvl)
{
    if (nSkill < 0 || nSkill >= dt(0xBA0)) return 0;
    int rec = dt(0xB98) + nSkill * 0x23C;
    if (rec == 0) return 0;
    d2game_a5ff4_se07a0(pGame);
    int m = d2game_a5ff4_ord10392(rd(pUnit, 0x2C));
    if (m == pUnit || m == 0) return 0;
    if (d2game_273c0_inTown(d2game_273c0_room(m)) != 0) return 0;
    d2game_273c0_s9e7b0(pUnit, d2game_273c0_eval(pUnit, rd(rec, 0x140), nSkill, nLvl));
    u32 D[0x1c];
    memset(D, 0, sizeof D);
    int o = d2game_a5ff4_ord10653(pUnit, nSkill, nLvl, 0);
    u32 f = d2game_a5ff4_sde5a0(m, pUnit, pGame, o);
    D[1] = f & 0xFFFF;
    if (f & 1) {
        D[1] = (f | (u32)(*(unsigned short*)(intptr_t)(rec + 0x12E))) & 0xFFFF;
        D[0] |= (u32)rd(rec, 0x130);
        if (rd(rec, 0x134) != 0) D[0x18] = (u32)rd(rec, 0x134);
        D[3] = (u32)d2game_273c0_eval(pUnit, rd(rec, 0x138), nSkill, nLvl);
        if (rd8(rec, 0x1DC) != 0) {
            int c = d2game_273c0_eval(pUnit, rd(rec, 0x144), nSkill, nLvl);
            D[0x1A] = (u32)c;
            if (c > 0) ((unsigned char*)&D[0x19])[1] = rd8(rec, 0x1DC);
        }
        d2game_273c0_sbf210(pUnit, nSkill, D, nLvl);
    }
    d2game_a5ff4_sddde0(pUnit, rd8(rec, 0x1A5) ? rd8(rec, 0x1A5) : 0x80, D, pGame, m);
    int dmg = d2game_273c0_sfaa90(pUnit, pGame, m);
    if (dmg != 0) *(int*)(intptr_t)(dmg + 8) += d2game_273c0_eval(pUnit, rd(rec, 0x13C), nSkill, nLvl) * 0x100;
    int st = rds(rec, 0x80);
    if (st >= 0 && st < dt(0xC4)) {
        int list = d2game_273c0_stateList(pUnit, st);
        if (list == 0) {
            list = d2game_273c0_allocList(rd(pGame, 0x1C), rd(pUnit, 0), rd(pUnit, 0xC));
            if (list == 0) return 1;
            d2game_273c0_setState(list, st);
            d2game_273c0_postStat(pUnit, list);
            d2game_273c0_setCb(list);
        }
        d2game_273c0_aura(pUnit, rds(rec, 0x80));
        d2game_273c0_s648e0(rec, pUnit, list, nSkill, nLvl);
    }
    return 1;
}
