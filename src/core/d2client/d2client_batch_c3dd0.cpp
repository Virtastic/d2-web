// D2Client 0x88D90 / 0xC3DD0..0xC4CE0 batch: env-seam implementations from the decompile.
// Ordinal_* / FUN_* callees are Env function pointers named by ordinal/RVA; retail memory via rd32/wr32.
// Registers the decompiler lost (unaff_*, in_EAX) are explicit parameters.
#include <cstdint>
namespace d2client_batch_c3dd0 {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P); void (*wr32)(P, uint32_t);
 P dataTables;                       // *sgptDataTables
 P gCC268, gBB9CC, gBB9D0, gBB9D4, gBB9D8, gCBEFC, gC97F8; // DAT_6fbcc268, 6fbbb9cc.., 6fbcbefc, 6fbc97f8
 uint32_t (*tick)();                 // GetTickCount
 void (*writeFile)(P, const char*, uint32_t); // WriteFile after wvsprintfA
 void (*f39640)(); uint32_t (*f38a70)(P, int, int, int);
 uint32_t (*o10511)(P); int32_t (*o10786)(P, uint32_t, uint32_t, uint32_t);
 void (*o10243)(uint32_t, uint32_t); uint32_t (*o10494)(P, uint32_t); void (*o10945)(P, uint32_t, uint32_t);
 uint32_t (*o10750)(P); uint32_t (*o10867)(P);
 P (*f5ff10)(P, P, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
 uint8_t (*o10663)(); void (*o10706)(uint32_t, uint8_t); void (*o10520)(uint32_t, uint32_t);
 uint32_t (*o10331)(P, uint32_t, uint32_t); void (*o10591)(uint32_t, const uint32_t*, uint32_t); void (*o10052)(P*);
 P (*f630f0)(const uint32_t*); int32_t (*o10534)(P); void (*o10558)(P, int32_t); void (*o10970)(P, int32_t);
 void (*o10344)(P*);
 P (*f560d0)(P); void (*o11121)(P, P);
 void (*fada0b0)(); uint32_t (*o10265)(uint32_t); void (*o10024)(const char*, uint32_t); P (*o10392)(uint32_t);
 int32_t (*o10096)(P); uint32_t (*f1630)(P); uint32_t (*f1660)(P); void (*o10554)(uint32_t, uint32_t); void (*o10748)(uint32_t, uint32_t);
 uint32_t (*o10980)(uint32_t); uint32_t (*o10649)(uint32_t);
 int32_t (*f40550)(P); int32_t (*f51340)(int32_t*); P (*f515c0)(); P (*f51540)();
 void (*o10637)(P, int32_t); void (*o10019)(P, int32_t);
 int32_t (*o10080)(P, uint32_t, uint32_t); void (*o10425)(uint32_t, int32_t); void (*o10441)(P, int32_t); void (*o10837)(P, int32_t);
};
static bool isTyped(int32_t t) { return t == 2 || (3 < t && t < 6); }
static P clsRow(const Env& e, int32_t c) {
 if (c < 0 || (int32_t)e.rd32(e.dataTables + 0xba0) <= c) return 0;
 return e.rd32(e.dataTables + 0xb98) + (P)c * 0x23c;
}
static P rowAt(const Env& e, int32_t i) { return e.rd32(e.dataTables + 0xb64) + (P)i * 0x1a4; }
static bool inRows(const Env& e, int32_t i) { return -1 < i && i < (int32_t)e.rd32(e.dataTables + 0xb6c); }
static uint32_t unitW10(const Env& e, P u) { // unit +0x2c: field +0x10 / O10750
 P p = e.rd32(u + 0x2c); return isTyped((int32_t)e.rd32(u)) ? e.rd32(p + 0x10) : (p ? e.o10750(p) : 0);
}
static uint32_t unitW0c(const Env& e, P u) {
 P p = e.rd32(u + 0x2c); return isTyped((int32_t)e.rd32(u)) ? e.rd32(p + 0xc) : (p ? e.o10867(p) : 0);
}
// 1.13c D2Client+0x88D90
uint32_t sub_88D90(const Env& e) {
 uint32_t g = e.rd32(e.gCC268), r = g / 3;
 if (g % 3 == 0) { e.f39640(); r = e.f38a70(0, 0, 0, 0); }
 return r;
}
// 1.13c D2Client+0xC3DD0  (formatted text already produced by the caller's vsprintf)
void sub_C3DD0(const Env& e, P h, const char* text, uint32_t len) { e.writeFile(h, text, len); }
// 1.13c D2Client+0xC4370
uint32_t sub_C4370(const Env& e, P a, int32_t cls, uint32_t retaddr) {
 P r = clsRow(e, cls);
 if (!r) return 0;
 uint32_t h = e.o10511(a);
 if (!h) return 0;
 int32_t v = e.o10786(a, e.rd32(r + 0x13c), cls, retaddr);
 if (v < 1) v = 1;
 e.o10243(h, e.rd32(e.gC97F8) + (uint32_t)v);
 if (e.o10494(a, 0xc) == 0) e.o10945(a, 0xc, 1);
 return 1;
}
// 1.13c D2Client+0xC4410  (u = in_EAX, flag = unaff_ESI)
uint32_t sub_C4410(const Env& e, P u, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t flag) {
 uint32_t a = unitW10(e, u), b = unitW0c(e, u);
 P n = e.f5ff10(p2, u, p1, b, a, p2, p3);
 if (!n) return 0;
 uint8_t c = e.o10663();
 uint32_t q = e.rd32(n + 0x2c);
 e.o10706(q, c); e.o10520(q, 0);
 if (flag) e.wr32(n + 0xc4, e.rd32(n + 0xc4) | 0x10000);
 return 1;
}
// 1.13c D2Client+0xC45C0  (u = param_1, seed = u+0x20/+0x24, count = iStack_c4 of the 0x3002 query)
uint32_t sub_C45C0(const Env& e, P u, uint32_t p2, int32_t p3) {
 P cr = 0;
 if ((int32_t)e.rd32(e.dataTables + 0xba0) >= 0x11e) {
  P t = e.rd32(e.dataTables + 0xb98);
  if (p3 >= 1) cr = (P)((int32_t)e.rd32(t + 0x27e28) * (p3 - 1)) + e.rd32(t + 0x27e24);
 }
 uint32_t w = unitW0c(e, u), h = unitW10(e, u);
 uint32_t q[14] = {0};
 q[0] = 0x3002; q[2] = (uint32_t)u; q[3] = w; q[4] = h; q[5] = (uint32_t)cr; // apiStack_a0[0..], uStack_90, piStack_8c
 uint32_t k = e.o10331(u, w, h);
 uint32_t in = p2;
 e.o10591(0, &in, k);
 P list = 0; e.o10052(&list);
 int32_t count = (int32_t)q[13]; // iStack_c4 filled by the query
 for (int32_t i = 0; i < count;) {
  P o = e.rd32(list + (P)i * 4);
  uint32_t ah = unitW10(e, o);
  uint32_t S[23] = {0};
  S[0] = 1; S[1] = (uint32_t)u; S[4] = 0x24b; S[5] = unitW0c(e, o); S[6] = ah; S[7] = h; S[10] = (uint32_t)p3;
  int32_t idx = i;
  P r = e.f630f0(S);
  if (r) {
   idx = e.o10534(r);
   uint64_t s = (uint64_t)e.rd32(u + 0x20) * 0x6ac690c5ull + e.rd32(u + 0x24);
   e.wr32(u + 0x20, (uint32_t)s); e.wr32(u + 0x24, (uint32_t)(s >> 32));
   int32_t v = idx - 10 + (int32_t)((uint32_t)s % 0x14);
   e.o10558(r, v); e.o10970(r, v);
  }
  i = idx + 1;
 }
 P pl = cr; e.o10344(&pl);
 return 1;
}
// 1.13c D2Client+0xC4830
uint32_t sub_C4830(const Env& e, uint32_t, int32_t cls) {
 P r = clsRow(e, cls);
 uint32_t a = r ? e.rd32(r + 0x148) : 0;
 int32_t b = r ? (int32_t)e.rd32(r + 0x14c) : 0;
 int32_t c = r ? (int32_t)e.rd32(r + 0x150) : 0;
 int32_t d = r ? (int32_t)e.rd32(r + 0x154) : 0;
 c = (c * 1000) / 0x19;
 if (c != 0) {
  e.wr32(e.gBB9CC, e.tick());
  e.wr32(e.gBB9D8, (uint32_t)((d * 1000) / 0x19));
  e.wr32(e.gBB9D0, (uint32_t)((b * 1000) / 0x19));
  e.wr32(e.gBB9D4, (uint32_t)c);
  e.wr32(e.gCBEFC, a);
 }
 return 1;
}
// 1.13c D2Client+0xC4960
uint32_t sub_C4960(const Env& e, P a) {
 P p = e.f560d0(a);
 if (!p) return 0;
 e.o11121(a, p);
 return 1;
}
// 1.13c D2Client+0xC4990  (fatal path: caller's _exit(-1) is the seam's job; returns 0xFFFFFFFF marker as 2)
uint32_t sub_C4990(const Env& e, P a) {
 uint32_t h = e.o10511(a);
 if (!h) return 0;
 e.fada0b0();
 if (e.rd32(a + 0x2c) == 0) { e.o10024("", e.o10265(0x946)); return 2; }
 P u = e.o10392(e.rd32(a + 0x2c));
 if (!u || !e.o10096(u)) return 0;
 e.o10243(h, e.f1630(u)); e.o10554(h, e.f1660(u)); e.o10748(h, e.rd32(u + 4));
 return 1;
}
// 1.13c D2Client+0xC4A20
uint32_t sub_C4A20(const Env& e, P a) {
 uint32_t h = e.o10511(a);
 if (!h) return 0;
 e.fada0b0();
 if (e.rd32(a + 0x2c) == 0) { e.o10024("", e.o10265(0x946)); return 2; }
 P u = e.o10392(e.rd32(a + 0x2c));
 if (!u) return 0;
 e.o10243(h, unitW0c(e, u));
 e.o10554(h, unitW10(e, u));
 return 1;
}
// 1.13c D2Client+0xC4B00  (a = param_1, edi = unaff_EDI, esi = unaff_ESI)
uint32_t sub_C4B00(const Env& e, P a, int32_t cls, P edi, uint32_t esi) {
 P r = clsRow(e, cls);
 if (!r) return 0;
 uint32_t h = e.o10511(a);
 if (!h) return 0;
 uint32_t x = e.o10980(h), y = e.o10649(h);
 if (!x || !y) return 0;
 int32_t i = (int16_t)e.rd32(r + 0xea);
 if (inRows(e, i)) {
  P n = e.f5ff10(a, edi, (uint32_t)i, x, y, (uint32_t)cls, a);
  if (!n) return 0;
  if ((int16_t)e.rd32(r + 0x10e) > 0) { e.f38a70(n, 0, 0, 0); x = esi; }
 }
 i = (int16_t)e.rd32(r + 0xec);
 if (inRows(e, i)) e.f5ff10(e.dataTables, edi, (uint32_t)i, x, y, (uint32_t)cls, a);
 return 1;
}
// 1.13c D2Client+0xC4C10
uint32_t sub_C4C10(const Env& e, P u, int32_t cls) {
 if (!clsRow(e, cls)) return 0;
 int32_t i = e.f40550(u);
 if (!inRows(e, i)) return 0;
 int32_t pt[2];
 if (!e.f51340(pt)) return 0;
 P o = e.f515c0();
 if (!o) return 0;
 int32_t x = (int32_t)e.f1630(u), y = (int32_t)e.f1660(u);
 e.o10637(o, -(pt[0] - y));   // local_4 (decompile: first slot)
 e.o10019(o, pt[1] - x);      // local_8
 return 1;
}
// 1.13c D2Client+0xC4CE0  (edi = unaff_EDI, ebp0 = unaff_EBP, stk = uStack_8, e2c/e30 = iStack_2c/iStack_30 stack args)
uint32_t sub_C4CE0(const Env& e, P u, int32_t cls, P edi, int32_t ebp0, uint32_t stk, int32_t e2c, int32_t e30) {
 P cr = clsRow(e, cls);
 if (!cr) return 0;
 int32_t ri = (int16_t)e.rd32(cr + 0xec);
 if (!inRows(e, ri)) return 0;
 P row = rowAt(e, ri);
 int32_t l24[3];                               // local_24, local_20, local_1c
 if (!e.f51340(l24)) return 0;
 int32_t total = e.o10080(u, (uint32_t)l24[1], (uint32_t)l24[0]);
 if (total < 1) total = 1;
 int32_t ux = (int32_t)e.f1630(u), uy = (int32_t)e.f1660(u);
 int32_t dx = e2c - ux;
 int32_t step = (int32_t)e.rd32(row + 0x38); if (step < 1) step = 1;
 int32_t inc = 1; if ((int32_t)e.rd32(row + 0x3c) > 0) inc = (int32_t)e.rd32(row + 0x3c);
 int32_t ebp = ebp0;
 if (step < total) {
  int32_t ax = step * ((dx * 100) / total);
  int32_t ay = step * (((e30 - uy) * 100) / total);
  const int32_t ax0 = ax; int32_t cy = ay, k = step;
  do {
   P n = e.f5ff10(ax / 100 + ux, (P)u, (uint32_t)cr, ax / 100 + ux, cy / 100 + uy, (uint32_t)u, stk);
   if (n) {
    uint32_t c = e.o10663();
    e.o10425(e.rd32(n + 0x2c), (int32_t)c);
    int32_t b = e.o10534(n);
    e.o10558(n, (int32_t)u + b);
    e.o10970(n, (int32_t)c + b);
    e.o10837(n, b - (int32_t)n);
    e.wr32(n + 200, e.rd32(n + 200) | 0x40000);
    ax += ax0;
   } else ax += ax0;
   k += step; cy += ay; ebp += inc;
  } while (k < total);
 }
 int32_t ei = (int16_t)e.rd32((P)l24[0] + 0xea);
 if (inRows(e, ei)) {
  P o = e.f51540();
  if (!o) return 0;
  int32_t v = e.o10786(edi, e.rd32((P)l24[0] + 0x138), (uint32_t)u, stk);
  if (v > 0) e.o10441(o, v);
  int32_t b = e.o10534(o);
  int32_t t = ebp * (int32_t)edi + b;
  e.o10558(o, t); e.o10970(o, t);
  e.o10837(o, b - ebp * (int32_t)edi);
  e.wr32(o + 200, e.rd32(o + 200) | 0x40000);
 }
 return 1;
}
} // namespace d2client_batch_c3dd0
