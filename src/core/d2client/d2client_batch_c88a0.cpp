// D2Client 0xC88A0..0xCA4A9 batch: env-seam implementations from the decompile.
// Unrecovered callees / imported ordinals are Env function pointers (names positional); retail memory via rd32/rd8/wr32.
// Register-passed args are given in decompile order; ghost stack words (uninitialised in the decompile) are left 0.
#include <cstdint>
namespace d2client_batch_c88a0 {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P); uint32_t (*rd8)(P); void (*wr32)(P, uint32_t);
 P dataTables; P bitMasks;        // *sgptDataTables, gdwBitMasks
 P tblDX; P tblDY;                // DAT_6fb86dac, DAT_6fb86d8c (8 x int32)
 P tblAA420; uint32_t cb23b60;    // DAT_6fbaa420 byte table; callback addr
 int32_t (*f40550)(P); int32_t (*f51340)(uint32_t*); P (*f630f0)(const uint32_t*);
 P (*f515c0)(); uint32_t (*f1330)(P); int32_t (*f1260)(); P (*f1580)();
 void (*f52380)(uint32_t, int32_t); int32_t (*f55f70)(); void (*f38a70)(P, int, int, int);
 void (*f55420)(); P (*f58c20)(int); void (*f58be0)(P); void (*f77e30)(int32_t, int32_t, int32_t);
 void (*fda0b0)(); int32_t (*f55440)(); int32_t (*f51cf0)(int32_t, int32_t); void (*f1b300)(P, int, int, int, int);
 uint32_t (*f52c90)(P, int32_t, uint32_t, uint32_t);
 P (*o10511)(P); int32_t (*o10980)(P); int32_t (*o10649)(P); void (*o10243)(P, int); void (*o10554)(P, int);
 uint32_t (*o10867)(P); uint32_t (*o10750)(P); int32_t (*o10080)(); uint32_t (*o10192)(); void (*o10425)(uint32_t, uint32_t);
 int32_t (*o10786)(P, uint32_t, uint32_t, uint32_t); void (*o10440)(P, uint32_t); void (*o10074)(P, int32_t);
 void (*o10637)(P, uint32_t); void (*o10019)(P, int32_t); void (*o10520)(uint32_t, int); void (*o10441)(P, uint32_t);
 uint32_t (*o10392)(); uint32_t (*o10983)(); void (*o10764)(uint32_t); void (*o10286)(uint32_t, uint32_t, uint32_t);
 void (*o11129)(); int32_t (*o10494)(); void (*o10945)();
 void (*fatal)(uint32_t, const char*);
 void (*scale80)(uint32_t&, uint32_t&, uint16_t&); // x87 multiply by DAT_6fbaa438
};
static int32_t s16(const Env& e, P a) { return (int16_t)e.rd32(a); }
static P clsRow(const Env& e, int32_t c) {
 return (c < 0 || (int32_t)e.rd32(e.dataTables + 0xba0) <= c) ? 0 : e.rd32(e.dataTables + 0xb98) + (P)c * 0x23c;
}
static bool rowOk(const Env& e, int32_t i) { return i >= 0 && i < (int32_t)e.rd32(e.dataTables + 0xb6c); }
// unit X / Y (types 2,4,5 read the cached path; else ordinal)
static int32_t ux(const Env& e, P u) {
 uint32_t t = e.rd32(u); P p = e.rd32(u + 0x2c);
 if (t == 2 || (t > 3 && t < 6)) return (int32_t)e.rd32(p + 0xc);
 return p ? (int32_t)e.o10867(p) : 0;
}
static int32_t uy(const Env& e, P u) {
 uint32_t t = e.rd32(u); P p = e.rd32(u + 0x2c);
 if (t == 2 || (t > 3 && t < 6)) return (int32_t)e.rd32(p + 0x10);
 return p ? (int32_t)e.o10750(p) : 0;
}
static uint64_t seedStep(const Env& e, P u) {
 uint64_t v = (uint64_t)e.rd32(u + 0x20) * 0x6ac690c5u + e.rd32(u + 0x24);
 e.wr32(u + 0x20, (uint32_t)v); e.wr32(u + 0x24, (uint32_t)(v >> 32));
 return v;
}
// 1.13c D2Client+0xC88A0
uint32_t sub_C88A0(const Env& e, P a1, int32_t cls) {
 if (!clsRow(e, cls)) return 0;
 int32_t idx = e.f40550(a1);
 if (!(idx > -1 && idx < (int32_t)e.rd32(e.dataTables + 0xb6c))) return 0;
 P o = e.o10511(a1);
 if (!o) return 0;
 uint32_t S[23] = {0};
 S[0] = 0x2001; S[4] = (uint32_t)idx; S[11] = (uint32_t)cls;
 S[4] = (uint32_t)e.o10980(o);
 S[4] = (uint32_t)e.o10649(o);
 e.f630f0(S);
 e.o10243(o, 0); e.o10554(o, 0);
 return 1;
}
// 1.13c D2Client+0xC8980  (unit in EAX)
uint32_t sub_C8980(const Env& e, P u) {
 uint32_t B[24] = {0};
 if (!e.f51340(B + 1)) return 0; // ponytail: B[0] mirrors the decompile's iStack_6c (read after call)
 int32_t x = ux(e, u), y = uy(e, u);
 int32_t dx = (int32_t)B[2] - x, dy = (int32_t)B[1] - y;
 int32_t sc = e.o10080(); if (sc < 2) sc = 1;
 uint32_t S[23] = {0};
 S[0] = 3;
 S[5] = (uint32_t)((dx * 2) / sc + x);
 S[6] = (uint32_t)((dy * 2) / sc + y);
 S[7] = B[0] * 5; S[8] = (uint32_t)(dy * 5);
 P q = e.f630f0(S);
 if (!q) return 0;
 e.o10425(e.rd32(q + 0x2c), e.o10192());
 return 1;
}
// 1.13c D2Client+0xC8B40
uint32_t sub_C8B40(const Env& e, P u, int32_t cls, uint32_t a3) {
 P r = clsRow(e, cls);
 if (!r || !rowOk(e, s16(e, r + 0xea))) return 0;
 if (u) e.wr32(u + 0xc4, e.rd32(u + 0xc4) | 0x40);
 uint32_t tmp[2] = {0, 0};
 if (!e.f51340(tmp)) return 0;
 int32_t v = e.o10786(u, e.rd32(r + 100), (uint32_t)cls, a3) - 3;
 if (v < 2) v = 2;
 uint32_t S[23] = {0};
 S[4] = 0xbf; S[11] = (uint32_t)cls;
 P q = e.f630f0(S);
 if (!q) return 0;
 e.f55420();
 P w = e.f58c20(1);
 if (w) { e.f58be0(w); e.wr32(q + 0x64, (uint32_t)w); }
 e.o10440(q, 0); // arg2 = unaff_EBP in the decompile
 e.o10074(q, v);
 return 1;
}
// 1.13c D2Client+0xC8CE0  (unit in EAX)
uint32_t sub_C8CE0(const Env& e, P u, int32_t cls, uint32_t a2, int32_t a3, int32_t a4) {
 P r = clsRow(e, cls);
 if (!r) return 0;
 int32_t ea = s16(e, r + 0xea);
 if (!rowOk(e, ea) || !a3 || !a4) return 0;
 int32_t t = e.f1260();
 if (!t) return 0;
 if (u) e.wr32(u + 0xc4, e.rd32(u + 0xc4) | 0x40);
 uint32_t S[23] = {0};
 S[0] = 0x2021;
 S[7] = (e.f1330(u + 0x20) - 1) + (uint32_t)a3;
 S[8] = (e.f1330(u + 0x20) - 1) + (uint32_t)a4;
 S[6] = (uint32_t)a4; S[12] = a2; S[5] = (uint32_t)a3; S[11] = (uint32_t)cls; S[4] = (uint32_t)ea;
 uint32_t b = e.rd8(t + 5);
 if (!(e.rd32(e.bitMasks + 8) & b)) {
  if (!(e.rd32(e.bitMasks + 0xc) & b)) {
   e.f52380(2, 2);
   int32_t s = s16(e, e.f1580() + 0x1a);
   if (s >= 1 && rowOk(e, s)) { S[4] = (uint32_t)s; e.f52380(3, 1); }
  } else {
   e.f52380(3, 3);
   int32_t s = s16(e, e.f1580() + 0x18);
   if (s >= 1 && rowOk(e, s)) { S[4] = (uint32_t)s; e.f52380(4, 2); }
  }
 } else e.f52380(1, 0);
 S[6] = (uint32_t)a4; S[5] = (uint32_t)a3;
 P q = 0;
 int32_t v = s16(e, r + 0xec);
 if (v > 0 && rowOk(e, v)) { S[4] = (uint32_t)v; q = e.f630f0(S); }
 v = s16(e, r + 0xee);
 if (v > 0 && rowOk(e, v)) { S[12] = 2; S[4] = (uint32_t)v; e.f630f0(S); }
 if (!e.f55f70() && q && s16(e, r + 0x10a) > 0) e.f38a70(q, 0, 0, 0);
 return 1;
}
// 1.13c D2Client+0xC9030  (flag in EAX, unit in ESI; callers AF090/AF0F0)
uint32_t sub_C9030(const Env& e, P u, uint32_t flag, int32_t cls, uint32_t arg) {
 P r = clsRow(e, cls);
 if (!r) return 0;
 e.fda0b0();
 uint32_t w = e.rd32(u + 0x2c);
 if (!w) e.fatal(0x946, "");
 int32_t i3 = (int32_t)e.o10392();
 uint32_t u7 = e.rd32(u + 0x2c);
 uint32_t v = e.o10983();
 e.o10764(e.rd32(u + 0x2c));
 uint32_t bit = (e.rd32(u + 0xc4) >> 6) & 1;
 if (!flag) e.o10286(e.rd32(u + 0x2c), arg, (uint32_t)cls); else e.o11129();
 if (e.rd32(e.bitMasks + 8) & e.rd8(r + 8)) {
  bool b = false;
  if (flag && e.f55440() && e.o10494()) { b = true; e.o10945(); }
  if (!e.f51cf0(i3, -1)) v = 0; else {
   if (b) e.o10945();
   goto rest;
  }
  goto done;
 }
rest:
 if (s16(e, r + 0x122) >= 0) { (void)u7; e.f38a70(u, 0, 0, 0); }
 if (s16(e, r + 0x124) >= 0 && s16(e, r + 0x124) < (int32_t)e.rd32(e.dataTables + 0xbc0)) e.f1b300(u, 2, 0, 0, 0);
 v = e.f52c90(u, i3, v, arg & 0xffff);
done:
 if (!w) e.o10286(e.rd32(u + 0x2c), 0, bit); else e.o11129();
 e.wr32(u + 0xc4, bit ? (e.rd32(u + 0xc4) | 0x40) : (e.rd32(u + 0xc4) & ~0x40u));
 return v;
}
// 1.13c D2Client+0xC94E0
uint32_t sub_C94E0(const Env& e, P u, int32_t cls, uint32_t a3) {
 P r = clsRow(e, cls);
 if (!r) return 0;
 uint32_t B[24] = {0};
 if (!e.f51340(B)) return 0;
 for (int i = 1; i < 24; i++) B[i] = 0;
 B[7] = B[0]; B[1] = 1; B[13] = a3; B[2] = (uint32_t)u; B[12] = (uint32_t)cls;
 int32_t v = s16(e, r + 0xea);
 if (v >= 0 && (B[5] = (uint32_t)v, v < (int32_t)e.rd32(e.dataTables + 0xb6c))) e.f630f0(B + 1);
 v = s16(e, r + 0xec);
 if (v >= 0 && (B[5] = (uint32_t)v, v < (int32_t)e.rd32(e.dataTables + 0xb6c))) {
  P q = e.f630f0(B + 1);
  if (q) e.o10637(q, e.rd32(u + 0x28));
 }
 return 1;
}
// 1.13c D2Client+0xC95F0
// ponytail: the decompile reuses iVar1/iVar2 oddly; y/z = results for row+0x118 / +0x11c.
uint32_t sub_C95F0(const Env& e, P u, uint32_t cls, uint32_t a3) {
 P r = clsRow(e, (int32_t)cls);
 if (!r) return 0;
 if (e.o10786(u, e.rd32(r + 100), cls, a3) <= 0) return 0;
 if (e.o10786(u, e.rd32(r + 0x114), cls, a3) <= 0) return 0;
 int32_t y = e.o10786(u, e.rd32(r + 0x118), cls, a3); if (y <= 0) return 0;
 int32_t z = e.o10786(u, e.rd32(r + 0x11c), cls, a3); if (z <= 0) return 0;
 int32_t i3 = y * (int32_t)cls, i4 = z * (int32_t)cls;
 uint32_t tmp[2] = {0, 0};
 if (!e.f51340(tmp)) return 0;
 uint32_t S[23] = {0};
 S[0] = 1; S[1] = (uint32_t)u; S[5] = a3;
 int16_t slots[2] = {(int16_t)s16(e, r + 0xea), (int16_t)s16(e, r + 0xec)};
 for (int k = 0; k < 2; k++) {
  if (slots[k] < 0) continue;
  S[4] = (uint32_t)(int32_t)slots[k];
  P q = e.f630f0(S);
  if (!q) continue;
  e.f77e30(i3, 0, i4);
  P a = e.rd32(q + 0x54);
  if (!a) continue;
  P t = e.rd32(a + 0x30);
  if (!t) continue;
  e.wr32(t + 0x10, (uint32_t)(z * -0x800)); e.wr32(t + 0x14, 0); e.wr32(t + 0x18, (uint32_t)(y * -0x800));
 }
 return 1;
}
// 1.13c D2Client+0xC97D0  (unit in EAX, class row ptr in a1)
void sub_C97D0(const Env& e, P row, uint32_t a2, uint32_t a3, P u) {
 (void)a2;
 uint32_t k = (uint32_t)seedStep(e, u) & 7;
 int32_t x = ux(e, u), y = uy(e, u);
 int32_t dx = (int32_t)e.rd32(e.tblDX + k * 4), dy = (int32_t)e.rd32(e.tblDY + k * 4);
 int32_t cx = x + dx;
 int32_t idx = s16(e, row + 0xea);
 if (idx < 0) return;
 if (s16(e, row + 0xec) >= 0) {
  if (s16(e, row + 0xee) < 0) {
   if (!(seedStep(e, u) & 1)) idx = s16(e, row + 0xec);
  } else {
   uint32_t m = (uint32_t)seedStep(e, u) % 3;
   if (m == 0) idx = s16(e, row + 0xec); else if (m == 1) idx = s16(e, row + 0xee);
  }
 }
 uint32_t S[23] = {0};
 S[0] = 3; S[4] = (uint32_t)idx; S[8] = (uint32_t)(-dx); S[12] = a3;
 S[5] = (uint32_t)cx; S[6] = (uint32_t)(y + dy); S[7] = (uint32_t)dy;
 P q = e.f630f0(S);
 if (!q) return;
 uint32_t r = e.f1330(u + 0x20);
 if (P a = e.rd32(q + 0x54)) if (P t = e.rd32(a + 0x30)) { e.wr32(t + 4, 0); e.wr32(t + 8, 0); e.wr32(t + 0xc, r << 11); }
 e.o10637(q, (uint32_t)x);
 e.o10019(q, y);
}
// 1.13c D2Client+0xC9A50  (unit in EAX)
uint32_t sub_C9A50(const Env& e, P u, int32_t count, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t p5, int32_t p6, int32_t p7) {
 uint32_t B[24] = {0};
 if (!e.f51340(B)) return 0;
 if (p6) {
  count--;
  P q = e.f515c0();
  if (q) { if (p7) e.o10520(e.rd32(q + 0x2c), 0); e.o10441(q, p5); }
 }
 for (int i = 1; i < 24; i++) B[i] = 0;
 B[1] = 0x21; B[6] = (uint32_t)ux(e, u); B[7] = (uint32_t)uy(e, u);
 B[13] = p4; B[8] = p4; B[9] = B[0]; B[12] = p3; B[5] = p2; B[22] = e.cb23b60;
 if (p7) B[1] = 0x2021;
 for (int i = 0; i < count; i++) {
  B[23] = (uint32_t)i;
  P q = e.f630f0(B + 1);
  if (q) { if (p7) e.o10520(e.rd32(q + 0x2c), 0); e.o10441(q, p5); }
 }
 return 1;
}
// x87 80-bit operand (mantissa lo/hi, sign+exponent)
struct X80 { uint32_t lo, hi; uint16_t se; };
// 1.13c D2Client+0xC9EE0 (CRT fdiv special-case classifier)
uint32_t sub_C9EE0(const Env& e, X80 a, X80 b) {
 uint32_t lo = a.lo, hi = a.hi; uint16_t se = a.se;
 while (!(hi >> 31)) {
  if (!lo && !hi) return 0;
  if (se & 0x7fff) return se & 0x7fff;
  uint32_t x = b.se & 0x7fff;
  if (!x) { if (b.hi >> 31) return b.hi * 2; }
  else if (x == 0x7fff || !(b.hi >> 31)) return x == 0x7fff ? x : b.hi * 2;
  e.scale80(lo, hi, se);
 }
 uint32_t v = hi * 2 ^ 0xe000000u;
 if (v & 0xe000000u) return v;
 uint32_t i = (hi & 0x7fffffffu) >> 0x1b;
 if (e.rd8(e.tblAA420 + i)) {
  if ((se & 0x7fff) && (se & 0x7fff) != 0x7fff) return (b.se & 0x7fff) != 1 ? (b.se & 0x7fffu) : 1;
  return se & 0x7fff;
 }
 return i;
}
// 1.13c D2Client+0xCA496 (__fdivp_sti_st: ST0 first)
uint32_t sub_CA496(const Env& e, X80 st0, X80 st1) { return sub_C9EE0(e, st0, st1); }
// 1.13c D2Client+0xCA4A9 (__fdivrp_sti_st: ST1 first)
uint32_t sub_CA4A9(const Env& e, X80 st0, X80 st1) { return sub_C9EE0(e, st1, st0); }
} // namespace d2client_batch_c88a0
