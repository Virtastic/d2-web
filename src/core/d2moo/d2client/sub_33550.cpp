// Prior art: none (D2MOO has no D2Client). Derived from the 1.13c disassembly of D2Client+0x33550 and its sibling 0x33B50.
// Env-seam style, as sub_33290.cpp: sgptDataTables, the string lookup (D2Lang 10003), D2Common 10708 (skill of a unit) and 10306 (its level),
// FUN_6fae16c0 (0x316C0), 0x2C8C0 and the D2Win measure/draw come in through Env; the Unicode strcpy/strcat composition, the "%d" numbers,
// the 0xBF5F0 colour prefix and the 0x17B0 max-level read are ours.
#include <cstdint>
#include <cstring>
namespace d2client_33550 {
using I = int32_t;
struct Env {
    void* c;
    const uint8_t* dt;                                                   // *sgptDataTables
    const char16_t* (*str)(void* c, uint32_t id);                        // D2Lang 10003 (ECX = id)
    uint32_t (*skill)(void* c, I unit, I id, I m1);                      // D2Common 10708 (ECX unit, EDX id, [esp+4] -1)
    I (*level)(void* c, I unit, uint32_t sk, I bonus);                   // D2Common 10306, stdcall 3
    I (*part)(void* c, I unit, char16_t* buf, const uint8_t* rec, I id, I lvl, I p4, I i, I p8);   // 0x316C0, stdcall 8
    I (*alt)(void* c, I unit);                                           // 0x2C8C0 (EBX = unit)
    void (*measure)(void* c, char16_t* buf, I* w, I* x);                 // D2Win 10177 (ECX buf, EDX &w, [esp+4] &x)
    void (*draw)(void* c, char16_t* buf, I y, I x, I a, I b);            // D2Win 10085 (ECX buf, EDX y, 3 stack words)
};
static void wcat(char16_t* d, const char16_t* s) { while (*d) ++d; while ((*d++ = *s++)) {} }
static void wcpy(char16_t* d, const char16_t* s) { while ((*d++ = *s++)) {} }
static I rd32(const uint8_t* p) { I v; memcpy(&v, p, 4); return v; }
static int16_t rd16(const uint8_t* p) { int16_t v; memcpy(&v, p, 2); return v; }
static const uint8_t* ptr(const uint8_t* p) { return (const uint8_t*)(uintptr_t)(uint32_t)rd32(p); }
// wsprintfA("%d") then Unicode::toUnicode(dst, s, 0x20)
static void num(char16_t* w, I v) {
    char r[12]; I m = 0; uint32_t u = v < 0 ? 0u - (uint32_t)v : (uint32_t)v; I n = 0;
    do { r[m++] = (char)('0' + u % 10); u /= 10; } while (u);
    if (v < 0) w[n++] = u'-';
    while (m) w[n++] = (char16_t)r[--m];
    w[n] = 0;
}
// skills[id]+0x194 -> skilldesc record field `off` (a string id), or 0x1506
static uint32_t descStr(const Env& e, I id, I recOff, uint32_t off) {
    const uint8_t* dt = e.dt;
    if (id >= rd32(dt + 0xba0)) return 0x1506;
    const uint8_t* rec = ptr(dt + 0xb98) + recOff;
    if (!rec) return 0x1506;
    const I di = rd16(rec + 0x194);
    if (di < 0 || di >= rd32(dt + 0xb94)) return 0x1506;
    const uint8_t* d = ptr(dt + 0xb8c) + di * 0x120;
    if (!d) return 0x1506;
    uint16_t v; memcpy(&v, d + off, 2); return v;
}
// 0x17B0 (EAX = id): skills[id]+0x12C, 0x14 when out of range or not positive
static I maxLevel(const Env& e, I id) {
    if (id < 0 || id >= rd32(e.dt + 0xba0)) return 0x14;
    const I v = rd16(ptr(e.dt + 0xb98) + id * 0x23c + 0x12c);
    return v > 0 ? v : 0x14;
}
// 0xBF5F0 (EDI = s, [esp+4] = colour): a non-empty s := str(0xF9A) + '0'+colour + s
static void colour(const Env& e, char16_t* s, uint8_t col) {
    if (!*s) return;
    char16_t t[0x400]; wcpy(t, e.str(e.c, 0xf9a));
    const char16_t cc[2] = { (char16_t)(uint8_t)(col + 0x30), 0 }; wcat(t, cc); wcat(t, s); wcpy(s, t);
}
// the common tail of 0x33550 and 0x33B50: desc lines, the colour-2 long description, the bonus and 0x2C8C0 footers
static void tail(const Env& e, char16_t* buf, I id, I recOff, bool bonus, I unit, char16_t* buf2) {
    wcat(buf, e.str(e.c, descStr(e, id, recOff, 0xc)));
    wcat(buf, e.str(e.c, 0xf9e));
    buf2[0] = 0; wcpy(buf2, e.str(e.c, descStr(e, id, recOff, 0x8)));
    colour(e, buf2, 2);
    wcat(buf, buf2);
    if (bonus) { wcat(buf, e.str(e.c, 0xf9e)); wcat(buf, e.str(e.c, 0xf9e)); wcat(buf, e.str(e.c, 0x1113)); }
    if (!e.alt(e.c, unit)) { wcat(buf, e.str(e.c, 0xf9e)); wcat(buf, e.str(e.c, 0xf9e)); wcat(buf, e.str(e.c, 0x1114)); }
}
static bool bonusOf(const Env& e, I unit, uint32_t sk) { return sk && e.level(e.c, unit, sk, 1) && !e.level(e.c, unit, sk, 0); }

// 1.13c D2Client+0x33550, stdcall(unit, id, lvl, p4, y, x), ret 0x18: the skill tooltip of the skill tree (current and next level)
void sub_33550(const Env& e, I unit, I id, I lvl, I p4, I y, I x) {
    if (!unit || id < 0 || id >= rd32(e.dt + 0xba0)) return;
    const I recOff = id * 0x23c;
    const uint8_t* rec = ptr(e.dt + 0xb98) + recOff;
    if (!rec) return;
    const uint32_t s1 = e.skill(e.c, unit, id, -1);
    const uint32_t s2 = e.skill(e.c, unit, id, -1);
    const bool bonus = bonusOf(e, unit, s2);
    char16_t buf[0x400], buf2[0x100]; char16_t n1[0x20], n2[0x20]; buf[0] = 0;
    wcpy(buf, e.str(e.c, 0x109c));
    const I l1 = lvl > 0 ? lvl : 1;
    bool any = false;
    for (I i = 0x10; i >= 0xa; i--) if (e.part(e.c, unit, buf, rec, id, lvl + 1, p4, i, 1)) any = true;
    if (any) wcat(buf, e.str(e.c, 0xf9e));
    any = false;
    for (I i = 0; i < 6; i++) if (e.part(e.c, unit, buf, rec, id, l1, p4, i, 1)) any = true;
    if (any) {
        if (lvl < 1) wcat(buf, e.str(e.c, 0x10ad));
        else { num(n1, lvl); wcat(buf, e.str(e.c, 0x109e)); wcat(buf, n1); }
        if (bonus) wcat(buf, e.str(e.c, 0x1112));
        wcat(buf, e.str(e.c, 0xf9e)); wcat(buf, e.str(e.c, 0xf9e));
    }
    any = false;
    for (I i = 6; i < 10; i++) if (e.part(e.c, unit, buf, rec, id, l1, p4, i, 1)) any = true;
    if (any) wcat(buf, e.str(e.c, 0xf9e));
    bool maxed = false;
    if (s1) {
        const I m = maxLevel(e, id);
        if (e.level(e.c, unit, s1, 0) >= m) { wcat(buf, e.str(e.c, 0x1111)); wcat(buf, e.str(e.c, 0xf9e)); maxed = true; }
    }
    if (!maxed) {
        const I v = rd16(rec + 0x174) + lvl;
        if (v) { num(n2, v); wcat(buf, e.str(e.c, 0x105b)); wcat(buf, n2); wcat(buf, e.str(e.c, 0xf9e)); }
    }
    tail(e, buf, id, recOff, bonus, unit, buf2);
    I w = 0, xx = 0;
    e.measure(e.c, buf, &w, &xx);
    e.draw(e.c, buf, y, xx + x + 3, 1, 0);
}

// 1.13c D2Client+0x33B50, thiscall(ECX = id; unit, lvl, p4, y, x), ret 0x14: the skill tooltip of the skill bar (next level first)
void sub_33B50(const Env& e, I id, I unit, I lvl, I p4, I y, I x) {
    if (!unit || id < 0 || id >= rd32(e.dt + 0xba0)) return;
    const I recOff = id * 0x23c;
    const uint8_t* rec = ptr(e.dt + 0xb98) + recOff;
    if (!rec) return;
    const uint32_t s0 = e.skill(e.c, unit, id, -1);
    const bool bonus = bonusOf(e, unit, s0);
    char16_t buf[0x400], buf2[0x400]; char16_t n1[0x20]; buf[0] = 0;
    wcpy(buf, e.str(e.c, 0x109c));
    bool maxed = false;
    if (const uint32_t s1 = e.skill(e.c, unit, id, -1)) { const I l = e.level(e.c, unit, s1, 0); maxed = l >= maxLevel(e, id); }
    bool any = false;
    for (I i = 0x10; i >= 0xa; i--) if (e.part(e.c, unit, buf, rec, id, lvl + 1, p4, i, 0)) any = true;
    if (any) wcat(buf, e.str(e.c, 0xf9e));
    if (!maxed) {
        any = false;
        for (I i = 0; i < 6; i++) if (e.part(e.c, unit, buf, rec, id, lvl + 1, p4, i, 0)) any = true;
        if (any) { wcat(buf, e.str(e.c, lvl ? 0x109d : 0x10ad)); wcat(buf, e.str(e.c, 0xf9e)); wcat(buf, e.str(e.c, 0xf9e)); }
    }
    if (lvl > 0) {
        any = false;
        for (I i = 0; i < 6; i++) if (e.part(e.c, unit, buf, rec, id, lvl, p4, i, 0)) any = true;
        if (any) {
            num(n1, lvl); wcat(buf, e.str(e.c, 0x109e)); wcat(buf, n1);
            if (bonus) wcat(buf, e.str(e.c, 0x1112));
            wcat(buf, e.str(e.c, 0xf9e)); wcat(buf, e.str(e.c, 0xf9e));
        }
    }
    any = false;
    for (I i = 6; i < 10; i++) if (e.part(e.c, unit, buf, rec, id, lvl + 1, p4, i, 0)) any = true;
    if (any) wcat(buf, e.str(e.c, 0xf9e));
    tail(e, buf, id, recOff, bonus, unit, buf2);
    I w = 0, xx = 0;
    e.measure(e.c, buf, &w, &xx);
    e.draw(e.c, buf, y - 0x18, xx + x + 3, 0, 0);
}
}
