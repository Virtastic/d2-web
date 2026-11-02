// D2Client 0xBBC10..0xBC760 batch: env-seam implementations from the decompile.
// Imported ordinals / unrecovered callees are Env function pointers (names positional guesses); retail memory via rd32.
// Register-passed args the decompile shows as unaff_*/in_EAX are explicit parameters.
#include <cstdint>
#include <initializer_list>
#include "d2client_batch_bbc10.h"
namespace d2client_batch_bbc10 {
static int32_t s16(const Env& e, P a) { return (int16_t)e.rd32(a); }
static P rowAt(const Env& e, int32_t i) {
 return (i < 0 || (int32_t)e.rd32(e.dataTables + 0xb6c) <= i) ? 0 : e.rd32(e.dataTables + 0xb64) + (P)i * 0x1a4;
}
static P unitRow(const Env& e, P u) { return u ? rowAt(e, (int32_t)e.rd32(u + 4)) : 0; }
static P clsRow(const Env& e, int32_t c) {
 return (c < 0 || (int32_t)e.rd32(e.dataTables + 0xba0) <= c) ? 0 : e.rd32(e.dataTables + 0xb98) + (P)c * 0x23c;
}
// 1.13c D2Client+0xBBC10
void sub_BBC10(const Env& e, P u) {
 P r = unitRow(e, u);
 if (!r || s16(e, r + 0x1e) < 0) { e.f61720(); return; }
 if (s16(e, r + 0x1e) < s16(e, r + 0x20)) e.f1380(u + 0x20);
 int32_t cls = (int32_t)e.o10240(u);
 P q = clsRow(e, cls);
 if (q) {
  P p = e.f560d0();
  uint32_t m = e.o11029(u);
  e.o10786(p, e.rd32(q + 0x138), cls, m, nullptr);
  int32_t aux = 0;
  int32_t v = e.o10786(p, e.rd32(q + 0x13c), cls, m, &aux);
  if (v != 0 && aux > 0) {
   if (e.f68df0((uint32_t)aux)) e.f62e00();
   e.f617c0(u);
   return;
  }
 }
 e.f61720();
}
// 1.13c D2Client+0xBBD40
void sub_BBD40(const Env& e, P u) {
 P r = unitRow(e, u);
 if (!r || s16(e, r + 0x1e) < 0) { e.f61720(); return; }
 if (s16(e, r + 0x1e) < s16(e, r + 0x20)) e.f1380(u + 0x20);
 int32_t m = (int32_t)e.o11029(u);
 int32_t d = (int32_t)e.rd32(r + 0x60); if (d < 1) d = 1;
 int32_t n = 2; if (m / d > 1) n = m / d;
 if (e.f68df0(e.rd32(r + 0x58) + (uint32_t)n)) e.f62e00();
 e.f617c0(u);
}
// 1.13c D2Client+0xBBE00
void sub_BBE00(const Env& e, P u) {
 if (unitRow(e, u)) e.f69470();
 e.f617c0(u);
}
// 1.13c D2Client+0xBBE60
void sub_BBE60(const Env& e, P u) {
 if (e.o10203(e.rd32(u + 0x2c))) {
  int32_t cls = (int32_t)e.rd32(u + 4);
  P r = rowAt(e, cls);
  if (r && s16(e, r + 0x1e) >= 0) {
   uint32_t m = e.o11029(u);
   e.o10231(u, e.f560d0(), e.rd32(r + 0x84), (uint32_t)cls, m);
   e.f69650(s16(e, r + 0x1e));
  }
 }
 e.f617c0(u);
}
// 1.13c D2Client+0xBBF30
uint32_t sub_BBF30(const Env& e, P u, uint32_t a2) {
 P r = unitRow(e, u);
 if (r && s16(e, r + 0x2c) >= 0) {
  if (a2) return 0;
  uint32_t m = e.o11029(u);
  e.fb210(e.rd32(r + 0x6c), s16(e, r + 0x2c), e.o10240(u), u, m, u);
 }
 return 1;
}
// 1.13c D2Client+0xBC210  (ctx = unaff_EBX)
void sub_BC210(const Env& e, P ctx, P u) {
 P r = unitRow(e, u);
 if (!r) { e.f61720(); return; }
 if (!e.f560d0()) { e.f617c0(u); return; }
 int32_t f = (int32_t)e.o10985(u);
 int32_t a = s16(e, r + 0x1e);
 if (a > 0 && f > 10) e.f6b5b0(ctx, u, a, (uint32_t)f, e.f60000(r));
 int32_t d = (int32_t)e.rd32(r + 0x58); if (d < 1) d = 1;
 if (s16(e, r + 0x20) > 0 && f % d == 0) e.f682b0(ctx, s16(e, r + 0x20), 0x14);
 int32_t w = (int32_t)e.rd32(r + 0x5c);
 if (w > 0 && f == 0x5a) { int32_t v = ((w / 3) * 25000) / 0x19; e.f39f00(8, v, v); }
 if (s16(e, r + 0x22) > 0 && f > 0x73 && f < 0x13b) e.f6b570();
 e.f617c0(u);
}
// 1.13c D2Client+0xBC3E0  (out = unaff_EBX, str = unaff_EDI, UTF-16 buffer)
uint32_t sub_BC3E0(const Env& e, P out, P str) {
 e.wr32(out, 0);
 int32_t len = 0; while (e.rd16(str + (P)len * 2)) len++;
 int32_t i = 0;
 for (;; i++) { uint32_t c = e.rd16(str + (P)i * 2); if (c == 0) return 0; if (c == 10) break; }
 e.wr16(str + (P)i * 2, 0);
 if (len <= i) return 0;
 e.wr32(out, (uint32_t)(str + (P)(i + 1) * 2));
 return 1;
}
// 1.13c D2Client+0xBC430  (a = EAX, b = EBP, c = ESI)
// ponytail: decompile shows a stack array of (short,int) pairs starting one slot below local_10c; modelled as 4 slots.
int32_t sub_BC430(const Env& e, P a, P b, P c) {
 if (!a) return 0;
 int32_t loc[4] = {0, 0, 0, 0};  // loc[k] = local_10c[k-1]
 P x = e.rd32(a + 0x60);
 if (x) { loc[0] = (int32_t)e.o11139(x, 4); loc[2] = (int32_t)e.o11139(x, 5); }
 int32_t n = e.o10702(), res = 0;
 for (int32_t i = 0; i < n; i++) {
  int32_t s = i * 2 < 4 ? (int16_t)loc[i * 2] : 0, v = i * 2 + 1 < 4 ? loc[i * 2 + 1] : 0;
  if (s == 0) { res = v; continue; }
  bool hit = (b && e.o10744(b, s)) || (c && e.o10744(c, s));
  if (hit && res < v) res = v;
 }
 return res;
}
// 1.13c D2Client+0xBC4E0  (EBX = str, ECX = left, EAX = right; stack a, b; ret 8): centres str in [left, right] when its width
// (Ordinal_10028) is under the span, then draws it (Ordinal_10150: ECX = str, EDX = x, stack a, b, 0).
void sub_BC4E0(const Env& e, P str, uint32_t left, uint32_t right, uint32_t a, uint32_t b) {
 int32_t span = (int32_t)(right - left + 1u);
 int32_t w = e.o10028(str);
 if (w < span) left += (uint32_t)((int32_t)((uint32_t)span - (uint32_t)w) >> 1);
 e.o10150(str, left, a, b, 0);
}
// 1.13c D2Client+0xBC520  (ebp = unaff_EBP, 3rd o10973 arg)
int32_t sub_BC520(const Env& e) {
 int32_t tot = e.o10672() + e.o10973(0, 0, 0);
 uint32_t idx = e.rd32(e.gCC390);
 int32_t id = (int32_t)e.rd32(e.gA218C);
 if (idx > 2) { e.fatal(e.o10265(0)); return 0; }
 P row = (id < 0 || (int32_t)e.rd32(e.dataTables + 0xa80) <= id) ? 0 : e.rd32(e.dataTables + 0xa78) + (P)id * 0x1a8;
 if (!row) return 0;
 int32_t k = (int32_t)e.rd32(e.gCC394);
 uint32_t mode = (k < 2 || k > 3) ? 1 : e.rd32(e.gC9858);
 int32_t val = s16(e, row + 0xaa + idx * 2);
 uint32_t st[14] = {0};
 e.o11089(id, mode, idx, val, 0, (P)st);
 if (tot == 0) {
  for (uint32_t fl : {0x10u, 0x20u}) {
   uint32_t z[14] = {0};
   k = (int32_t)e.rd32(e.gCC394);
   mode = (k < 2 || k > 3) ? 1 : e.rd32(e.gC9858);
   e.o11089(id, mode, idx, val, fl, (P)z);
  }
 }
 if ((e.rd32(row + 0x4c) & 0xff) != 1 && e.rd32(e.gC9854) == 0 && e.rd32(e.gCC390) != 0) tot = (tot * 10) / 0xf;
 int32_t d = 8, pct = 0;
 if (tot < 0) { d = 8 - tot; tot = 0; }
 if (d != 0 && tot != 0) pct = (tot * 100) / (d + tot);  // ponytail: tot==0 leaves a stack ptr in retail; 0 here
 int32_t w = e.o10973(0, 0xc, 0);
 return (val * pct * 2) / (w + val);
}
// 1.13c D2Client+0xBC720  (ECX = c, EAX = a): a + the dword at +0x3C of data-table record [c+4] (0xC4-byte records at
// [*sgptDataTables+0xBC4], count [+0xBC8], signed compare) when c is set, [c] == 0 and 0 <= [c+4] < count; else a unchanged.
uint32_t sub_BC720(const Env& e, P c, uint32_t a) {
 if (!c || e.rd32(c) != 0) return a;
 int32_t i = (int32_t)e.rd32(c + 4);
 if (i < 0 || i >= (int32_t)e.rd32(e.dataTables + 0xbc8)) return a;
 P r = (P)(uint32_t)(e.rd32(e.dataTables + 0xbc4) + (uint32_t)i * 0xc4);
 if (!r) return a;
 return a + e.rd32(r + 0x3c);
}
// 1.13c D2Client+0xBC760  (level = EAX, key = EBP; formats a level string into a 128-wchar local)
void sub_BC760(const Env& e, int32_t level, int32_t x, int32_t y, int32_t key) {
 if (level < 5) level = 5; else if (level > 0x5f) level = 0x5f;
 uint16_t buf[256] = {0};
 int32_t g0 = (int32_t)e.rd32(e.g9A0), g1 = (int32_t)e.rd32(e.g9A4), g2 = (int32_t)e.rd32(e.g8BC4C);
 if (x < g0 + 0xad || x > g0 + 0x137) return;
 if (y < g1 - 0x11e + g2 || y > g1 - 0x10b + g2) return;
 P pl = e.rd32(e.gBBFC);
 int32_t t = e.o10212(pl, e.rd32(e.gC9854));
 if (t < 1) {
  t = (int32_t)e.bc430(pl);
  if (t > 0) { e.o10431(pl, e.rd32(pl + 0x60), (P)buf, -1, 1); if (key != 0xd) t = 0; }
 }
 if (!e.f1230()) { e.fatal(e.o10265(0x15c)); return; }
 uint32_t s = e.o10003((uint32_t)level);
 if (t == 0) e.usprintf(0x80, (P)buf, e.o10003(s));
 else e.usprintf(0x80, (P)buf, e.o10003b((uint32_t)t, (uint32_t)level));
 e.o10013(g1 - 0x13b + g2, e.rd32(e.gBBC8), 2, 0);
}
}
