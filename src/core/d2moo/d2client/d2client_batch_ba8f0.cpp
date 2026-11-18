// D2Client 0xBA8F0..0xBB8F0 batch: env-seam implementations from the decompile.
// Imported ordinals / unrecovered callees are Env function pointers (names positional guesses); retail memory via rd32.
#include <cstdint>
namespace d2client_batch_ba8f0 {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P);
 P dataTables;                 // *sgptDataTables
 P tblBA608; P tblBAA08;       // DAT_6fbba608 (0x80/row), DAT_6fbbaa08 (128 ptrs)
 P cb663c0; P cb66480;         // callback addresses passed to f51860
 uint32_t (*o10240)(P); uint32_t (*o11029)(P); uint32_t (*o11018)(P); uint32_t (*o10818)(P);
 int32_t (*o10786)(P, uint32_t, uint32_t, uint32_t); int32_t (*o10985)(P); uint32_t (*o10737)(P);
 P (*o10331)(P); void (*o10637)(P, uint32_t); void (*o10019)(P, int32_t);
 void (*o10231)(P, P, uint32_t, uint32_t, uint32_t); uint32_t (*o10614)(); uint32_t (*o10513)();
 uint32_t (*o10867)(P); uint32_t (*o10750)(P);
 uint32_t (*f1630)(P); uint32_t (*f1660)(P); uint32_t (*f1330)(P); uint32_t (*f1380)(P);
 int32_t (*f1510)(P, uint32_t); P (*f15b0)();
 P (*f560d0)(); P (*f5ff10)(P, uint32_t, uint32_t, uint32_t, uint32_t, P);
 void (*f65b70)(P, uint32_t, int32_t, int32_t);
 void (*f525c0)(const uint32_t*, int32_t);
 P (*f54e20)(uint32_t, P, uint32_t); void (*f51860)(P, uint32_t, uint32_t, int32_t, uint32_t, P);
 void (*f69740)(P, const int32_t*, int32_t, int32_t); void (*f69a80)(int32_t, int32_t, int32_t, int32_t, int32_t, uint32_t, uint32_t, P);
 void (*f617c0)(P); void (*f61720)(); void (*f38a70)(P, int, int, int); void (*f692b0)(P); void (*f69470)();
 void (*f67b40)(P, P); void (*f67de0)(P, int32_t); P (*f630f0)(const uint32_t*);
 void (*f682b0)(P, int32_t, int32_t); P (*f68df0)(uint32_t); void (*f628e0)();
 void (*fatal)(uint32_t, const char*);
};
static int32_t s16(const Env& e, P a) { return (int16_t)e.rd32(a); }
static P rowAt(const Env& e, int32_t i) {
 return (i < 0 || (int32_t)e.rd32(e.dataTables + 0xb6c) <= i) ? 0 : e.rd32(e.dataTables + 0xb64) + (P)i * 0x1a4;
}
static P unitRow(const Env& e, P u) { return u ? rowAt(e, (int32_t)e.rd32(u + 4)) : 0; }
static P clsRow(const Env& e, int32_t c) {
 return (c < 0 || (int32_t)e.rd32(e.dataTables + 0xba0) <= c) ? 0 : e.rd32(e.dataTables + 0xb98) + (P)c * 0x23c;
}
// 1.13c D2Client+0xBA8F0
uint32_t sub_BA8F0(const Env& e, P u, P a2) {
 P r = unitRow(e, u);
 if (!r) return 1;
 if (a2) { if (e.rd32(a2) != 1) return 0; if (!e.f1510(e.dataTables, 0xb)) return 0; }
 if (s16(e, r + 0x2c) >= 0) {
  e.o11029(u);
  e.f5ff10(e.f560d0(), s16(e, r + 0x2c), e.f1630(u), e.f1660(u), e.o10240(u), u);
 }
 e.f65b70(u, e.rd32(r + 0x70), s16(e, r + 0x2e), s16(e, r + 0x30));
 return 1;
}
// 1.13c D2Client+0xBAAC0
uint32_t sub_BAAC0(const Env& e, P u) {
 P r = unitRow(e, u);
 if (!r || s16(e, r + 0x24) < 0) return 1;
 P p = e.f560d0();
 if (!p) return 1;
 uint32_t S[23] = {0};
 S[0] = 3; S[1] = (uint32_t)p; S[5] = e.f1630(u); S[6] = e.f1660(u); S[4] = (uint32_t)s16(e, r + 0x24);
 S[10] = e.o11029(u);
 int32_t v = ((int32_t)e.o11029(u) - 1) * (int32_t)e.rd32(r + 0x70) + (int32_t)e.rd32(r + 0x6c);
 if (v < 1) {
  uint32_t a = e.o10240(u), b = e.o11029(u);
  P d = e.f15b0();
  if (!d) return 1;
  v = e.o10786(p, e.rd32(d + 0x144), a, b);
  if (v < 1) v = 1;
 }
 e.f525c0(S, v);
 return 1;
}
// 1.13c D2Client+0xBAD40  (r = row register, unaff_EBX)
uint32_t sub_BAD40(const Env& e, P u) {
 P r = unitRow(e, u);
 if (!r || s16(e, r + 0x24) < 0) return 1;
 P p = e.f560d0();
 if (!p) return 1;
 int32_t cls = (int32_t)e.o10240(u); uint32_t m = e.o11029(u);
 P q = clsRow(e, cls);
 if (!q) return 1;
 uint32_t a = e.o11018(u); uint32_t b = e.o10818(u);
 if (!e.f54e20(b, e.tblBA608 + (P)a * 0x80, a)) return 0;
 uint32_t x = e.f1630(u), y = e.f1660(u);
 int32_t v = (int32_t)e.rd32(r + 0x6c);
 if (v < 1) { v = e.o10786(u, e.rd32(q + 100), cls, m); if (v < 1) v = 1; }
 if ((int32_t)e.rd32(r + 0x70) < 1) e.o10786(u, e.rd32(q + 0x144), cls, m);
 e.f51860(u, x, y, v, e.rd32(q + 0x50) ? 0xa683 : 0, e.cb663c0);
 return 1;
}
// 1.13c D2Client+0xBAEF0  (r = unaff_ESI)
uint32_t sub_BAEF0(const Env& e, P u) {
 P r = unitRow(e, u);
 if (!r || s16(e, r + 0x2c) < 0) return 1;
 int32_t cls = (int32_t)e.o10240(u); uint32_t m = e.o11029(u);
 P q = clsRow(e, cls);
 if (!q) return 1;
 P p = e.f560d0();
 if (!p) return 1;
 uint32_t x = e.f1630(u), y = e.f1660(u);
 int32_t v = (int32_t)e.rd32(r + 0x6c);
 if (v < 1) { v = e.o10786(p, e.rd32(q + 100), cls, m); if (v < 1) v = 1; }
 if ((int32_t)e.rd32(r + 0x70) < 1) e.o10786(p, e.rd32(q + 0x138), cls, m);
 uint32_t f = e.rd32(q + 0x50); if (!f) f = 0xa783;
 e.f51860(p, x, y, v, f, e.cb66480);
 return 1;
}
// 1.13c D2Client+0xBB070
uint32_t sub_BB070(const Env& e, P u) {
 P r = unitRow(e, u);
 if (!r) return 1;
 int32_t cls = (int32_t)e.o10240(u); uint32_t m = e.o11029(u);
 P q = clsRow(e, cls);
 if (!q) return 1;
 int32_t arr[3]; int32_t n = 0;
 for (; n < 3 && s16(e, r + 0x2c + n * 2) >= 0; n++) arr[n] = s16(e, r + 0x2c + n * 2);
 if (n > 0) {
  int32_t v = (int32_t)e.rd32(r + 0x6c);
  if (v < 1) { v = e.o10786(e.f560d0(), e.rd32(q + 0x138), cls, m); if (v < 1) v = 1; }
  e.o10231(u, e.f560d0(), e.rd32(r + 0x88), e.rd32(u + 4), m);
  e.f69740(u, arr, n, v);
 }
 return 1;
}
// 1.13c D2Client+0xBB2A0  (decompile derefs iVar4+0x60/+100; read as row r+0x60/+100, as in BB780)
void sub_BB2A0(const Env& e, P u) {
 P r = unitRow(e, u);
 if (!r || s16(e, r + 0x1e) < 0) { e.f61720(); return; }
 int32_t cls = (int32_t)e.o10240(u);
 P q = clsRow(e, cls);
 if (!q) { e.f61720(); return; }
 uint32_t m = e.o11029(u);
 P p = e.f560d0();
 int32_t a = (int32_t)e.rd32(r + 0x5c), b = (int32_t)e.rd32(r + 0x58);
 if (a < 1) { a = e.o10786(p, e.rd32(q + 100), cls, m); if (a < 1) a = 1; }
 if (b < 1) { b = e.o10786(p, e.rd32(q + 0x144), cls, m); if (b < 1) b = 1; }
 int32_t t = e.o10985(u);
 if ((int32_t)e.rd32(r + 0x60) < t && t < (int32_t)e.rd32(r + 100) && t % b == 0) {
  e.o11018(u); e.o10614();
  uint32_t s1 = e.f1330(u + 0x20); uint32_t x = e.f1630(u);
  uint32_t s2 = e.f1330(u + 0x20); uint32_t y = e.f1660(u);
  uint32_t o = e.o10513();
  e.o10637(u, 0);
  uint32_t c = e.o10240(u);
  e.f69a80(s16(e, r + 0x1e), 0, 0, (int32_t)((s1 - (uint32_t)u) + x), (int32_t)((s2 - (uint32_t)u) + y), c, o, u);
 }
 (void)a;
 e.f617c0(u);
}
// 1.13c D2Client+0xBB470
void sub_BB470(const Env& e, P u) {
 P r = unitRow(e, u);
 if (!r) { e.f61720(); return; }
 if (s16(e, r + 0x34) > 0) {
  uint32_t a = e.o11018(u);
  P pa = e.rd32(u + 0x54), pb = pa ? e.rd32(pa + 0x30) : 0;
  uint32_t c = pb ? e.rd32(pb + 0x40) : 0;
  if (a != 0 && a != c) e.f38a70(u, 0, 0, 0);
  e.o10637(u, c);
 }
 e.f692b0(u);
}
// 1.13c D2Client+0xBB500
void sub_BB500(const Env& e, P u) {
 P r = unitRow(e, u);
 if (!r || s16(e, r + 0x1e) < 0) { e.f61720(); return; }
 e.f69470(); e.f617c0(u);
}
// 1.13c D2Client+0xBB570  (in_EAX = v, EBX = b, EDI = d)
void sub_BB570(const Env& e, int32_t v, P b, P d) {
 if (v > 200 && v < 0xe1) { e.f67b40(b, d); e.f67b40(b, d); return; }
 if (v % 4 == 0) e.f67b40(b, d);
}
// 1.13c D2Client+0xBB5B0
void sub_BB5B0(const Env& e, P p1, P p2, int32_t p3, uint32_t p4, int32_t p5) {
 P r = rowAt(e, p3);
 if (!r) return;
 if (s16(e, r + 0x1e) > 0) e.f67de0(p1, s16(e, r + 0x1e));
 if (s16(e, r + 0x34) > 0 && p4 == 0x13b) e.f38a70(p1, 0, 0, 0);
 if ((int32_t)p4 % 2 != 0) return;
 if (p5 < 3 && (int32_t)p4 % 16 != 0) return;
 uint32_t n = p5 < 3 ? 1 : p5 < 6 ? 2 : p5 < 9 ? 3 : 4;
 int32_t dy = 0;
 if (n > 1) { e.f1380(p2 + 0x20); dy = (int32_t)(e.f1380(p2 + 0x20) - n / 2); }
 uint32_t S[23] = {0};
 S[0] = 1; S[1] = (uint32_t)p1; S[2] = (uint32_t)p1; S[10] = e.o11029(p2);
 int32_t t = (int32_t)e.rd32(p2); P pa = e.rd32(p2 + 0x2c);
 bool direct = t == 2 || (t > 3 && t < 6);
 S[3] = (direct ? e.rd32(pa + 0xc) : pa ? e.o10867(pa) : 0) + p3;
 S[4] = (direct ? e.rd32(pa + 0x10) : pa ? e.o10750(pa) : 0) + dy;
 e.f630f0(S);
}
// 1.13c D2Client+0xBB780
void sub_BB780(const Env& e, P u) {
 P r = unitRow(e, u);
 if (!r) { e.f61720(); return; }
 P p = e.f560d0();
 if (!p) { e.f617c0(u); return; }
 int32_t d = (int32_t)e.rd32(r + 0x60);
 if (s16(e, r + 0x1e) > 0) {
  int32_t v = (int32_t)e.o11018(u);
  if (v < 1) { e.f682b0(p, s16(e, r + 0x1e), d); v = (int32_t)e.rd32(r + 0x58); }
  e.o10637(u, v - 1);
 }
 if (s16(e, r + 0x20) > 0) {
  int32_t v = (int32_t)e.o10818(u);
  if (v < 1) {
   uint32_t S[24] = {0};
   S[0] = 0x102; S[2] = (uint32_t)u; S[4] = (uint32_t)s16(e, r + 0x20); S[11] = e.o10240(u);
   S[12] = e.f1330(u + 0x20) + 1;
   S[7] = e.f1330(u + 0x20) - d; S[8] = e.f1330(u + 0x20) - d;
   if (S[7] == 0 && S[8] == 0) S[7] = e.f1330(u + 0x20) + 1;
   P q = e.f630f0(S);
   if (q) e.o10637(q, e.f1330(u + 0x20));
   v = (int32_t)e.rd32(p + 0x5c);
  }
  e.o10019(u, v - 1);
 }
 e.f617c0(u);
}
// 1.13c D2Client+0xBB8F0
void sub_BB8F0(const Env& e, P u) {
 P r = unitRow(e, u);
 if (!r) { e.f61720(); return; }
 int32_t v = (int32_t)e.o10737(u);
 P p = e.f560d0();
 int32_t d = (int32_t)e.rd32(r + 0x58);
 if (v < s16(e, r + 0x96) - d && d < v && s16(e, r + 0x1e) > 0 && e.f68df0(e.rd32(r + 0x60))) e.f628e0();
 if (v < d + 0x7d && d < v) {
  P w = e.o10331(u);
  for (P q = e.rd32(w + 0x74); q; q = e.rd32(q + 0xe8)) {
   uint32_t t = e.rd32(q + 4);
   if (e.rd32(q) != 2 || t <= 0x10 || t >= 0x16) continue;
   if (s16(e, r + 0x1e) > 0) {
    uint32_t m = e.o11029(u); uint32_t c = e.o10240(u); (void)c;
    if (e.f5ff10(p, s16(e, r + 0x1e), e.f1630(q), e.f1660(q), m, u)) e.f628e0();
   }
   if (s16(e, r + 0x20) > 0 && v % 8 == 0) {
    uint32_t id = e.o11018(u);
    if (!id) continue;
    P m = e.rd32(e.tblBAA08 + (P)(id & 0x7f) * 4);
    while (m && e.rd32(m + 0xc) != id) m = e.rd32(m + 0xe4);
    if (!m) { e.o10637(u, e.rd32(q + 0xc)); continue; }
    if (e.rd32(m) != 2) { e.fatal(e.o10240(u), "unit type"); return; }
    uint32_t S[24] = {0};
    S[0] = 0x421; S[5] = (uint32_t)s16(e, r + 0x20); S[6] = e.f1660(q);
    S[8] = e.f1630(m); S[9] = e.f1660(m); S[10] = e.o10240(u); S[13] = e.o11029(u);
    e.f630f0(S + 1);
    if (e.f1330(m + 0x20) == 0) e.o10637(u, e.rd32(q + 0xc));
   }
  }
 }
 e.f617c0(u);
}
}
