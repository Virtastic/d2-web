// Prior art used: none (D2MOO has no D2Client). Derived from the 1.13c x86 (the cached decompile loses the stack argument and the register arguments).
#include <cstdint>
#include <cstring>
namespace d2client_45660 {
using U = uint32_t;
// Globals are 32-bit words at module RVAs (+0x11c340 block: 340..374; 254..260 table; bbfc player; 97f8 frame tick; 75b8 array).
struct G {
    U *player;                                 // 0x11bbfc  pPlayer (unit pointer, opaque here)
    U *c340, *c344, *c348, *c34c, *c350, *c354, *c358, *c35c, *c360, *c364, *c368, *c36c, *c370, *c374;
    U *tbl254, *cnt258, *lo25c, *hi260;        // 0x11c254..0x11c260; tbl254 holds a 32-bit pointer to 0x58-byte rows
    U *tick97f8;                               // 0x1197f8
    U *arr75b8;                                // 0x1175b8 holds a 32-bit pointer to dwords
};
struct Calls {
    U (*ord10331)(U unit); U (*ord10826)(U x); const uint8_t* (*ord10014)(U x);   // stdcall, 1 stack word each
    U (*ordD270)(); U (*ordD26A)();                                             // no arguments
    void (*f388b0)(); void (*f38a70)(U ebx, U a, U b, U c, U d);               // 4 stack words, EBX = a global
    U (*f55440)(U eax); void (*f38930)(U a);                                    // EAX = unit; 1 stack word
    U (*f37d20)(U edx); U (*f37af0)(); U (*f37de0)(U edx); U (*f38080)(U edx); // EDX = id
    const uint8_t* (*f0da50)(U eax); void (*ad3e20)();                          // EAX = index
};
static inline U rd32(const uint8_t* p, U o) { U v; memcpy(&v, p + o, 4); return v; }
static inline const uint8_t* ptr(U v) { return (const uint8_t*)(uintptr_t)v; }
// 1.13c D2Client+0x45660  stdcall(now), ret 4
void sub_45660(U now, G& g, Calls& c) {
    U pl = *g.player;
    if (!pl) return;
    U h = c.ord10331(pl); if (!h) return;
    U iv = c.ord10826(h); if (!iv) return;
    if (iv != *g.c34c) { *g.c34c = iv; *g.c340 = now; }
    U cls = 0;
    pl = *g.player;
    if (pl) { U h2 = c.ord10331(pl); if (h2) { U i2 = c.ord10826(h2); if (i2) cls = c.ord10014(i2)[0x21c]; } }
    U r = 0;
    if (*g.tbl254 && (int32_t)cls >= 0 && (int32_t)cls < (int32_t)*g.cnt258) {
        const uint8_t* row = ptr(*g.tbl254) + cls * 0x58;
        r = rd32(row, 0);
        if (r && ((int32_t)r < (int32_t)*g.lo25c || (int32_t)r > (int32_t)*g.hi260)) r = 0;
    }
    bool ok = c.ordD270() && c.ordD26A();
    bool f = *g.c348 == 0 || !ok || now - *g.c340 >= 0x4b;
    if (*g.c348 != r && f) { *g.c348 = r; *g.c350 = now; }
    if (!ok) c.f388b0();
    if (!*g.c348) c.f388b0();
    if (*g.c354) {
        if (*g.c360 && *g.tick97f8 >= *g.c35c) { c.f38a70(*g.c358, 0, 0, 0, 0); *g.c360 = 0; }
        if (*g.c36c && *g.tick97f8 >= *g.c368) {
            if (!c.f55440(*g.player)) c.f38a70(*g.c364, *g.player, 0, 0, 0);
            *g.c36c = 0;
        }
        if (*g.tick97f8 < *g.c370) return;
        *g.c354 = 0;
        if (ok) c.f38930(*g.c358);
    }
    if (*g.c348 && ok && !c.f37d20(*g.c348)) {
        const U* arr = (const U*)(uintptr_t)*g.arr75b8;
        U v = arr[(int32_t)(*g.c348 - *g.lo25c)];
        U m = (!c.f37af0() && !v) ? 2 : 0;
        c.f388b0();
        c.f38a70(*g.c348, 0, 0, m, v);
        *g.c344 = now;
    }
    if (*g.c34c != *g.c374 && now - *g.c340 >= 0x3e) { c.ad3e20(); *g.c374 = *g.c34c; }
    if (*g.c348 && now > *g.c350 + 0x7d) {
        const uint8_t* d = c.f0da50(*g.c348);
        U hd = c.f37d20(*g.c348);
        if (hd && c.f38080(hd)) {
            int32_t k = (int32_t)c.f37de0(hd), o = 0, a = (int32_t)rd32(d, 0x54), b = (int32_t)rd32(d, 0x58), e = (int32_t)rd32(d, 0x5c);
            if (k >= 0 && k < a) o = a;
            if (k >= a && k < b) o = b;
            if (k >= b && k < e) o = e;
            U* arr = (U*)(uintptr_t)*g.arr75b8;
            arr[(int32_t)(*g.c348 - *g.lo25c)] = (U)o;
            *g.c350 = now;
        }
    }
}
}
