// D2Client 0xBEA10..0xC0390 batch: env-seam implementations from the decompile.
// Globals are retail addresses (P) read through rd*/wr*; imported ordinals and unrecovered callees are Env function
// pointers (names positional). Wide strings are uint16_t buffers (Ghidra "Unicode" is a 1-byte stand-in, step 2).
#include <cstdint>
#include <cstddef>
#include <initializer_list>
namespace d2client_batch_bea10 {
using P = uintptr_t;
using W = uint16_t;
struct Env {
 uint32_t (*rd32)(P); void (*wr32)(P, uint32_t); uint32_t (*rd8)(P); void (*wr8)(P, uint32_t);
 // globals (retail addresses)
 P gBC48, gBC58, gB9C4, gB834, gBBFC, gAD54, gD3410, gD340C, gC288, gC4D4, gCC578, gCC778, gB9DC, gB9E0, gC991C;
 P gBEA0, gBE88, gC0A0, gC5590, gBC4C, g9264C, gCC28B, gB858, gB864, gB854, gCC3EC, gB92500, gD2496, gD24C0;
 P gC3AA4, gC3AB0, tblB834F0;           // tblB834F0 = first 10-byte record (base-4 of the Ghidra short*)
 P flagsA[6];                           // BEA10 must-be-zero flags: ADDC ADE4 ADE8 ADB0 ADF0 ADEC
 P flagsHalfL[8];                       // ADF4 ADD8 AD88 ADBC AE10 AE14 ADD0 (+pad) -> *out0 = w/2
 P flagsHalfR[2];                       // AD84 AD90 -> out2 = w/2
 // BEAF0 / BEB90 / BF370 / BF990
 P (*f2b420)(P); void (*f143e0)(const uint8_t*); P (*f4d630)(P, uint32_t); int32_t (*f4d670)(P, uint32_t);
 bool (*f4dc60)(P, uint32_t, uint32_t); void (*toUnicode)(W*, P, uint32_t);
 bool (*f98c0)(P, uint32_t); uint32_t (*f8e50)(P, uint32_t);
 // ordinals
 P (*o10331)(P); int32_t (*o10826)(); uint8_t (*o10190)(uint32_t, uint32_t, uint32_t); void (*o10065)(P); void (*o10043)(uint32_t, uint32_t);
 int32_t (*o10014)(int32_t); uint32_t (*o10459)(int32_t); void (*o10004)(char*, uint32_t); void (*o10138)(int32_t, uint32_t, uint32_t, uint32_t);
 uint32_t (*o10265)(uint32_t); void (*o10024)(uint32_t, uint32_t);
 void (*o10184)(); void (*o10028)(); void (*o10150)(const W*, uint32_t, uint32_t, uint32_t);
 const W* (*o10003)(); int32_t (*o10973)(P, uint32_t, uint32_t); int32_t (*o10227)();
 uint32_t (*o10106)(); int32_t (*o10202)(); P (*o10695)(); int32_t (*o10744)(); int32_t (*o11088)();
 void (*o10085)(const W*, uint32_t, uint32_t); void (*o426)(const char*, const char*, uint32_t, uint32_t);
 void (*f55d90)(); void (*f414f0)(const W*, W*);
 void (*f24d80)(); void (*faed470)(); void (*faf6a20)(); void (*fb09740)(uint32_t); void (*fb2beb0)(P);
 int32_t (*spr)(char*, const char*, int32_t);  // _sprintf one %d
 void (*fatal)(uint32_t, const char*);
};
static uint32_t wlen(const W* s) { uint32_t n = 0; while (s[n]) n++; return n; }
static void wcpy(W* d, const W* s) { while ((*d++ = *s++)) {} }
static void wcat(W* d, const W* s) { wcpy(d + wlen(d), s); }
static void wdigit(W* d, char c) { d[0] = (uint8_t)c; d[1] = 0; }
static void freeBlk(const Env& e, P p) { e.o10065(p); if (p) e.o10043(0x7a, 0); }

// 1.13c D2Client+0xBEA10
bool sub_BEA10(const Env& e, int32_t* o) {
 int32_t w = (int32_t)e.rd32(e.gBC48);
 o[0] = 0; o[2] = w; o[1] = 0; o[3] = w;
 for (P a : e.flagsA) if (e.rd32(a)) return false;
 for (P a : e.flagsHalfL) if (a && e.rd32(a)) { o[0] = w / 2; break; }
 for (P a : e.flagsHalfR) if (e.rd32(a)) { o[2] = w / 2; break; }
 return o[0] != o[2];
}
// 1.13c D2Client+0xBEAF0
void sub_BEAF0(const Env& e) {
 if (e.rd32(e.gB9C4) == 0) e.wr32(e.gB9C4, (uint32_t)e.f2b420(0));  // path "DATA\\GLOBAL\\ui\\panel\\buysellbtn" built, passed implicitly
}
// 1.13c D2Client+0xBEB90
void sub_BEB90(const Env& e) {
 P u = e.rd32(e.gB834);
 if (!u) return;
 uint32_t v = e.rd32(u + 0xc);
 uint8_t b[9] = {0x27, (uint8_t)v, (uint8_t)(v >> 8), (uint8_t)(v >> 16), (uint8_t)(v >> 24), (uint8_t)v, (uint8_t)(v >> 8), (uint8_t)(v >> 16), (uint8_t)(v >> 24)};
 e.f143e0(b);
}
// 1.13c D2Client+0xBF0D0
void sub_BF0D0(const Env& e) {
 P u = e.rd32(e.gBBFC);
 P nm = u ? e.o10331(u) : 0;
 if (!u || !nm) {
  P s = e.rd32(e.gAD54);
  if (s) { freeBlk(e, s); e.wr32(e.gAD54, 0); }
  return;
 }
 int32_t id = e.o10826();
 if (id != (int32_t)e.rd32(e.gD3410)) {
  e.wr8(e.gC288, e.o10190(0, 0x80, 0)); e.wr8(e.gC288 + 1, e.o10190(0xc0, 0xc0, 0)); e.wr8(e.gC288 + 2, e.o10190(0xff, 0, 0));
  P s = e.rd32(e.gAD54);
  if (s) { freeBlk(e, s); e.wr32(e.gAD54, 0); }
  e.wr32(e.gD340C, ((e.rd32(e.gD3410) != 0) - 1u) & 0x78);
  e.wr32(e.gD3410, (uint32_t)id);
 }
 if ((int32_t)e.rd32(e.gD340C) < 0x78) {
  e.wr32(e.gD340C, e.rd32(e.gD340C) + 1);
  int32_t k = e.o10014(id);
  char act[16], mid[16];
  if (id < 0x6d) { char t[8]; (void)t; e.spr(act, "ACT%d", (int32_t)(e.o10459(id) & 0xff) + 1); }
  e.o10004(mid, 0);
  (void)k; // path "%s\\UI\\%s\\%s\\%s" built; consumed implicitly by FUN_6fadb420 below
  int32_t n = (int32_t)e.rd32(e.gBEA0);
  P rec = e.gBE88;  // 13-byte records {id@-4 .. } indexed from gBE88-4
  for (int32_t i = 0; i < n; i++, rec += 0xd) {
   if (id != (int32_t)e.rd32(rec - 4)) continue;
   if (e.rd32(e.gC0A0) == 0) { e.o10024(0, e.o10265(0x60)); return; }
   if ((int32_t)e.rd32(rec) > 0x24) { e.o10024(0, e.o10265(0x65)); return; }
   if (e.rd8(e.gC5590 + e.rd32(rec)) != 0) break;  // ACT1 override path
  }
  if (!e.rd32(e.gAD54)) e.wr32(e.gAD54, (uint32_t)e.f2b420(0));
  e.o10138((int32_t)e.rd32(e.gBC4C) / 2 - 0x8c, 1, 5, 1);
 } else {
  P s = e.rd32(e.gAD54);
  if (s) { freeBlk(e, s); e.wr32(e.gAD54, 0); }
 }
}
// 1.13c D2Client+0xBF370
void sub_BF370(const Env& e, const W* str, uint32_t a2, uint32_t a3) {
 W buf[512] = {0};
 int32_t o[4];
 if (sub_BEA10(e, o) && str) {
  e.o10184(); wcpy(buf, str); e.o10028(); e.o10150(buf, a2, a3, 1); e.o10184();
 }
}
// 1.13c D2Client+0xBF5F0
void sub_BF5F0(const Env& e, char digit, W* s) {
 if (!s[0]) return;
 W buf[1024] = {0}, d[4];
 wcpy(buf, e.o10003());
 wdigit(d, (char)(digit + '0'));
 wcat(buf, d); wcat(buf, s); wcpy(s, buf);
}
// 1.13c D2Client+0xBF990
void sub_BF990(const Env& e, P a1) {
 if (!(e.rd32(e.g9264C) & 2) || !e.rd32(e.gBBFC)) return;
 P r = e.f4d630(a1, e.rd32(e.rd32(e.gBBFC) + 0xc));
 if (!r) return;
 int32_t v = (int32_t)e.rd32(r + 0x18);
 char c[12]; W w[12] = {0};
 e.spr(c, "%d", v);
 for (int i = 0; i < 11 && c[i]; i++) w[i] = (uint8_t)c[i];
 e.o10150(w, 0x19, 4, 0);
}
// 1.13c D2Client+0xBFA10  (s = 8 slot indexes + table ptr @+0x20, rows of 12 bytes)
void sub_BFA10(const Env& e, P ctx, P s) {
 struct S { int slot; uint32_t code, flag, def; };
 const S t[] = {{1, 0x16, 1, 0xc0}, {0, 0x26, 1, 0x1b}, {4, 0x29, 1, 0x4e}, {6, 0x2c, 1, 0x57},
                {3, 0x16, 0, 0xc0}, {2, 0x26, 0, 0x1b}, {5, 0x29, 0, 0x4e}, {7, 0x2c, 0, 0x57}};
 for (const S& x : t) {
  if ((x.slot == 6 || x.slot == 7) && !e.o10227()) continue;
  int32_t i = (int32_t)e.rd32(s + x.slot * 4);
  if (i == -1) continue;
  P cell = e.rd32(s + 0x20) + 4 + (P)i * 0xc;
  uint32_t r;
  if (e.f98c0(ctx, x.code) && (uint16_t)(r = e.f8e50(ctx, x.flag)) != 0xffff && (uint16_t)r < 0xe0) e.wr32(cell, r & 0xffff);
  else e.wr32(cell, x.def);
 }
}
// 1.13c D2Client+0xBFCA0
void sub_BFCA0(const Env& e, P u) {
 if (u && (int32_t)e.rd32(u + 4) >= 0x2c0 && (int32_t)e.rd32(u + 4) <= 0x2c5) return;
 const W* sep2 = e.o10003(); const W* sep1 = e.o10003();
 W acc[256] = {0};
 for (int i = 0; i < 6; i++) {
  P r = e.tblB834F0 + (P)i * 10;
  if (e.o10973(u, e.rd32(r), 0) > 99) {
   W t[256] = {0};
   if (acc[0]) { wcat(acc, sep2); wcat(acc, sep1); }
   wcpy(t, e.o10003());
   sub_BF5F0(e, (char)e.rd32(r + 6), t);
   wcat(acc, t);
  }
 }
 wcpy((W*)e.gCC578, acc);  // gCC578 holds a host pointer to the 256-wchar retail buffer
 e.wr32(e.gCC778, 4);
}
// 1.13c D2Client+0xBFEC0  (in = EAX string; out/outSize = caller stack args)
void sub_BFEC0(const Env& e, const W* in, W* out, int32_t outSize) {
 e.wr32(e.gBC58, e.rd32(e.gBC58) - 0x10);
 if (!in) return;
 e.f55d90();
 char c = 0;
 switch (e.o10106()) {
  case 4: c = 3; break; case 5: c = 2; break; case 6: c = 9; break;
  case 7: c = 4; break; case 8: c = 8; break; case 9: c = 10; break;
  default: if (e.o10202() || e.o10202()) c = 5;
 }
 P p = e.o10695();
 if (e.rd8(p + 0x12a) && e.rd32(p + 0x80) != 0x2067656c) c = 4;
 uint32_t k = e.rd32(p + 0x80);
 if (k == 0x20686563 || k == 0x20746562 || k == 0x20736574 || k == 0x20646566 || k == 0x20616f74 || k == 0x206e6864 ||
     k == 0x20796562 || k == 0x2072626d || k == 0x20316b70 || k == 0x20326b70 || k == 0x20336b70 || e.o10744()) c = 8;
 uint32_t bc58 = e.rd32(e.gBC58);
 W buf[256] = {0};
 uint32_t code;
 if (e.o11088() == 4) {
  int32_t v = e.o10973(0, 0, 0);
  char t[16]; e.spr(t, "%i", v);
  W d[16] = {0}; for (int i = 0; i < 9 && t[i]; i++) d[i] = (uint8_t)t[i];
  wcpy(buf, d); wcat(buf, e.o10003()); wcat(buf, in);
  code = 0x5e0;
 } else {
  e.f414f0(in, buf);
  code = 0x5eb;
 }
 if (!out) { e.o10085(buf, c, 1); (void)bc58; return; }
 sub_BF5F0(e, c, buf);
 if ((int32_t)wlen(buf) < outSize) { wcpy(out, buf); return; }
 e.fatal(code, 0);
}
// 1.13c D2Client+0xC0250
void sub_C0250(const Env& e) {
 e.f24d80();
 P slot = e.gB92500 + (P)(e.rd32(e.gC991C) & 0xff) * 0x22;
 uint32_t s = e.rd32(slot);
 if (s) { freeBlk(e, s); e.wr32(slot, 0); }
 e.faed470();
 for (int i = 0; i < 0x29; i++) { e.wr8(e.gD2496 + i, 0); e.wr8(e.gD24C0 + i, 0); }
 e.faf6a20(); e.fb09740(0); e.fb2beb0(e.gC3AA4); e.fb2beb0(e.gC3AB0);
 e.wr8(e.gCC28B, 0);
 for (P g : {e.gAD54, e.gB9DC, e.gB9E0}) { uint32_t p = e.rd32(g); if (p) { freeBlk(e, p); e.wr32(g, 0); } }
 e.o426("Diablo II", "Always Run", 0, e.rd32(e.gCC3EC));
 e.wr32(e.gB834, 0); e.wr32(e.gB858, 0); e.wr32(e.gB864, 1); e.wr32(e.gB854, 5);
}
// 1.13c D2Client+0xC0390  (out2 = EDI int*)
uint32_t sub_C0390(const Env& e, P self, W* name, P outScale, P out2) {
 int32_t id = self ? (int32_t)e.rd32(self + 0xc) : -1;
 P n = e.rd32(e.gC4D4);
 if (!n) return 0;
 while ((int32_t)e.rd32(n + 8) != id) { n = e.rd32(n + 0x30); if (!n) return 0; }
 uint32_t k = e.rd32(n + 0xc);
 if (k == 0xffffffffu) return 0;
 if (self && e.rd32(self + 0x10) != 0 && e.rd32(self + 0x10) != 0xc) {
  P r = e.f4d630(self, k);
  if (!r) return 0;
  e.toUnicode(name, r + 0x66, 0x80);
  e.wr32(outScale, (uint32_t)(((int32_t)e.f4d670(self, k) << 7) / 100));
  P me = e.rd32(e.gBBFC);
  e.wr32(out2, 0);
  if (e.f4dc60(outScale, me ? e.rd32(me + 0xc) : 0xffffffffu, k)) e.wr32(out2, 1);
 }
 return 1;
}
}
