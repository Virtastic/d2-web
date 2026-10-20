// Batch D2Client 0x4B420..0x4C500. Decompile-derived; unresolved callees are seams (unverified).
// Register-passed args (ESI/EAX/ECX) are made explicit parameters.
#include <cstdint>
namespace d2client_batch_4b420 {
struct Unit { int32_t type, classId, f08, id, f10; void* data; int32_t f18[2]; uint32_t seedLo, seedHi; void* path; Unit* next; };
struct Seams { // all unverified: names from ordinals / sub_ addresses
 virtual ~Seams()=default;
 virtual void s53d20()=0;                 // 0xD3D20-area ui teardown (FUN_6fb53d20)
 virtual void s0ac00()=0;                 // FUN_6fb0ac00
 virtual void fatal(int32_t msg)=0;       // Ordinal_10265 + 10024 + _exit(-1)
 virtual void* createDlg()=0;             // FUN_6fb53e40(...,0x6fafaaf0,...)
 virtual void s53340(int32_t)=0;
 virtual void dlgText(void* dlg,int32_t kind,int32_t cb,int32_t last)=0; // Ordinal_10003 + FUN_6fb536f0
 virtual Unit* s6150()=0;                 // sub_46150
 virtual void s8b20()=0;                  // sub_48B20
 virtual void s38a70()=0;                 // sub_88A70(0,0,0,0)
 virtual Unit* findUnit(int32_t type,uint32_t id)=0; // bucket walk, 128 buckets
 virtual void s3f660()=0; virtual void s44970()=0;
 virtual int32_t ord11017(int32_t)=0; virtual void s62d0()=0;
 virtual int32_t ord10973(int32_t,int32_t,int32_t)=0;
 virtual int32_t ord10494()=0; virtual void* s1260()=0; virtual void s630f0(uint32_t* buf)=0;
 virtual int32_t ord11085()=0; virtual uint8_t sc0b0(Unit*)=0;
};
struct State { // globals, RVAs of the D2Client image
 uint8_t g975f, g9781, g9663, g978e, g9753, g9729, g9737, g9786_dummy; uint32_t g9786; void* g975b; int32_t gbc34, gb834, gb828, gb824;
 int32_t d22f8, d22f9, d22fd, c2f8, c2f4;
 uint32_t g977f, g9783, g9787; uint16_t g978b; uint8_t g978d; int32_t g966b; int32_t clientPlayer; int32_t tab3e[5];
};
// 1.13c D2Client+0x4B420
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
// 1.13c D2Client+0x4BFA0   (ecx = enable, eax = {idx, nonzero})
uint32_t sub_4BFA0(Seams& s, int32_t enable, const uint32_t* e, const uint32_t* bitMasks) {
 if (!enable || !e || !e[1] || (int32_t)e[0] >= 0x10) return 0;
 uint8_t* r = (uint8_t*)s.s1260();
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
// 1.13c D2Client+0x4C500: LCG seed step is hi' = lo*0x6AC690C5 + hi (64-bit); only the
// seed advance and the entry gates are reproduced; the placement loop is seam-driven (unverified).
static uint32_t step(Unit* u) { uint64_t v = (uint64_t)u->seedLo * 0x6ac690c5ull + u->seedHi; u->seedLo = (uint32_t)v; u->seedHi = (uint32_t)(v >> 32); return (uint32_t)v; }
struct Spawn { virtual ~Spawn()=default; virtual uint32_t count(Unit*)=0; virtual bool eligible(Unit*)=0; virtual bool isPlayerLike(Unit*)=0;
 virtual uint32_t ord10663(Unit*)=0; virtual int32_t room(Unit*)=0; virtual int32_t ux(Unit*)=0; virtual int32_t uy(Unit*)=0;
 virtual bool blocked(int32_t room,int32_t x,int32_t y)=0; virtual void place(uint32_t)=0; virtual uint8_t spawnRate(Unit*)=0; };
uint32_t sub_4C500(Spawn& s, Unit* u, int32_t flag, const int32_t dx[8], const int32_t dy[8]) {
 if (flag) { uint32_t r = step(u); if (r % 0x19) return r / 0x19; }
 if (u && u->type == 1 && u->classId == 3) { uint32_t r = step(u); if (r % 3) return r / 3; }
 uint32_t n = s.count(u), ret = n;
 if (!s.eligible(u) || (int32_t)n <= 0) return ret;
 uint8_t rate = s.spawnRate(u);
 for (int32_t i = 0; i < (int32_t)n; ++i) {
  uint32_t d;
  if (!s.isPlayerLike(u)) d = step(u);
  else { uint32_t a = s.ord10663(u); d = (step(u) % 7) - 3 + ((a & 0xff) >> 3); }
  d &= 7; uint32_t stop = (d - 1) & 7; int32_t rm = s.room(u);
  if (d == stop) return stop;
  while (s.blocked(rm, dx[d] + s.ux(u), dy[d] + s.uy(u))) { d = (d + 1) & 7; if (d == stop) return stop; }
  if (rate) step(u);
  s.place((uint32_t)rate * 2);
  ret = (uint32_t)i + 1;
 }
 return ret;
}
}
