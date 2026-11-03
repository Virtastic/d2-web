// Prior art: none (D2MOO has no D2Client). Derived from the 1.13c disassembly of D2Client+0x33290.
// Env-seam style: sgptDataTables, the string lookup (D2Lang 10003), FUN_6fae16c0 (0x316C0) and the D2Win measure/draw come in
// through Env; the Unicode strcpy/strcat and wsprintfA("%d") composition is ours.
#include <cstdint>
#include <cstring>
namespace d2client_33290 {
using I = int32_t;
struct Env {
    void* c;
    const uint8_t* dt;                                                   // *sgptDataTables
    const char16_t* (*str)(void* c, uint32_t id);                        // D2Lang 10003 (ECX = id)
    I (*part)(void* c, const I* p1, char16_t* buf, const uint8_t* rec, I p2, I p3, I p4, I i, I p5);   // 0x316C0, stdcall 8
    void (*measure)(void* c, char16_t* buf, I* w, I* x);                 // D2Win 10177 (ECX buf, EDX &w, [esp+4] &x)
    void (*draw)(void* c, char16_t* buf, I y, I x, I p5, I z);           // D2Win 10085 (ECX buf, EDX y, 3 stack words)
};
static void wcat(char16_t* d, const char16_t* s) { while (*d) ++d; while ((*d++ = *s++)) {} }
static I rd32(const uint8_t* p) { I v; memcpy(&v, p, 4); return v; }
static const uint8_t* ptr(const uint8_t* p) { return (const uint8_t*)(uintptr_t)(uint32_t)rd32(p); }
// skills[p2]+0x194 -> skilldesc record field `off` (a string id), or 0x1506
static uint32_t descStr(const Env& e, I p2, I recOff, uint32_t off) {
    const uint8_t* dt = e.dt;
    if (p2 >= rd32(dt + 0xba0)) return 0x1506;
    const uint8_t* rec = ptr(dt + 0xb98) + recOff;
    if (!rec) return 0x1506;
    int16_t di; memcpy(&di, rec + 0x194, 2);
    if (di < 0 || di >= rd32(dt + 0xb94)) return 0x1506;
    const uint8_t* d = ptr(dt + 0xb8c) + di * 0x120;
    if (!d) return 0x1506;
    uint16_t v; memcpy(&v, d + off, 2); return v;
}
// 1.13c D2Client+0x33290, stdcall(p1, p2, p3, p4, p5, p6, p7)
void sub_33290(const Env& e, const I* p1, I p2, I p3, I p4, I p5, I p6, I p7) {
    if (!p1 || p2 < 0 || p2 >= rd32(e.dt + 0xba0)) return;
    const I recOff = p2 * 0x23c;
    const uint8_t* rec = ptr(e.dt + 0xb98) + recOff;
    if (!rec) return;
    char16_t buf[0x400]; char16_t num[0x20]; buf[0] = 0; num[0] = 0;
    { const char16_t* s = e.str(e.c, 0x109c); char16_t* d = buf; while ((*d++ = *s++)) {} }
    I any = 0;
    for (I i = 0; i < 10; i++) if (e.part(e.c, p1, buf, rec, p2, p3, p4, i, p5)) any = 1;
    if (any) {
        char a[24]; I n = 0; uint32_t v = p3 < 0 ? 0u - (uint32_t)p3 : (uint32_t)p3; char r[12]; I m = 0;
        do { r[m++] = (char)('0' + v % 10); v /= 10; } while (v);
        if (p3 < 0) a[n++] = '-';
        while (m) a[n++] = r[--m];
        a[n] = 0;
        for (I k = 0; k <= n && k < 0x20; k++) num[k] = (char16_t)(uint8_t)a[k];
        wcat(buf, e.str(e.c, 0x109e)); wcat(buf, num); wcat(buf, e.str(e.c, 0xf9e)); wcat(buf, e.str(e.c, 0xf9e));
    }
    if (p2 == 0 && p1[0] == 0 && p1[1] == 6) wcat(buf, e.str(e.c, 0x5722));
    wcat(buf, e.str(e.c, descStr(e, p2, recOff, 0xa)));
    wcat(buf, e.str(e.c, 0xf9e));
    wcat(buf, e.str(e.c, descStr(e, p2, recOff, 0x8)));
    I w = 0, x = 0;
    e.measure(e.c, buf, &w, &x);
    e.draw(e.c, buf, p6, x + p7 + 3, p5, 0);
}
}
