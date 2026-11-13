// 1.13c D2Client+0x34320 -- fastcall (ECX this, EDX p, stack a0..a4, ret 0x14) numeric-range line builder.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace {
// wsprintfA("%d") family of the 1.13c formats, written as UTF-16 (Unicode::toUnicode of the ASCII text).
struct W { char16_t* o; int n = 0; };
void ch(W& w, char c) { if (w.n < 31) w.o[w.n] = (char16_t)(unsigned char)c; ++w.n; }
void num(W& w, int32_t v)
{
    char t[12]; int k = 0; uint32_t u = v < 0 ? 0u - (uint32_t)v : (uint32_t)v;
    do { t[k++] = (char)('0' + u % 10); u /= 10; } while (u);
    if (v < 0) t[k++] = '-';
    while (k) ch(w, t[--k]);
}
int32_t kilo(int32_t v) { return (int32_t)((uint32_t)v + 500u) / 1000; }
void fmtRange(char16_t* o, int32_t a, int32_t b)
{
    W w{o};
    if (a == b) num(w, a);
    else if (a < 10000) { num(w, a); ch(w, '-'); if (b < 10000) num(w, b); else { num(w, kilo(b)); ch(w, 'K'); } }
    else { num(w, kilo(a)); ch(w, 'K'); ch(w, '-'); num(w, kilo(b)); ch(w, 'K'); }
}
}
namespace d2cl {
void f34320(GE& e, uint32_t self, uint32_t p, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
{
    if (!p || !a0) return;
    uint32_t s1c = 0, s18 = 0, s14 = 0;   // retail [esp+0x20], [+0x1c], [+0x18]
    e.h(0x31220, 0, self, 0, 0, 0, e.stk(&s1c), e.stk(&s18), e.stk(&s14), 0, p, 0x80, 0, 0);
    e.h(0x30E30, 0, 0, 0, 0, 0, self, e.stk(&s1c), e.stk(&s18), e.stk(&s14), p);
    uint32_t s0c = 0, s8 = 0, s10 = 0;
    e.h(0x2E190, a0, 0, 0, a1, 0, self, e.stk(&s0c), e.stk(&s8), e.stk(&s10));
    if (!s1c && !s18) { e.h(0x2F0A0, s8, s0c, 0, 0, 0, s10, a2, a3, a4); return; }
    if (!s0c && !s8) { e.h(0x2F0A0, s18, s1c, 0, 0, 0, s14, a2, a3, a4); return; }
    char16_t b1[32] = {}, b2[32] = {};
    fmtRange(b1, (int32_t)s1c, (int32_t)s18);
    fmtRange(b2, (int32_t)s0c, (int32_t)s8);
    e.h(0x2D3C0, 0, 0, 0, 0, 0, e.stk(b1), e.stk(b2), a2, a3, a4, s14, s10);
    e.ws(0x2D3C0, 0, b1);
    e.ws(0x2D3C0, 1, b2);
}
}
