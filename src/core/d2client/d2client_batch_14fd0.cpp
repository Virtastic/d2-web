// D2Client batch 14FD0: env-seam implementations (globals/ordinals injected). Not yet vectored.
#include <cstdint>
#include <emscripten.h>
typedef int32_t i32; typedef uint32_t u32;
extern "C" {

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

// 1.13c D2Client+0x45DD0  (list = 12 u16 after first; recs 0x16 bytes, id at +0, flag at +0x12)
EMSCRIPTEN_KEEPALIVE void d2c_45dd0(uint8_t* recs, i32 n, i32 limit, const uint16_t* list) {
  for (i32 k = 0; k < 12; k++) { u32 id = *++list;
    if ((i32)id < limit && n > 0)
      for (i32 i = 0; i < n; i++) if (*(u32*)(recs + i * 0x16) == id) recs[i * 0x16 + 0x12] = 1; } }

// 1.13c D2Client+0x45EF0
struct E45EF0 { i32 g97b8, g97b4; i32* blk58; i32 g973b, g973f; uint8_t g9663;
  i32 (*o10939)(i32); void (*getTick)(); i32 (*f2b420)(i32);
  void (*fmt)(char* out, const char* f); i32 cel97c0, cel97c4; i32 g2305; uint8_t g22f8; i32 g2309; i32 g2301; };
EMSCRIPTEN_KEEPALIVE void d2c_45ef0(E45EF0* e) {
  e->g97b8 = 0; e->g97b4 = 0; e->getTick();
  for (i32 i = 0; i < 0x58; i++) e->blk58[i] = 0;
  e->g973b = e->o10939(0); e->g973f = e->o10939(0); e->g9663 = 0;
  char path[0x104];   // sprintf's buffer: retail hands its address to the cel loader in EAX
  e->fmt(path, "%s\\ui\\panel\\buysell"); e->cel97c0 = e->f2b420((i32)(uintptr_t)path);
  e->fmt(path, "%s\\ui\\menu\\goldbtn"); e->cel97c4 = e->f2b420((i32)(uintptr_t)path);
  e->g2305 = 0; e->g22f8 = 0; e->g2309 = 0; e->g2301 = 0; }

// 1.13c D2Client+0x45FB0
struct E45FB0 { uint8_t g978e; i32 g971d; void (*f43e0)(const char*); };
EMSCRIPTEN_KEEPALIVE void d2c_45fb0(E45FB0* e) {
  uint8_t b[10] = {'8'}; e->g978e = 0; *(i32*)(b + 5) = e->g971d; e->f43e0((char*)b); }

// 1.13c D2Client+0x46070
struct E46070 { uint32_t g9721; i32 g971d, g9725; }; // 0x119721 is stored as a dword (mov dword [0x6FBC9721], 0|1)
EMSCRIPTEN_KEEPALIVE void d2c_46070(E46070* e, const i32* p) {
  if (!p) { e->g9721 = 0; return; } e->g9721 = 1; e->g971d = p[3]; e->g9725 = p[1]; }

}
