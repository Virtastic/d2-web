// 1.13c D2Game+0xA8E80 (retail-layout port; its callees go through a0830d_env, see d2game_a0830d_env.h). Probe: src/core/fn/d2game_a8e80.cpp.
#include "d2game_a0830d_env.h"

// Unit path x / y the way retail inlines them: static units read path +0xC / +0x10, dynamic ones go through
// Ord_10867 / Ord_10750; no path gives 0.
static inline bool staticPath(int* u) { return u[0] == 2 || (u[0] > 3 && u[0] <= 5); }
static inline int unitPX(int* u) { return staticPath(u) ? at<int>(P(u[0xb]), 0xc) : (u[0xb] ? (int)Ordinal_10867(u[0xb]) : 0); }
static inline int unitPY(int* u) { return staticPath(u) ? at<int>(P(u[0xb]), 0x10) : (u[0xb] ? (int)Ordinal_10750(u[0xb]) : 0); }

// Skill weapon/aura class test shared by both hands of A8E80: ids 0x3B / 0x38 always, 0x33 above level 3,
// 0x1B above level 7.
static inline bool a8e80SkillFlag(int* u, intptr_t sk, int line)
{
    bool f = false;
    int id = (int)Ordinal_10601(sk, kFile, line);
    if (id == 0x3b || id == 0x38) f = true;
    if (id == 0x33) { if ((int)Ordinal_10306(u, sk, 1) > 3) f = true; }
    else if (id == 0x1b) { if ((int)Ordinal_10306(u, sk, 1) > 7) f = true; }
    return f;
}

// 1.13c D2Game+0xA8E80  int (stack unit, target, a3, a4, pt; eax ctx), ret 0x14
// Picks one of 17 hireling actions: a weight table chosen by melee range / line of sight, adjusted, then rolled.
int D2GAME_A8E80(int* unit, int* target, uint32_t a3, int a4, int* pt, int* ctx)
{
    if (ctx[5]) return ctx[5];
    if (!target) {
        int y = unitPY(unit), x = unitPX(unit);
        if (Ordinal_10851(Ordinal_10331(unit), x, y, 2, 0x40)) return 0x10;
        seedStep(unit + 8);
        return at<uint32_t>(unit + 8, 0) % 1000 >= 1 ? 0xb : 4;
    }
    u8* game = (u8*)(intptr_t)target[0x20];
    if (!game) game = (u8*)(intptr_t)FUN_6fcff780(unit);
    int* found = nullptr;
    bool chaos = false;
    if (target[0] == 0) {
        int guid = (int)FUN_6fc57440(target);
        if (!game) __builtin_trap();   // retail asserts
        int* u = (int*)(intptr_t)at<int>(game, 0x1520 + (guid & 0x7f) * 4);
        while (u && u[3] != guid) u = (int*)(intptr_t)u[0x39];
        found = u;
        if (u && Ordinal_10826(Ordinal_10331(u)) == 0x6c && (uint32_t)Ordinal_10080(u, pt[3], pt[4]) < 0x55) chaos = true;
    }
    bool skFlag = false;
    if (intptr_t sk = Ordinal_10828(target)) skFlag |= a8e80SkillFlag(target, sk, 0x27f);
    if (intptr_t sk = Ordinal_10507(target)) skFlag |= a8e80SkillFlag(target, sk, 0x286);
    bool far1 = false, far2 = false;
    if (pt) {
        int d = (int)FUN_6fcd0980(pt[3], pt[4], target);
        if (d > 0x55) far1 = true;
        if (d > 0x69) far2 = true;
    }
    int melee = (int)Ordinal_11138(unit, target, 0);
    bool noLos = !Ordinal_10839(unit, target, 4);
    int fireRes = (int)Ordinal_10973(target, 0x27, 0);
    int lightRes = (int)Ordinal_10973(target, 0x29, 0);
    int W[17];
    bool done = false;
    if (melee) {
        static const int T1[17] = {0, 0, 40, 70, 0, 40, 24, 40, 15, 0, 0, 0, 0, 0, 0, 0, 0};
        for (int i = 0; i < 17; i++) W[i] = T1[i];
        if ((int)Ordinal_11027(target) < 0x14) W[2] += 10;
        if (Ordinal_10494(target, 0xb)) { W[7] = 0; W[8] = 0; }
        if (fireRes > lightRes) W[6] -= 10;
        if (fireRes < lightRes) W[6] += 10;
        if (!noLos) { W[5] = 0; W[6] = 0; }   // retail zeroes both when 10839 answers nonzero (ebx == 0 at 0xA922A)
        if (chaos) W[15] = 10;
        done = true;
    }
    if (!noLos) {
        if (!done) {
            static const int T2[17] = {0, 0, 0, 0, 5, 0, 25, 0, 25, 40, 0, 0, 25, 0, 0, 0, 0};
            for (int i = 0; i < 17; i++) W[i] = T2[i];
            if (a4 < 2) { W[8] -= 5; W[6] = 0; W[1] = 25; W[9] = 0; }
            if (skFlag && !far1) W[13] = 15;
            else if (far1) {
                W[13] = 25; W[1] = 0; W[12] = 15;
                if (W[9] == 0) W[9] = 0x14;
                if (a4 < 2) W[9] -= 5;
            }
            if (far2) W[14] = 0x3c;
            if (chaos) W[15] = 0x14;
        }
    } else if (!done) {
        static const int T3[17] = {0, 0, 0, 0, 0, 25, 25, 0, 15, 20, 10, 0, 20, 0, 0, 0, 0};
        for (int i = 0; i < 17; i++) W[i] = T3[i];
        if ((uint32_t)FUN_6fcd0a20(target, unit) > 0x19) { W[10] += 10; W[8] -= 5; W[5] = 0; }
        if (fireRes > lightRes) { W[6] -= 10; W[8] -= 10; }
        if (fireRes < lightRes) { W[6] += 10; W[8] += 10; }
        if (a4 < 2) W[6] -= 10;
        if (a4 > 3) W[6] += 5;
        if (a3 > 0x3c) W[9] += 10;
        Ordinal_10331(unit);
        int out[5];
        FUN_6fccf840(unit, out, game);
        if (a4 < 2 && out[3] < 2) W[9] = 0;
        if (skFlag && !far1) { W[6] += 10; W[10] = 30; W[13] = 15; }
        else if (far1) {
            W[1] = 0; W[12] = 10; W[10] = 0; W[13] = 15;
            if (W[9] == 0) W[9] = 10;
        }
        if (far2) { W[9] = 0x14; W[14] = 0x3c; }
        if (chaos) W[15] = 0xf;
    }
    if (FUN_6fd13e30(target, game, 0xc7, 0, 0, unit)) W[9] = 0;
    if (W[15] && found && !FUN_6fd13e30(found, game, 0xc7, 0, 0, unit)) W[15] = 0;
    int sum = 0;
    for (int i = 0; i < 17; i++) sum += W[i];
    uint32_t r = 0;
    if (sum > 0) {
        seedStep(unit + 8);
        uint32_t lo = at<uint32_t>(unit + 8, 0);
        r = (sum & (sum - 1)) ? lo % (uint32_t)sum : lo & (uint32_t)(sum - 1);
    }
    int acc = 0;
    for (int i = 0; i < 17; i++) { acc += W[i]; if ((int)r < acc) return i; }
    return 0xb;
}
