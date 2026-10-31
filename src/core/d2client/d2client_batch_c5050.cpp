// D2Client 0xC5050..0xC6150 batch: env-seam implementations from the decompile.
// Imported ordinals / unrecovered callees are Env function pointers (names positional guesses); retail memory via rd32/wr32.
#include <cstdint>
namespace d2client_batch_c5050 {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P); void (*wr32)(P, uint32_t);
 P dataTables;                 // *sgptDataTables
 P bitMasks;                   // gdwBitMasks
 P tblBA608;                   // DAT_6fbba608 (0x80/row)
 P tbl87078; P tbl87098;       // DAT_6fb87078 / DAT_6fb87098 (8 entries each)
 P cbLab74290; P cb23b60;      // LAB_6fb74290, FUN_6fb23b60
 // D2Client internals
 P (*f51540)(); int32_t (*f51340)(int32_t*); P (*f55f70)(); P (*f630f0)(const uint32_t*);
 void (*f77b00)(); void (*f1a6b0)(int32_t*, int32_t*); void (*f77e30)(int32_t, int32_t, int32_t);
 void (*f77fb0)(P); int32_t (*f77ec0)(); int32_t (*faf0550)(P);
 void (*f74410)(int32_t, int32_t, P); P (*f699d0)(P); P (*f54e20)(uint32_t, P, uint32_t);
 void (*f61420)(P, int); void (*fafc500)(P); P (*fa1400)(P);
 uint32_t (*f1630)(P); uint32_t (*f1660)(P); uint32_t (*f1330)(P);
 int32_t (*f50800)(int32_t*); void (*f507c0)(); void (*f23bc0)();
 P (*fcc00)(P, P);
 // imported ordinals
 uint32_t (*o10630)(); void (*o10302)(P, uint32_t, uint32_t, uint32_t, const char*, uint32_t);
 int32_t (*o10601)(uint32_t, const char*, uint32_t); int32_t (*o10306)(P, uint32_t, uint32_t);
 int32_t (*o10980)(uint32_t); void (*o10243)(uint32_t, int32_t); int32_t (*o10649)(uint32_t); int32_t (*o10179)(uint32_t);
 int32_t (*o10786)(P, uint32_t, int32_t, int32_t); int32_t (*o10080)(P, int32_t, int32_t);   // 10080(unit, p2, p3)
 P (*o10331)(P); P (*o10331a)(P, uint32_t, uint32_t, P, uint32_t, uint32_t); P (*o10389)(P);
 void (*o10554)(uint32_t, uint32_t); void (*o10748)(uint32_t, uint32_t);
 void (*o10065)(); void (*o11087)(int32_t*, int32_t*);
 int32_t (*o10534)(P); int32_t (*o10494)(P, uint32_t); void (*o10542)(P, uint32_t);
 void (*o10637)(P, int32_t); void (*o10019)(P, int32_t); int32_t (*o10057)(P);
 int32_t (*o10867)(P); int32_t (*o10750)(P); int32_t (*o10179r)(P); uint32_t (*o10631)();
 void (*o10137)(); void (*o10054)(P, int); void (*o10770)(P); void (*o10168)(P, int); void (*o11005)(P, int);
 void (*o10336)(P, int); void (*o10286)(P, int32_t, int32_t); void (*o10647)(P, int); void (*o10527)(P, int);
 void (*o10334)(P, P, int);
 int32_t (*o10042)(); void (*o10706)(uint32_t, uint32_t); int32_t (*o10013)(); void (*o10651)(); void (*o11142)();
 void (*o11168)(uint32_t*); uint32_t (*o11080)(); void (*o10002)(uint32_t);
};
static int32_t s16(const Env& e, P a) { return (int16_t)e.rd32(a); }
static P clsRow(const Env& e, int32_t c) {
 return (c < 0 || (int32_t)e.rd32(e.dataTables + 0xba0) <= c) ? 0 : e.rd32(e.dataTables + 0xb98) + (P)c * 0x23c;
}
static bool skOk(const Env& e, int32_t i) { return i >= 0 && i < (int32_t)e.rd32(e.dataTables + 0xb6c); }
static P skRow(const Env& e, int32_t i) { return skOk(e, i) ? e.rd32(e.dataTables + 0xb64) + (P)i * 0x1a4 : 0; }
static void zero23(uint32_t* s) { for (int i = 0; i < 23; i++) s[i] = 0; }

// 1.13c D2Client+0xC5050
uint32_t sub_C5050(const Env& e, uint32_t, int32_t cls) {
 P r = clsRow(e, cls);
 if (!r) return 0;
 if (skOk(e, s16(e, r + 0xec))) {
  if (!e.f51540()) return 0;
  if (e.f55f70()) { int32_t x = 0, y = 0; e.f77b00(); e.f1a6b0(&x, &y); e.f77e30(-x, 0, y); }
 }
 return 1;
}
// 1.13c D2Client+0xC50F0
uint32_t sub_C50F0(const Env& e, P u, uint32_t a2) {
 if (!u || e.rd32(u) != 1) return 0;
 uint32_t h = e.o10630();
 if (!h) { e.o10302(u, 0x126, 1, 0, "..\\Source\\D2Client\\SKILLS\\SkillsEMon.cpp", 0x35c); h = e.o10630(); if (!h) return 0; }
 int32_t cls = e.o10601(h, "..\\Source\\D2Client\\SKILLS\\SkillsEMon.cpp", 0x364);
 int32_t r306 = e.o10306(u, h, 1);                 // its return is the 4th argument of both 10786 calls (not param1)
 int32_t n = e.o10980(h);
 e.o10243(h, n + 1);
 int32_t p2 = e.o10649(h), p3 = e.o10179(h);
 P r = clsRow(e, cls);
 if (!r) return 0;
 int32_t flag = 0;
 if (n >= e.o10786(u, e.rd32(r + 0x118), cls, r306)) flag = 1;       // 0xC5 :751e6 jl skip
 int32_t calc2 = e.o10786(u, e.rd32(r + 0x118), cls, r306);          // second call, same arguments
 if (e.o10080(u, p2, p3) >= calc2) flag = 1;                         // :7521b jl skip
 int32_t v;
 if (e.rd32(u + 0x10) == 1) {
  if (flag) {
   P t = e.o10331a(u, e.f1630(u), e.f1660(u), e.cbLab74290, 0, 1);
   P w = e.o10389(t);
   if (w) e.f61420(w, 0);
  }
  v = s16(e, r + 0xf0);
 } else {
  if (e.rd32(u + 0x10) != 2) return 1;
  v = s16(e, r + 0xee) + (int32_t)e.f1330(u + 0x20);
 }
 if (flag && v > 0) {
  e.o10243(h, 0); e.o10554(h, e.f1630(u)); e.o10748(h, e.f1660(u));
  e.f74410(v, cls, u);
 }
 (void)a2;
 return 1;
}
// 1.13c D2Client+0xC52E0
uint32_t sub_C52E0(const Env& e, P u, int32_t cls) {
 P r = clsRow(e, cls);
 if (!r) return 0;
 P ur = skRow(e, e.faf0550(u));
 if (!ur) return 0;
 e.o10065();
 P w = e.f51540();
 if (!w) return 0;
 int32_t m = s16(e, ur + 0x96); if (m < 1) m = 1;
 int32_t n = e.o10786(u, e.rd32(r + 0x114), cls, 0);
 int32_t a = 0, b = 0;
 e.o11087(&a, &b); e.o11087(&b, &a);
 e.f77e30(n * m, (int32_t)u, 0);
 P q = e.rd32(w + 0x54);
 if (q) { P s = e.rd32(q + 0x30); if (s) { e.wr32(s + 0x10, (uint32_t)(a * 0x800)); e.wr32(s + 0x14, (uint32_t)(b * 0x800)); e.wr32(s + 0x18, (uint32_t)(n * -0x800)); } }
 e.f77fb0(w);
 return 1;
}
// 1.13c D2Client+0xC5480
uint32_t sub_C5480(const Env& e, P u, int32_t cls, int32_t a3) {
 P r = clsRow(e, cls);
 if (!r || !skOk(e, e.faf0550(u))) return 0;
 int32_t pt[2] = {0, 0};
 if (!e.f51340(pt)) return 0;
 int32_t step = e.rd32(r + 0x138);
 int32_t cnt = e.o10786(u, step, cls, a3);
 int32_t dx = pt[0] - (int32_t)e.f1630(u); e.f1660(u);
 e.f50800(pt); e.f507c0();
 int32_t pos = pt[0] - (dx * cnt) / 2;
 uint32_t S[23]; zero23(S);
 S[0] = 0x20; S[3] = (uint32_t)u; S[4] = (uint32_t)u;
 S[11] = (uint32_t)e.o10786(u, e.rd32(r + 0x13c), cls, a3);
 for (int32_t i = cnt; i > 0; i--) {
  S[2] = (uint32_t)pos; S[3] = (uint32_t)u;
  P p = e.f630f0(S);
  if (p) { e.o10534(p); e.f23bc0(); }
  pos += step;
 }
 if (e.o10494(u, 0xc)) { e.o10542(u, 8); e.wr32(u + 0x48, 0x600); }
 return 1;
}
// 1.13c D2Client+0xC5670
uint32_t sub_C5670(const Env& e, uint32_t, int32_t cls, uint32_t a3) {
 P r = clsRow(e, cls);
 if (!r || !skOk(e, s16(e, r + 0xec))) return 0;
 P w = e.f55f70();
 if (!w) return 0;
 e.fafc500(w);
 uint32_t S[23]; zero23(S);
 S[4] = (uint32_t)s16(e, r + 0xec); S[0x1a / 2 + 0] = 0;
 S[11] = a3; S[1] = (uint32_t)w; S[2] = (uint32_t)w; S[7] = (uint32_t)cls;
 e.f630f0(S);
 return 1;
}
// 1.13c D2Client+0xC5730
uint32_t sub_C5730(const Env& e, uint32_t, uint32_t a2, int32_t cnt, uint32_t a4, uint32_t a5) {
 uint32_t S[23]; zero23(S);
 S[0] = 3; S[7] = a4; S[8] = a5; S[4] = a2;
 int32_t pt[2];
 if (!e.f51340(pt)) return 0;
 S[5] = (uint32_t)pt[0]; S[6] = (uint32_t)pt[1];
 for (uint32_t i = 0; (int32_t)i < cnt; i++) {
  S[9] = e.rd32(e.tbl87078 + (i & 7) * 4); S[10] = e.rd32(e.tbl87098 + (i & 7) * 4);
  S[22] = i;
  e.f630f0(S);
 }
 return 1;
}
// 1.13c D2Client+0xC5A80
uint32_t sub_C5A80(const Env& e, P u, int32_t cls, uint32_t a3) {
 if (!clsRow(e, cls)) return 0;
 int32_t sk = e.faf0550(u);
 if (!skOk(e, sk)) return 0;
 int32_t pt[2];
 if (!e.f51340(pt)) return 0;
 if (u) e.wr32(u + 0xc4, e.rd32(u + 0xc4) | 0x40);
 uint32_t S[23]; zero23(S);
 S[0] = 2; S[1] = (uint32_t)u; S[2] = (uint32_t)u; S[4] = (uint32_t)sk; S[7] = (uint32_t)cls; S[11] = a3;
 int32_t dx = pt[0] - (int32_t)e.f1630(u), dy = pt[1] - (int32_t)e.f1660(u);
 S[6] = (uint32_t)dx; S[5] = (uint32_t)dy;
 P p = e.f630f0(S);
 if (!p) return 0;
 e.o10637(p, -dy); e.o10019(p, dx);
 return 1;
}
// 1.13c D2Client+0xC5C70
uint32_t sub_C5C70(const Env& e, P u, int32_t cls, uint32_t a3) {
 P r = clsRow(e, cls);
 if (!r) return 0;
 if (u) e.wr32(u + 0xc4, e.rd32(u + 0xc4) | 0x40);
 int32_t s = s16(e, r + 0xea);
 if (skOk(e, s)) e.f74410(s, cls, a3);
 if (skOk(e, s16(e, r + 0xec))) {
  int32_t n = e.o10786(u, e.rd32(r + 0x114), cls, a3);
  sub_C5730(e, 0, 0, n, (uint32_t)cls, a3);
 }
 return 1;
}
// 1.13c D2Client+0xC5D20
uint32_t sub_C5D20(const Env& e, P u, int32_t cls, uint32_t a3) {
 if (!clsRow(e, cls)) return 0;
 int32_t sk = e.faf0550(u);
 if (!skOk(e, sk)) return 0;
 int32_t pt[2];
 if (!e.f51340(pt)) return 0;
 if (u) e.wr32(u + 0xc4, e.rd32(u + 0xc4) | 0x40);
 P p = e.f699d0(a3);
 if (!p) return 0;
 int32_t d = e.o10534(p);
 e.o10786(u, e.rd32(skRow(e, sk) + 0x118), cls, 0);
 int32_t t = e.f77ec0() + pt[0];
 int32_t h = d == 0 ? t / 2 : t / d;
 e.f77e30(t, 0, 0);
 P q = e.rd32(p + 0x54);
 if (q) { P s = e.rd32(q + 0x30); if (s) { e.wr32(s + 0x10, 0); e.wr32(s + 0x14, 0); e.wr32(s + 0x18, (uint32_t)(h * -0x800)); } }
 e.f77fb0(p);
 return 1;
}
// 1.13c D2Client+0xC5E50
void sub_C5E50(const Env& e, P self, int32_t a1) {
 uint32_t h = e.rd32(self + 0x2c);
 if (!h) return;
 uint8_t c = (uint8_t)e.o10042();
 uint8_t b = (uint8_t)(c + (a1 == 0 ? 0x2a : 0x15));
 e.o10706(h, b & 0x3f);
 if (!e.o10013()) { e.o10651(); e.o11142(); }
 else { uint32_t t = h; e.o11168(&t); e.o10002(e.o11080()); }
 e.fcc00(self, 0);
 e.o10706(h, c);
}
// 1.13c D2Client+0xC5FE0
uint32_t sub_C5FE0(const Env& e, int32_t cls, P a1) {
 P r = clsRow(e, cls);
 if (!r || !e.o10331(a1)) return 0;
 P x = e.fa1400(a1);
 if (!x) return 0;
 if (!((uint8_t)e.rd32(e.bitMasks) & (uint8_t)e.rd32(r + 5)) && e.o10057(x)) return 0;
 return 1;
}
// 1.13c D2Client+0xC6150
void sub_C6150(const Env& e, P self) {
 P h = e.rd32(self + 0x2c);
 if (!h) return;
 uint32_t k = (uint32_t)e.o10179r(h);
 P e54 = e.f54e20(e.o10631(), e.tblBA608 + k * 0x80, k);
 if (!e54) return;
 e.o10137(); e.o10542(self, 0xc);
 e.wr32(self + 0x48, (e.rd32(self + 0x48) & 0xffffff00u) + 0x100);
 e.o10054(self, 0); e.o10770(h); e.o10168(h, 5); e.o11005(h, 1); e.o10336(h, 0);
 P m = e.rd32(e54 + 0x2c);
 uint32_t t = e.rd32(e54);
 bool alt = t == 2 || (t > 3 && t < 6);
 int32_t w = alt ? (int32_t)e.rd32(m + 0xc) : (m ? e.o10867(m) : 0);
 int32_t hh = alt ? (int32_t)e.rd32(m + 0x10) : (m ? e.o10750(m) : 0);
 e.o10331(self);
 P x = e.fa1400(self);
 if (!x || !e.o10057(x)) {
  e.o10286(h, (int32_t)h * 3 - w * 2, (int32_t)self * 3 - hh * 2);
  e.o10647(h, 0xb); e.o10527(h, 5); e.o10334(h, self, 0);
 }
}
} // namespace d2client_batch_c5050
