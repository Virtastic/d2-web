// D2Client 0x88D90 / 0xC3DD0..0xC4960 batch: env-seam implementations from the decompile (0xC45C0 and 0xC4990..0xC4CE0 are their own files, d2client_<rva>.cpp).
// Ordinal_* / FUN_* callees are Env function pointers named by ordinal/RVA; retail memory via rd32/wr32.
// Registers the decompiler lost (unaff_*, in_EAX) are explicit parameters.
#include "d2client_batch_c3dd0.h"
namespace d2client_batch_c3dd0 {
static bool isTyped(int32_t t) { return t == 2 || (3 < t && t < 6); }
static P clsRow(const Env& e, int32_t c) {
 if (c < 0 || (int32_t)e.rd32(e.dataTables + 0xba0) <= c) return 0;
 return e.rd32(e.dataTables + 0xb98) + (P)c * 0x23c;
}
static uint32_t unitW10(const Env& e, P u) { // unit +0x2c: field +0x10 / O10750
 P p = e.rd32(u + 0x2c); return isTyped((int32_t)e.rd32(u)) ? e.rd32(p + 0x10) : (p ? e.o10750(p) : 0);
}
static uint32_t unitW0c(const Env& e, P u) {
 P p = e.rd32(u + 0x2c); return isTyped((int32_t)e.rd32(u)) ? e.rd32(p + 0xc) : (p ? e.o10867(p) : 0);
}
// 1.13c D2Client+0x88D90
uint32_t sub_88D90(const Env& e) {
 uint32_t g = e.rd32(e.gCC268), r = g / 3;
 if (g % 3 == 0) r = e.f38a70e(e.f39640(0x6bb) + 0xb76, 0, 0, 0, 0);   // ESI = 0x6BB into 0x89640; EBX = its result + 0xB76 into 0x88A70
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
} // namespace d2client_batch_c3dd0
