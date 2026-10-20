// D2Client 0xAFD10..0xB0DF0 batch: env-seam implementations from the decompile.
// Imported ordinals are Env function pointers (names are positional guesses); retail memory via rd32/wr32.
#include <cstdint>
namespace d2client_batch_afd10 {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P); void (*wr32)(P, uint32_t);
 P dataTables;            // *sgptDataTables
 uint32_t bitMask8;       // gdwBitMasks[8]
 P playerSelf;            // DAT_6fbcbbfc
 const int32_t* tbl83cb0; // 3 rows x 5 dwords: [count, 4 entries]
 const int32_t* tblY83cec; const int32_t* tblX83d0c; // 8 dwords each
 void (*o10034)(uint32_t); void (*f3cb00)();
 int32_t (*o10985)(); int32_t (*o10191)(P); uint32_t (*o11029)(P); void (*o10240)(P, uint32_t);
 int32_t (*o10973)(P); void (*o10887)(P, int32_t, int32_t, int32_t);
 P (*f560d0)(uint32_t); P (*f630f0)(uint32_t kind, P unit);
 uint32_t (*o10042)(uint32_t, uint32_t); void (*o10706)(uint32_t, uint32_t);
 int32_t (*o10867)(P); int32_t (*o10750)(P); uint32_t (*o10818)(P);
 void (*o10286)(P, int32_t, int32_t); void (*o10558)(P, int32_t); void (*o10970)(P);
 void (*o10336)(uint32_t, P); int32_t (*o10637)(); int32_t (*o10080)(); int32_t (*o10522)(); void (*o10334)(uint32_t);
 void (*o10739)(uint32_t, int32_t, int32_t, int32_t);
 int32_t (*o10910)(P); int32_t (*o10587)(); void (*o10614)();
 int32_t (*o10534)(P); uint32_t (*o10777)(uint32_t); void (*o10488)(int32_t, uint32_t, const char*, int32_t);
 void (*f77c80)(int32_t, int32_t); void (*f1380)(P); void (*f58be0)(P);
};
static P row(const Env& e, int32_t i) {
 return (i < 0 || (int32_t)e.rd32(e.dataTables + 0xb6c) <= i) ? 0 : e.rd32(e.dataTables + 0xb64) + (P)i * 0x1a4;
}
static P unitRow(const Env& e, P u) { return u ? row(e, (int32_t)e.rd32(u + 4)) : 0; }
static bool pathDirect(const Env& e, P u) { int32_t t = (int32_t)e.rd32(u); return t == 2 || (t > 3 && t < 6); }
static int32_t posX(const Env& e, P u) { P p = e.rd32(u + 0x2c); return pathDirect(e, u) ? (int32_t)e.rd32(p + 0xc) : p ? e.o10867(p) : 0; }
static int32_t posY(const Env& e, P u) { P p = e.rd32(u + 0x2c); return pathDirect(e, u) ? (int32_t)e.rd32(p + 0x10) : p ? e.o10750(p) : 0; }
static uint64_t lcg(uint64_t s) { return (s & 0xffffffff) * 0x6ac690c5ull + (s >> 32); }
static uint64_t seedStep(const Env& e, P u) { // 64-bit seed at +0x20
 uint64_t s = lcg(e.rd32(u + 0x20) | (uint64_t)e.rd32(u + 0x24) << 32);
 e.wr32(u + 0x20, (uint32_t)s); e.wr32(u + 0x24, (uint32_t)(s >> 32)); return s;
}
static uint32_t rnd(const Env& e, P u, int32_t n) { // 0 when n<=0
 if (n < 1) return 0;
 uint32_t s = (uint32_t)seedStep(e, u);
 return ((uint32_t)n & ((uint32_t)n - 1)) == 0 ? ((uint32_t)n - 1) & s : s % (uint32_t)n;
}
// 1.13c D2Client+0xAFD10
uint32_t sub_AFD10(const Env& e, uint32_t a) { e.o10034(a); e.f3cb00(); return 1; }
// 1.13c D2Client+0xAFF80  (in_EAX = v)
int32_t sub_AFF80(int32_t v) {
 if (v < 0x96) return 0x14; if (v < 200) return 0xf; if (v < 0xe1) return 10;
 return (((0x176 < v) - 1) & 0xffffffec) + 0x19;
}
// 1.13c D2Client+0xB0000  (in_EAX = v, param_1 = p)
int32_t sub_B0000(const Env& e, int32_t v, P p) {
 if (0x1d < v) {
  if (v < 0x55) { int32_t d = v - 0xf; return (d + ((d >> 31) & 3)) >> 2; }
  int32_t m = (int16_t)e.rd32(p + 0x96) - 0x32;
  if (v < m) return 0xc;
  if (v < (int16_t)e.rd32(p + 0x96)) { int32_t d = v - m; return 0xc - ((d + ((d >> 31) & 3)) >> 2); }
 }
 return 1;
}
// 1.13c D2Client+0xB0050
int32_t sub_B0050(const Env& e) {
 int32_t v = e.o10985();
 if (v < 0x7d) return 0x19; if (v < 0xaf) return 10;
 return (((299 < v) - 1) & 0xfffffff4) + 0xf;
}
// 1.13c D2Client+0xB0100  (u = implicit unit register of the ordinals)
uint32_t sub_B0100(const Env& e, P u) {
 P r = row(e, e.o10191(u));
 if (!(e.bitMask8 & (e.rd32(r + 4) & 0xff))) return 3;
 if (e.o10973(u)) { e.o10887(u, 0, 0, 0); return 2; }
 return 3;
}
// 1.13c D2Client+0xB02E0  (in_EAX = a, ESI = u)
void sub_B02E0(const Env& e, P a, P u) {
 if (!u || !unitRow(e, u)) return;
 if (a && !(e.rd32(a + 0xc4) & 0x10000)) {
  int32_t t = (int32_t)e.rd32(a), m = (int32_t)e.rd32(a + 0x10);
  if (t == 0 ? (m == 0 || m == 0x11) : t == 1 ? (m == 0 || m == 0xc) : false) a = 0;
 } else if (a) a = 0;
 e.o11029(a); e.o10558(a, 0); e.o10970(a);
 uint32_t tgt = e.rd32(u + 0x2c);
 int32_t r;
 if (!a) {
  uint32_t xy = e.o10818(u);
  e.o10286(u, (int16_t)xy + posX(e, u), (int16_t)(xy >> 16) + posY(e, u));
  e.o10637(); r = e.o10080();
 } else { e.o10336(tgt, a); e.o10637(); r = e.o10522(); }
 if (r < 0x19) e.o10334(tgt);
}
// 1.13c D2Client+0xB0450  (p1 = ECX, u = ESI; returns object or 0)
P sub_B0450(const Env& e, uint32_t p1, P u) {
 if (!u) return 0;
 int32_t t = (int32_t)e.rd32(u);
 if (t != 0 && t != 1) { u = e.f560d0(p1); if (!u) return 0; }
 return e.f630f0(0x20, u);
}
// 1.13c D2Client+0xB0870
uint32_t sub_B0870(const Env& e, P u) {
 P r = unitRow(e, u);
 if (!r) return 1;
 int32_t bx = posX(e, u), by = posY(e, u);
 if ((int16_t)e.rd32(r + 0x2c) >= 0) { e.o10240(u, e.o11029(u)); sub_B0450(e, 0, u); }
 if ((int16_t)e.rd32(r + 0x2e) >= 0) {
  uint64_t s1 = seedStep(e, u);
  uint64_t s2 = seedStep(e, u);
  const int32_t* tr = e.tbl83cb0 + (uint32_t)s1 % 3 * 5;
  for (int32_t i = 0; i < tr[0]; i++) {
   uint32_t k = (tr[1 + i] + ((uint32_t)s2 & 7)) & 7;
   e.o10240(u, e.o11029(u));
   P o = sub_B0450(e, (int16_t)e.rd32(r + 0x2e), u);
   if (o) e.o10739(e.rd32(o + 0x2c), e.tblX83d0c[k] + bx, e.tblY83cec[k] + by, 0);
  }
 }
 return 1;
}
// 1.13c D2Client+0xB0A20  (in_EAX = a)
void sub_B0A20(const Env& e, P a) {
 uint32_t u1 = e.o11029(a); e.o10240(a, u1);
 P o = sub_B0450(e, 0, a);
 if (o) e.o10706(e.rd32(o + 0x2c), e.o10042(e.rd32(a + 0x2c), u1));
}
// 1.13c D2Client+0xB0A60  (in_EAX = a, param_1 = p1)
void sub_B0A60(const Env& e, P a, uint32_t p1) {
 if (a && e.rd32(a) > 1) return;
 if (!(e.bitMask8 & (e.rd32(row(e, e.o10191(p1)) + 4) & 0xff))) return;
 uint64_t seed = (uint64_t)0x9c << 32 | (uint32_t)a; // ponytail: low dword of the pointer token
 int32_t n = e.o10910(a) + e.o10973(a), chance = n, i = 0;
 if (!n) return;
 if (!a || e.rd32(a) != 0 || e.playerSelf == a) chance = e.o10587(); else chance = 100;
 e.o10614();
 while (i < 4 && (seed = lcg(seed), (int32_t)((uint32_t)seed % 100) < chance)) i++;
 e.o10887(a, 0x148, i, 0);
}
// 1.13c D2Client+0xB0B70
uint32_t sub_B0B70(const Env& e, P u) {
 int32_t a[18] = {0, 0, 0, 1, 1, 1, 0, -1, -1, -1, 1, 1, 0, -1, -1, -1, 0, 1}; // [0],[1] unrecovered: guessed 0
 int32_t bx = posX(e, u), by = 0;
 if (!pathDirect(e, u) && e.rd32(u + 0x2c)) e.o10750(e.rd32(u + 0x2c));
 e.o10240(u, e.o11029(u)); sub_B0450(e, 0, u);
 for (int i = 0; i < 8; i++) {
  e.o10240(u, e.o11029(u));
  P o = sub_B0450(e, 0, u);
  if (o) e.o10739(e.rd32(o + 0x2c), a[i] + bx, a[i + 8] + by, 0);
  e.o10240(u, e.o11029(u));
  o = sub_B0450(e, 0, u);
  if (o) {
   int32_t n = e.o10534(o);
   e.o10558(o, n - (int32_t)rnd(e, u, n / 2));
   by = (int32_t)e.rd32(o + 0x2c);
   uint32_t m = e.o10777((uint32_t)by);
   e.o10488(bx, rnd(e, u, (int32_t)m), "..\\Source\\D2Client\\UNIT\\Missile.cpp", 0x1a6b);
   e.f77c80(bx, 0x1e);
  }
 }
 return 1;
}
// 1.13c D2Client+0xB0DF0
void sub_B0DF0(const Env& e, P u) {
 P r = unitRow(e, u);
 P v;
 if (!(e.rd32(u + 0x44) & 0x300) && (v = e.rd32(u + 100)) &&
     (int32_t)(e.rd32(r + 0x130) & 0xff) <= ((int32_t)e.rd32(v + 0x18) >> 3)) {
  e.f1380(u + 0x20); e.f58be0(v);
 }
}
} // namespace d2client_batch_afd10
