// D2Client 0xC62E0..0xC76C0 batch: env-seam implementations from the decompile.
// Imported ordinals / unrecovered callees are Env function pointers (names positional: oNNNNN = Ordinal_NNNNN,
// fXXXX = FUN_6fb(a)XXXX); uniform signature, ints widened to P. Retail memory via rd32/wr32.
// Register-argument functions (C66A0, C7370, C76C0) are decompiled with unaff_* registers; the mapping below is best effort.
#include <cstdint>
namespace d2client_batch_c62e0 {
using P = uintptr_t;
using F = P (*)(P, P, P, P, P, P);
struct Env {
 uint32_t (*rd32)(P); void (*wr32)(P, uint32_t);
 P dataTables;                 // *sgptDataTables
 P bitMasks;                   // gdwBitMasks
 P gPlayerAddr;                // &DAT_6fbcbbfc
 P listHeadAddr;               // &DAT_6fbcbf30
 P cb509f0;                    // callback 0x6fb509f0
 const char* skillsBarPath;    // "..\\Source\\D2Client\\SKILLS\\SkillsBar.cpp"
 void (*da0b0)(); void (*fatal)(uint32_t);  // fatal: Ordinal_10265(id), Ordinal_10024, _exit(-1)
 F o10392, o10338, o10678, o10096, o10630, o10397, o10137, o10980, o10649, o10123, o10331, o11073, o11056,
   o11164, o10542, o10786, o10080, o10851, o10864, o10509, o10589, o10867, o10750, o10862, o10739,
   o10601, o10494, o11138, o10334, o10647, o10488, o10817, o10565, o10243, o10554, o10566, o10748, o11031, o10511;
 F f4c500, f25d0, f38550, f38bf0, f1400, f1630, f1660, f1260, f1230, f1300, f38a70, f51860, f630f0,
   f51340, f55f70, f53040, f53220, f3200, f75fe0;
};
static P sess(const Env& e, P c) {   // shared prologue: da0b0, fatal(0x946) if +0x2c==0, then Ordinal_10392(+0x2c)
 e.da0b0();
 uint32_t id = e.rd32(c + 0x2c);
 if (!id) { e.fatal(0x946); return 0; }
 return e.o10392(id, 0, 0, 0, 0, 0);
}
static P clsRow(const Env& e, int32_t c) {
 return (c < 0 || (int32_t)e.rd32(e.dataTables + 0xba0) <= c) ? 0 : e.rd32(e.dataTables + 0xb98) + (P)c * 0x23c;
}
static int16_t s16(const Env& e, P a) { return (int16_t)e.rd32(a); }
static bool isPlayer(const Env& e, P u) { return u == e.rd32(e.gPlayerAddr); }
// 1.13c D2Client+0xC62E0
uint32_t sub_C62E0(const Env& e, P c) {
 P p = sess(e, c);
 if (p && e.o10338(p, 0, 0, 0, 0, 0)) { e.wr32(c + 0xc4, e.rd32(c + 0xc4) | 0x40); e.f4c500(p, 0, 0, 0, 0, 0); return 1; }
 return 0;
}
// 1.13c D2Client+0xC6350
uint32_t sub_C6350(const Env& e, P c) {
 P p = sess(e, c);
 return p && e.o10678(p, 0, 0, 0, 0, 0);
}
// 1.13c D2Client+0xC63A0
uint32_t sub_C63A0(const Env& e, P c) {
 P p = sess(e, c);
 if (p) { e.wr32(c + 0xc4, e.rd32(c + 0xc4) | 0x40); e.f4c500(p, 0, 0, 0, 0, 0); }
 return p != 0;
}
// 1.13c D2Client+0xC6400
uint32_t sub_C6400(const Env& e, P c) {
 P p = sess(e, c);
 return p && e.o10096(p, 0, 0, 0, 0, 0);
}
// 1.13c D2Client+0xC6450  (a = unaff_EDI arg of FUN_6fad25d0)
void sub_C6450(const Env& e, P a) {
 P o = e.o10630(0, 0, 0, 0, 0, 0);
 if (o) { uint32_t v = (uint32_t)e.o10397(o, 0, 0, 0, 0, 0); if (v & 1) e.o10137(o, v | 2, 0, 0, 0, 0); }
 P k = e.f25d0(a, 0, 0, 0, 0, 0);
 if (!k) return;
 for (P n = e.rd32(e.listHeadAddr); n; n = e.rd32(n + 4)) {
  P ent = e.rd32(n);
  if (e.rd32(ent + 8) == k) { e.f38550(ent, 0, 0, 0, 0, 0); return; }
 }
}
// 1.13c D2Client+0xC64B0
uint32_t sub_C64B0(const Env& e, P u) {
 uint32_t cmd = e.rd32(u + 0x2c);
 P o = e.o10630(0, 0, 0, 0, 0, 0);
 if (!o || !cmd) return 0;
 uint32_t t = e.rd32(u);
 P m = t == 0 ? 0x80 : 0x100;
 e.o10980(o, 0, 0, 0, 0, 0); e.o10649(o, 0, 0, 0, 0, 0);
 uint32_t b = (uint32_t)e.o10397(o, 0, 0, 0, 0, 0) & 0xff;
 if ((b & 3) == 3) {
  P w = e.o10123(u, m, 0, 0, 0, 0);
  P r = e.o10331(u, 0, 0, w, 0, 0);
  e.o11073(r, 0, 0, 0, 0, 0);
  e.o11056(m, m, 0, 0, 0, 0);
  e.o11164(m, t == 0 ? 0x1c09 : 0x3c01, 0, 0, 0, 0);
  e.o10137(o, 0, 0, 0, 0, 0);
  P k = e.f25d0(u, 0, 0, 0, 0, 0);
  e.f38bf0(0, k, 0, 0, 0, 0);
  return 1;
 }
 if (b & 1) { e.o10542(u, 3, 0, 0, 0, 0); e.wr32(u + 0x48, 0x500); }
 return 1;
}
// 1.13c D2Client+0xC66A0  (cls = in_EAX, lim = unaff_EDI; px/py host in/out coords)
uint32_t sub_C66A0(const Env& e, P u, P cls, int32_t lim, int32_t* px, int32_t* py) {
 P row = clsRow(e, (int32_t)cls);
 if (!row) return 0;
 uint32_t t = e.rd32(u);
 int32_t ly;   // local_18
 if (t == 2 || (t > 3 && t < 6)) ly = (int32_t)e.rd32(e.rd32(u + 0x2c) + 0xc);
 else { P q = e.rd32(u + 0x2c); ly = q ? (int32_t)e.o10867(q, 0, 0, 0, 0, 0) : 0; }
 if (!(t == 2 || (t > 3 && t < 6)) && e.rd32(u + 0x2c)) e.o10750(e.rd32(u + 0x2c), 0, 0, 0, 0, 0);
 int32_t rx = (int32_t)e.rd32(row + 100);
 int32_t d = (int32_t)e.o10786(u, 0, 0, 0, 0, 0);
 int32_t dist = (int32_t)e.o10080(u, (P)(uint32_t)*px, (P)(uint32_t)*py, 0, 0, 0);
 if (dist < 2) dist = 1;
 if (d < dist) {
  int32_t oy = *py;
  *px = ((*px - rx) * d) / dist + rx;
  *py = ((oy - ly) * d) / dist + ly;
 }
 int32_t dy = *py - ly; dy = dy < -1 ? -2 : dy > 1 ? 2 : dy;   // iStack_14
 int32_t dx = rx - *px; dx = dx < -1 ? -2 : dx > 1 ? 2 : dx;   // iStack_30
 int32_t x = *px, y = *py;
 for (uint32_t s = 3;;) {
  P r = e.o10331(u, 0, 0, 0, 0, 0); (void)r;
  P g = e.f1400(0, 0, 0, 0, 0, 0);
  P m = e.o10589(u, 0x1c09, 1, 0, 0, 0);
  P h = e.o10509(g, s, m, 0, 0, 0);
  if (h) {
   P w = e.o10123(u, 0x1c09, 0, 0, 0, 0);
   if (!e.o10851(h, (P)(uint32_t)x, (P)(uint32_t)y, w, 0, 0) &&
       e.o10864((P)(uint32_t)rx, (P)(uint32_t)ly, (P)(uint32_t)x, (P)(uint32_t)y, h, 0x804) &&
       (int32_t)e.o10080(u, (P)(uint32_t)x, (P)(uint32_t)y, 0, 0, 0) <= lim) {
    *px = x; *py = y; return 1;
   }
  }
  if (--s == 2) { x = *px + dy; y = *py + dx; }
  else if (s == 1) { x = *px - dy; y = *py - dx; }
  else break;
 }
 if (t == 0) e.f3200(u, 0x13, 0, 0, 0, 0);
 return 0;
}
// 1.13c D2Client+0xC6940
uint32_t sub_C6940(const Env& e, P u) {
 uint32_t id = e.rd32(u + 0x2c);
 if (((int32_t)e.rd32(u + 0x38) & 0xffffff00) < 0x600) {
  if (((uint32_t)e.o10862(0, 0, 0, 0, 0, 0) & 0xf) > 0xd) { e.fatal(0xb60); return 0; }
  e.f38a70(u, 4, 0, 0, 0, 0);
  e.da0b0();
  if (id) {
   P p = e.o10392(id, 0, 0, 0, 0, 0);
   if (!p) return 1;
   P y = e.f1660(p, 0, 0, 0, 0, 0), x = e.f1630(p, 0, 0, 0, 0, 0);
   e.o10739(id, x, y, 0, 0, 0);
   return 1;
  }
 } else {
  e.da0b0();
  if (id) { e.o10392(id, 0, 0, 0, 0, 0); e.f51860(u, 0, 0, 3, 0xa783, e.cb509f0); return 1; }
 }
 e.fatal(0x946);
 return 0;
}
// 1.13c D2Client+0xC7000  (c = client, idx = class index)
uint32_t sub_C7000(const Env& e, P c, int32_t idx) {
 P row = clsRow(e, idx);
 if (!row) return 0;
 P p = sess(e, c);
 if (!p || !e.o10338(p, 0, 0, 0, 0, 0)) return 0;
 e.wr32(c + 0xc4, e.rd32(c + 0xc4) | 0x40);
 e.f1630(p, 0, 0, 0, 0, 0); e.f1660(p, 0, 0, 0, 0, 0);
 if (!e.o10331(p, 0, 0, 0, 0, 0)) return 0;
 P g = e.f1400(0, 0, 0, 0, 0, 0);
 if (!g) return 0;
 uint32_t S[23] = {0};
 if (!e.o10509(g, (P)&S[3], 2, 0x1000, 1, 0) || e.rd32(p) != 1) return 0;
 int32_t s = s16(e, row + 0xea);
 P q = e.f1260(0, 0, 0, 0, 0, 0);
 if (!q) return 0;
 uint32_t b = e.rd32(q + 5) & 0xff;
 if ((e.rd32(e.bitMasks + 0xc) & b) == 0) { if (e.rd32(e.bitMasks + 8) & b) s = s16(e, row + 0xec); }
 else s = s16(e, row + 0xee);
 if (s < 0 || s >= (int32_t)e.rd32(e.dataTables + 0xb6c)) return 0;
 for (auto& v : S) v = 0;
 S[0] = 0x2001;
 e.f630f0((P)S, 0, 0, 0, 0, 0); e.f630f0((P)S, 0, 0, 0, 0, 0);
 return 1;
}
// 1.13c D2Client+0xC7370
uint32_t sub_C7370(const Env& e, P u, P a2) {
 if (!u || (e.rd32(u) != 0 && e.rd32(u) != 1)) return 0;
 P o = e.o10630(0, 0, 0, 0, 0, 0);
 uint32_t cmd = e.rd32(u + 0x2c);
 if (!o || !cmd) return 0;
 const P path = (P)e.skillsBarPath;
 if (isPlayer(e, u)) {
  if (e.o10601(o, path, 0 /*line no. lost in decompile*/, 0, 0, 0) == a2 && ((uint32_t)e.o10397(o, 0, 0, 0, 0, 0) & 1)) return 0;
  if (e.o10494(u, 0x36, 0, 0, 0, 0)) return 0;
 }
 uint32_t buf[2];
 if (!e.f51340((P)buf, 0, 0, 0, 0, 0)) return 0;
 P k = e.f55f70(0, 0, 0, 0, 0, 0);
 if (k && e.o11138(u, k, 0, 0, 0, 0)) return (uint32_t)e.f53040(u, 0, 0, 0, 0, 0);
 bool z = e.rd32(u) == 0;
 P m = z ? 0x80 : 0x100;
 e.o11164(cmd, m, 0, 0, 0, 0);       // args positional (decompile lost one to a register)
 e.o10647(cmd, 7, 0, 0, 0, 0);
 P out = 0;
 if (e.o10334(cmd, u, 0, (P)&out, 0, 0)) {
  if (out == 0) {
   P t = e.f1230(0, 0, 0, 0, 0, 0);
   e.o10488(cmd, (uint32_t)(s16(e, t + 0x32) << 8), path, 0x49f, 0, 0);
  } else {
   P t = e.f1300(0, 0, 0, 0, 0, 0);
   if (t) e.o10488(cmd, (e.rd32(t + 0x40) & 0xff) << 8, path, 0x499, 0, 0);
  }
  e.o11164(cmd, 0x401, 0, 0, 0, 0);
  P tbl = 0;
  uint32_t n = (uint32_t)e.o10817(cmd, (P)&tbl, 0, 0, 0, 0);
  if (n) {
   P x = e.rd32(tbl + n * 4 - 4) & 0xffff, y = e.rd32(tbl + n * 4 - 2) & 0xffff;
   P w = e.o10123(u, m, 0, 0, 0, 0);
   P r = e.o10331(u, x, y, w, 0, 0);
   e.o10565(r, 0, 0, 0, 0, 0);
   // decompile names iVar5 (size reg) here; the command object o is the consistent receiver (cf. C7550/C76C0)
   e.o10137(o, 1, 0, 0, 0, 0); e.o10243(o, x, 0, 0, 0, 0); e.o10554(o, y, 0, 0, 0, 0);
   e.f53220(0, 0, 0, 0, 0, 0);
   return 1;
  }
 }
 e.o11164(cmd, z ? 0x1c09 : 0x3c01, 0, 0, 0, 0);
 return 0;
}
// 1.13c D2Client+0xC7550
uint32_t sub_C7550(const Env& e, P u, P a2) {
 P o = e.o10630(0, 0, 0, 0, 0, 0);
 if (!o) return 0;
 if (isPlayer(e, u)) {
  if (e.o10494(u, 0x36, 0, 0, 0, 0)) return 0;
  if (e.o10601(o, (P)e.skillsBarPath, 0x3a2, 0, 0, 0) == a2 && ((uint32_t)e.o10397(o, 0, 0, 0, 0, 0) & 1)) return 0;
 }
 P p = sess(e, u);
 uint32_t o1[2] = {0, 0}, o2 = 0;
 if (!e.o10566(u, p, (P)o1, (P)&o2, 0, 0)) {
  if (p && e.o11138(u, p, 0, 0, 0, 0)) return (uint32_t)e.f53040(u, 0, 0, 0, 0, 0);
  if (e.rd32(u) != 1) e.f3200(u, 0x13, 0, 0, 0, 0);
  return 0;
 }
 P q = e.o10331(u, 0, 0, 0, 0, 0);
 if (!q) return 0;
 P w = e.o10123(u, 0, 0, 0, 0, 0);
 e.o10565(q, (P)o1, (P)&o2, w, 0x80, 0);
 e.o10243(o, 0x80, 0, 0, 0, 0);
 e.o10554(o, w, 0, 0, 0, 0);
 if (!p) e.o10748(o, 6, 0, 0, 0, 0);
 else { e.o10748(o, e.rd32(p), 0, 0, 0, 0); e.o11031(o, e.rd32(p + 12), 0, 0, 0, 0); }
 e.o10137(o, 0x1080, 0, 0, 0, 0);
 e.f53220(0, 0, 0, 0, 0, 0);
 return 1;
}
// 1.13c D2Client+0xC76C0  (lim = stack arg forwarded to C66A0)
uint32_t sub_C76C0(const Env& e, P u, P a2, int32_t lim) {
 P o = e.o10511(u, 0, 0, 0, 0, 0);
 if (!o) return 0;
 if (isPlayer(e, u)) {
  if (e.o10494(u, 0x36, 0, 0, 0, 0)) return 0;
  if (e.o10601(o, (P)e.skillsBarPath, 0x1ba, 0, 0, 0) == a2 && ((uint32_t)e.o10397(o, 0, 0, 0, 0, 0) & 1)) return 0;
 }
 bool one = (u ? e.rd32(u) : 6) == 1;
 int32_t xy[2] = {0, 0};
 uint32_t buf[2];
 int32_t x, y;
 if (!one) {
  if (!e.f51340((P)buf, 0, 0, 0, 0, 0)) return 0;
  xy[0] = (int32_t)buf[0]; xy[1] = (int32_t)buf[1];
  if (!e.f75fe0(u, 0, 0, 0, 0, 0)) return 0;
  if (!sub_C66A0(e, u, e.rd32(u + 4), lim, &xy[0], &xy[1])) return 0;
  x = xy[0]; y = xy[1];
 } else {
  P k = e.f55f70(0, 0, 0, 0, 0, 0);
  if (!k) {
   if (!e.f51340((P)buf, 0, 0, 0, 0, 0)) return 0;
   e.o11031(o, 0xffffffffu, 0, 0, 0, 0);
   x = (int32_t)buf[0]; y = (int32_t)buf[1];
  } else {
   x = 2 * (int32_t)e.f1630(k, 0, 0, 0, 0, 0) - (int32_t)e.f1630(u, 0, 0, 0, 0, 0);
   y = 2 * (int32_t)e.f1660(k, 0, 0, 0, 0, 0) - (int32_t)e.f1660(u, 0, 0, 0, 0, 0);
  }
  int32_t pt[2] = {x, y};
  P mk = e.o10589(u, 0x3c01, 0, 0, 0, 0);
  P r = e.o10331(u, (P)pt, mk, 0, 0, 0);
  if (!e.o10509(r, 0, 0, 0, 0, 0)) return 0;
 }
 e.o10331(u, 0, 0, 0, 0, 0);
 e.f1400(0, 0, 0, 0, 0, 0);
 P w = e.o10123(u, 0x80, 0, 0, 0, 0);
 e.o10565(one, (P)(uint32_t)x, (P)(uint32_t)y, w, 0, 0);
 e.o10243(o, (P)(uint32_t)x, 0, 0, 0, 0);
 e.o10554(o, (P)(uint32_t)y, 0, 0, 0, 0);
 if (u) { P k = e.f55f70(0, 0, 0, 0, 0, 0); if (k) { e.o10748(o, e.rd32(k), 0, 0, 0, 0); e.o11031(o, e.rd32(k + 12), 0, 0, 0, 0); } }
 e.o10137(o, 0x1080, 0, 0, 0, 0);
 e.f53220(0, 0, 0, 0, 0, 0);
 return 1;
}
}
