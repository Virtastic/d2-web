// D2Client batch 14FD0: env-seam implementations (globals/ordinals injected). Not yet vectored.
#include <cstdint>
#include <emscripten.h>
typedef int32_t i32; typedef uint32_t u32;
extern "C" {

// 1.13c D2Client+0x14fd0
struct E14FD0 { i32 tblCount; uint8_t* tblBase;  // sgptDataTables+0xba0 / +0xb98 (0x23c stride)
  i32 (*o10511)(); i32 (*o10179)(); i32 (*o10649)();
  void (*f54e20)(i32, i32 slotPtr, i32); i32 slotBase;  // DAT 0xBBA608, stride 0x80
  i32 (*o11133)(i32*); i32 (*o11138)(i32*, i32, i32);
  i32 (*o10980)(i32); void (*o10243)(i32, i32);
  void (*f51860)(i32*, i32, i32, i32, i32, i32); };
EMSCRIPTEN_KEEPALIVE u32 d2c_14fd0(E14FD0* e, i32* u, i32 idx, i32 ebp) {
  if (!(idx > -1 && idx < e->tblCount && e->tblBase + idx * 0x23c)) return 0;
  i32 a = e->o10511(); if (!a) return 0;
  i32 b = e->o10179(), c = e->o10649();
  e->f54e20(b, e->slotBase + c * 0x80, c);
  i32 d = e->o11133(u);
  if (ebp == 0 || e->o11138(u, ebp, 0) == 0) { e->f51860(u, 0, 0, d + 4, 0x2a283, 0x6fb509f0); }
  else { i32 n = e->o10980(a); e->o10243(a, n - 1); if (n - 1 < 1) return 1;
         e->f51860(u, 0, 0, d + 4, 0x2a283, 0x6fb509f0); }
  return 0; }

// 1.13c D2Client+0x15DA0
struct E15DA0 { i32 tblCount; uint8_t* tblBase; i32 cofCount;  // +0xba0/+0xb98/+0xb6c
  i32 (*o10076)(); i32 (*o10786)(i32*, i32, i32, i32);
  i32 (*f51340)(i32* out); i32 (*f1630)(i32*); i32 (*f1660)(i32*);
  void (*f50800)(); void (*f507c0)();
  void (*f630f0)(i32* desc); i32 ebp; i32 stepX; i32 stepY; };
EMSCRIPTEN_KEEPALIVE u32 d2c_15da0(E15DA0* e, i32* u, i32 idx, i32 p3) {
  if (u) u[0x31] |= 0x40;
  if (!(idx > -1 && idx < e->tblCount)) return 0;
  uint8_t* r = e->tblBase + idx * 0x23c; if (!r) return 0;
  i32 v = *(int16_t*)(r + 0xea);
  if (e->o10076() != 1 && *(int16_t*)(r + 0xec) > -1) v = *(int16_t*)(r + 0xec);
  if (!(v > -1 && v < e->cofCount)) return 0;
  i32 sc = e->o10786(u, *(i32*)(r + 0x138), idx, p3);
  i32 top; if (!e->f51340(&top)) return 0;
  i32 w = e->f1630(u); i32 h = top - e->f1660(u);
  e->f50800(); e->f507c0();
  i32 x = e->ebp - ((e->ebp - w) * sc) / 2, y = top - (h * sc) / 2;
  i32 d[0x17] = {0}; d[0] = 0x820; d[1] = (i32)(intptr_t)u; d[2] = (i32)(intptr_t)u;
  // asm 0x15EDD-0x15F40: desc base = esp+0x30; d[4]=v, d[11]=idx, d[12]=p3, d[13]=o10786(u,r+0x13c,idx,p3); x,y at d[7],d[8]
  d[4] = v; d[11] = idx; d[12] = p3;
  d[13] = e->o10786(u, *(i32*)(r + 0x13c), idx, p3);
  for (; h > 0; h--) { d[7] = x; d[8] = y; e->f630f0(d); x += e->stepX; y += e->stepY; }
  return 1; }

// 1.13c D2Client+0x45C50
EMSCRIPTEN_KEEPALIVE u32 d2c_45c50(i32 v) {
  switch (v) { case 0x92: case 0x20f: case 0x219: case 0x21a: case 0x21b: case 0x209: case 0x10a:
  case 0x17a: case 0xfb: case 0x179: case 0x14b: case 0x196: case 0x198: return (u32)v & ~0xffu | 1; }
  return (u32)v >> 8 << 8; }

// 1.13c D2Client+0x45CC0  (tbl = this, 20-byte entries, count = EAX)
EMSCRIPTEN_KEEPALIVE u32 d2c_45cc0(i32* tbl, i32 count, i32 key, i32 p2, i32 (*o10174)(i32,i32,i32)) {
  u32 r = 1;
  for (i32* p = tbl + 2; count > 0; count--, p += 5) {
    if (p[-2] != key) continue;
    i32 t = o10174(p2, p[-1], p[0]);
    if (p[1] == 0 ? t == 0 : t != 0) r = ((u32 (*)())(intptr_t)p[2])();
  }
  return r; }

// 1.13c D2Client+0x45DD0  (list = 12 u16 after first; recs 0x16 bytes, id at +0, flag at +0x12)
EMSCRIPTEN_KEEPALIVE void d2c_45dd0(uint8_t* recs, i32 n, i32 limit, const uint16_t* list) {
  for (i32 k = 0; k < 12; k++) { u32 id = *++list;
    if ((i32)id < limit && n > 0)
      for (i32 i = 0; i < n; i++) if (*(u32*)(recs + i * 0x16) == id) recs[i * 0x16 + 0x12] = 1; } }

// 1.13c D2Client+0x45E50
struct E45E50 { int16_t* tab; int16_t* tabEnd; i32 flag284; i32 flag29c;
  void (*f72790)(i32, i32, i32); i32 (*ord423)(const char*, const char*, i32, i32*); };
EMSCRIPTEN_KEEPALIVE void d2c_45e50(E45E50* e, i32 v) {
  int16_t* p = e->tab; do { if (*p == (int16_t)v) break; p += 8; } while (p < e->tabEnd);
  e->flag284 = 0; e->f72790(0x23, 0, 0);
  i32 out = 0; i32 r = e->ord423("Diablo II", "PopupHireling", 0, &out);
  (void)r; if (out == 0) e->flag29c = 1; }  // ponytail: decompile tests the string ptr, not the out value; vector decides

// 1.13c D2Client+0x45EF0
struct E45EF0 { i32 g97b8, g97b4; i32* blk58; i32 g973b, g973f; uint8_t g9663;
  i32 (*o10939)(i32); void (*getTick)(); i32 (*f2b420)(i32);
  void (*fmt)(const char*); i32 cel97c0, cel97c4; i32 g2305; uint8_t g22f8; i32 g2309; i32 g2301; };
EMSCRIPTEN_KEEPALIVE void d2c_45ef0(E45EF0* e) {
  e->g97b8 = 0; e->g97b4 = 0; e->getTick();
  for (i32 i = 0; i < 0x58; i++) e->blk58[i] = 0;
  e->g973b = e->o10939(0); e->g973f = e->o10939(0); e->g9663 = 0;
  e->fmt("%s\\ui\\panel\\buysell"); e->cel97c0 = e->f2b420(0);
  e->fmt("%s\\ui\\menu\\goldbtn"); e->cel97c4 = e->f2b420(0);
  e->g2305 = 0; e->g22f8 = 0; e->g2309 = 0; e->g2301 = 0; }

// 1.13c D2Client+0x45FB0
struct E45FB0 { uint8_t g978e; i32 g971d; void (*f43e0)(const char*); };
EMSCRIPTEN_KEEPALIVE void d2c_45fb0(E45FB0* e) {
  uint8_t b[10] = {'8'}; e->g978e = 0; *(i32*)(b + 5) = e->g971d; e->f43e0((char*)b); }

// 1.13c D2Client+0x45FF0  (bx = wanted id)
struct E45FF0 { int16_t (*tab)[4]; i32 tabN; i32 unit; i32 (*o10778)(i32, i32);
  int16_t* skip; uint16_t skipN; };
EMSCRIPTEN_KEEPALIVE i32 d2c_45ff0(E45FF0* e, int16_t bx) {
  for (i32 i = 0; i < e->tabN; i++) if (e->tab[i][0] == bx) return e->tab[i][1];
  i32 s = (int16_t)e->o10778(e->unit, 0);
  for (i32 i = 0;; s = (int16_t)e->o10778(e->unit, ++i)) {
    if (s == -1) return -1;
    if (s == bx) continue;
    i32 k = 0; while (k < e->skipN && e->skip[k] != s) k++;
    if (k >= e->skipN) return s; } }

// 1.13c D2Client+0x46070
struct E46070 { uint8_t g9721; i32 g971d, g9725; };
EMSCRIPTEN_KEEPALIVE void d2c_46070(E46070* e, const i32* p) {
  if (!p) { e->g9721 = 0; return; } e->g9721 = 1; e->g971d = p[3]; e->g9725 = p[1]; }

// 1.13c D2Client+0x460A0
struct E460A0 { uint8_t* clr[10]; // DAT 0xBC9685 + n*0x10, zeroed 4 bytes... first is 1 byte
  i32 (*o10460)(i32); i32 (*o10464)(i32); i32* (*o11147)(i32);
  void (*o11024)(i32); void (*o10251)(i32); void (*f56ef0)(i32* rec, i32 a, i32 b, i32 c); i32 slotBase; };
EMSCRIPTEN_KEEPALIVE void d2c_460a0(E460A0* e, uint8_t* in) {
  *e->clr[0] = 0; for (i32 i = 1; i < 10; i++) *(i32*)e->clr[i] = 0;
  if (!in) return; i32 h = *(i32*)(in + 0x60); if (!h) return;
  for (i32 it = e->o10460(h); it;) { i32 nx = e->o10464(it); i32* r = e->o11147(it); it = nx;
    if (r) e->f56ef0((i32*)(intptr_t)(e->slotBase + r[0] * 0x80), r[3] & 0x7f, r[3], r[0]); }
  e->o11024(h); e->o10251(h); }

// dup of D2CLIENT+0x46A60
struct E46A60 { i32** bucket; /* DAT 0xBBA808[128] */ i32 g97ac, g97b0; i32** list; // DAT 0xBCBF30
  void (*f460a0)(); void (*fatal)(); void (*f38550)(i32*); };
EMSCRIPTEN_KEEPALIVE void d2c_46a60(E46A60* e, u32 key) {
  i32* p = (i32*)e->bucket[key & 0x7f];
  for (; p; p = (i32*)p[0x39]) if ((u32)p[3] == key) {
    if (*p != 1) { e->fatal(); return; } e->f460a0(); break; }
  if (e->g97ac && e->g97b0)
    for (i32* n = (i32*)*e->list; n; n = (i32*)n[1])
      if (*(i32*)(*n + 8) == e->g97b0) { e->f38550((i32*)(intptr_t)*n); e->g97ac = 0; e->g97b0 = 0; return; }
  e->g97ac = 0; e->g97b0 = 0; }
}
