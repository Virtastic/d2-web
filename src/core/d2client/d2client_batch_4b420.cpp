// Batch D2Client 0x4B420..0x4C500. Decompile-derived; unresolved callees are seams (unverified).
// Register-passed args (ESI/EAX/ECX) are made explicit parameters.
#include "d2client_b4b420.h"
namespace d2client_batch_4b420 {
// (seam copy; verified body: dialog_47810_4b420.cpp)
void sub_4B420(Seams& s, State& g) {
 if (g.g975f) { s.s53d20(); g.g975f = 0; }
 if (g.g9781 == 5) s.s0ac00();
 else if (g.g9781 != 6 && g.g9781 != 3) {
  g.g9663 = 0xc;
  if (g.g975b) s.fatal(0x1413);
  g.g975b = s.createDlg(); s.s53340(1);
  s.dlgText(g.g975b,0x15,0,0); s.dlgText(g.g975b,0xf,0,1);
  return;
 }
 if (Unit* u = s.s6150()) {
  uint32_t c = (uint32_t)u->classId; // piVar4[1]
  bool hit = c < 0x10a ? (c == 0x109 || (c >= 0xf4 && c <= 0xf6)) : c == 0x208;
  if (hit) for (int32_t& t : g.tab3e) t = 2;
 }
 s.s8b20();
}
// 1.13c D2Client+0x4B5D0
void sub_4B5D0(Seams& s, State& g, int32_t player) {
 int32_t pid = player; // *(DAT_6fbcbbfc+0x60)
 if (g.g9781 > 6) {
  if (g.g975f) { s.s53d20(); g.g975f = 0; }
  int32_t msg;
  if (g.g9663 != 5) msg = 0x13ad;
  else {
   g.g9663 = 6; g.gbc34 = 7;
   if (!g.g975b) {
    if (g.g9729 == 2 && s.ord11017(pid) != 0 && g.g9737 == 1 && g.gb834 == 0) s.s62d0();
    g.g975b = s.createDlg();
    if (g.g975b) { s.dlgText(g.g975b,0x15,0,0); s.dlgText(g.g975b,0xf,0,1); return; }
    msg = 0x529; g.g975b = nullptr;
   } else msg = 0x1399;
  }
  s.fatal(msg);
 }
 if (g.g9781 == 0 || g.g9781 == 1) {
  s.s38a70();
  Unit* u = s.findUnit(4, g.g9786);
  if (g.g9781 == 0) g.g9753 = 0;
  if (!u) { if (g.g9781 == 0) g.g9753 = 0; g.g978e = 1; return; }
  if (g.g9781 == 1) { if (u->type == 4) { g.g978e = 1; return; } s.fatal(0x5a); }
  if (u->type != 4) s.fatal(0x5a);
 } else if (g.g9781 == 5) s.s0ac00();
 g.g978e = 0;
 if (g.g975f) { s.s53d20(); g.g975f = 0; }
 g.c2f8 = 0; g.c2f4 = 0; g.d22f8 = 0; g.d22f9 = 0; g.d22fd = 0; g.g9663 = 3;
 s.s3f660(); s.s44970();
}
// 1.13c D2Client+0x4BC80   (esi = 15-byte packet)
void sub_4BC80(Seams& s, State& g, const uint8_t* p, int32_t player) {
 auto rd=[&](int o){uint32_t v=0; for(int i=3;i>=0;--i) v=v<<8|p[o+i]; return v;};
 g.g977f = rd(0); g.g9783 = rd(4); g.g9787 = rd(8);
 g.g978b = (uint16_t)(p[12] | p[13] << 8); g.g978d = p[14];
 g.g966b = s.ord10973(player, 0xe, 0);
 if (g.g9663 == 5) { sub_4B5D0(s, g, player); return; }
 if (g.g9663 == 10) { sub_4B420(s, g); return; }
 if (p[2] == 1) s.s38a70();
}
// 1.13c D2Client+0x4BEC0   (esi = unit)
void sub_4BEC0(Seams& s, Unit* u) {
 if (!u || u->type != 1) return;
 int32_t a = s.ord10494();
 if (!a || s.ord10494() != 0) s.s1260();
 uint32_t buf[0x17] = {};
 s.s630f0(buf); s.s630f0(buf);
}
// 1.13c D2Client+0x4BFA0   (ecx = pointer to a record whose dword 1 selects the lookup, eax = {idx, nonzero})
uint32_t sub_4BFA0(Seams& s, const uint32_t* en, const uint32_t* e, const uint32_t* bitMasks) {
 if (!en || !e || !e[1] || (int32_t)e[0] >= 0x10) return 0;
 uint8_t* r = (uint8_t*)s.s1260i((int32_t)en[1]);
 return r ? (bitMasks[e[0] & 7] & r[0x104 + ((int32_t)e[0] >> 3)]) : 0;
}
// 1.13c D2Client+0x4C3C0   (param_2 = packet: u8 type, u32 id @1, u16 @5)
void sub_4C3C0(Seams& s, const uint8_t* p) {
 uint32_t id = p[1] | p[2] << 8 | p[3] << 16 | (uint32_t)p[4] << 24;
 Unit* u = s.findUnit(1, id);
 if (!u) return;
 if (u->type != 1) s.fatal(0x5a);
 uint16_t v = (uint16_t)(p[5] | p[6] << 8);
 if (u->data) *(uint32_t*)((uint8_t*)u->data + 0x40) = v == 0xffff ? 0xffffffffu : v;
}
// 1.13c D2Client+0x4C430   (esi = unit)
bool sub_4C430(Seams& s, Unit* u) {
 if (!u || u->type != 1) return false;
 if (s.ord11085() != 0 && (u->type != 1 || !u->data || !(((uint8_t*)u->data)[0x16] & 8)))
  return !(s.sc0b0(u) != 0);
 return false;
}
// LCG seed step: (lo,hi) = lo*0x6AC690C5 + hi as a 64-bit value; the draw is its low word.
static uint32_t step(uint32_t* seed) { uint64_t v = (uint64_t)seed[0] * 0x6ac690c5ull + seed[1]; seed[0] = (uint32_t)v; seed[1] = (uint32_t)(v >> 32); return (uint32_t)v; }
// Callee seams of the spawn-trail step; each is one call of the original, in the original's order.
struct Spawn {
 void* ctx;
 uint32_t (*count)(void*);                 // spawn count for the unit
 uint32_t (*rate)(void*, uint32_t* ret);   // spawn rate of the unit's class row; 0 when the lookup fails or the rate is 0, with the value the original returns there in *ret
 int32_t (*playerLike)(void*);
 uint32_t (*ord10663)(void*);
 uint32_t (*room)(void*);
 int32_t (*pathX)(void*, int32_t direct);  // direct: read the path record, else the path accessor
 int32_t (*pathY)(void*, int32_t direct);
 int32_t (*blocked)(void*, uint32_t room, int32_t x, int32_t y);
 void (*place)(void*, uint32_t);
};
// Unit spawn-trail / scatter step   (flag, ecx = unit: type, mode, seed, hasPath)
// 1.13c D2Client+0x4C500
uint32_t spawnTrail(const Spawn& s, uint32_t* seed, int32_t type, int32_t mode, int32_t hasPath, int32_t flag, const int32_t dx[8], const int32_t dy[8]) {
 if (flag) { uint32_t r = step(seed); if (r % 25) return r / 25; }
 if (type == 1 && mode == 3) { uint32_t r = step(seed); if (r % 3) return r / 3; }
 uint32_t n = s.count(s.ctx), ret = n, fail = 0;
 uint32_t rate = s.rate(s.ctx, &fail);
 if (!rate) return fail;
 if ((int32_t)n <= 0) return ret;
 const int32_t direct = type == 2 || (type > 3 && type < 6);
 for (int32_t i = 0; i < (int32_t)n; ++i) {
  uint32_t d;
  if (!s.playerLike(s.ctx)) d = step(seed);
  else { uint32_t a = s.ord10663(s.ctx); d = (step(seed) % 7) - 3 + ((a & 0xff) >> 3); }
  d &= 7; uint32_t rm = s.room(s.ctx), stop = (d - 1) & 7;
  if (d == stop) return stop;
  for (;;) {
   int32_t x = direct ? s.pathX(s.ctx, 1) : hasPath ? s.pathX(s.ctx, 0) : 0;
   int32_t y = direct ? s.pathY(s.ctx, 1) : hasPath ? s.pathY(s.ctx, 0) : 0;
   if (!s.blocked(s.ctx, rm, dx[d] + x, dy[d] + y)) break;
   d = (d + 1) & 7; if (d == stop) return stop;
  }
  s.place(s.ctx, step(seed) % (rate * 2) + 0x12);
  ret = (uint32_t)i + 1;
 }
 return ret;
}
}
