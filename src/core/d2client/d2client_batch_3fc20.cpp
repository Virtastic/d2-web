// Prior art: none (D2MOO has no D2Client module); derived from the Ghidra decompile.
// Imported-DLL calls and unresolved D2Client callees are seams (Services), unverified.
#include <cstdint>
#include <cstring>
namespace d2client_batch_3fc20 {
template<class T> T rd(const void* p, unsigned o) { T v; std::memcpy(&v, static_cast<const uint8_t*>(p)+o, sizeof v); return v; }
template<class T> void wr(void* p, unsigned o, T v) { std::memcpy(static_cast<uint8_t*>(p)+o, &v, sizeof v); }

struct Services {
 virtual ~Services() = default;
 [[noreturn]] virtual void fatal(int32_t stringId) = 0;      // Ordinal_10265/10024/_exit(-1)
 virtual void release(void* handle) = 0;                       // FUN_6fb53d20 (D2Client+0xA3D20)
 virtual void freeCel(int32_t h) = 0;                          // Ordinal_10065 + Ordinal_10043
 virtual void freeUnit(int32_t kind, int32_t h) = 0;           // Ordinal_10901
 virtual void line(int32_t x0,int32_t y0,int32_t x1,int32_t y1,int32_t c0,int32_t c1) = 0; // Ordinal_10010
 virtual void marker(int32_t x, int32_t col) = 0;              // FUN_6faef590
 virtual void text(int32_t x,int32_t y,const void* s,int32_t v) = 0; // Ordinal_10050
 virtual void isoToScreen(int32_t& x, int32_t& y) = 0;         // Ordinal_11087
 virtual void worldToScreen(int32_t* xy2) = 0;                 // Ordinal_10375
 virtual uint32_t flags(int32_t id) = 0;                       // Ordinal_10051
 virtual int32_t unitProp(int32_t unit,int32_t a,int32_t b) = 0; // Ordinal_10174
 virtual void* playerRoom(int32_t unit) = 0;                   // Ordinal_10561 / 10310
 virtual void queued() = 0;
};

// 1.13c D2Client+0x446B0
struct Hud446B0 { int32_t v46c8, f3a0, f3e0, f3fc, f3e8, f3ec, f3f0, f3fcOut; int32_t* player; int32_t a, b; };
void sub_446B0(Hud446B0& s, void (*send)(int32_t,int32_t,int32_t,uint8_t)) {
 s.v46c8 = 8;
 if (s.f3a0 && s.f3e0) {
  uint8_t b = 0; s.f3fc = 1;
  if (s.f3e8) b = 8;
  if (s.f3ec) b = 8;
  if (s.f3f0) b |= 4;
  if (s.player && *s.player == 0) send(4, s.a, s.b, b);
 }
}

// 1.13c D2Client+0x465F0
struct NpcFlags { int32_t haveCurrent; uint8_t f731; uint16_t f735; };
void sub_465F0(NpcFlags& s, Services& sv, const uint8_t* pkt) {
 if (!pkt) sv.fatal(0xfc6);
 if (rd<int16_t>(pkt,1) == 3 && s.haveCurrent) { s.f731 = 1; s.f735 = (uint16_t)rd<int16_t>(pkt,1); }
}

// Shared by 46640 / 467C0: eight owned handles, released in the order given.
struct Handles { void* h[8]; }; // order: 9757,975b,975f,9763,9767,976b,976f,9773
static void relOrder(Handles& s, Services& sv, const int* ord) {
 for (int i = 0; i < 8; ++i) { void*& p = s.h[ord[i]]; if (p) { sv.release(p); p = nullptr; } }
}
// 1.13c D2Client+0x467C0
void sub_467C0(Handles& s, Services& sv) { static const int o[8] = {4,5,3,7,0,1,2,6}; relOrder(s, sv, o); }

// 1.13c D2Client+0x46640
struct Teardown { int32_t u73b, u73f, cel7c0, cel7c4; void** obj77b; Handles h; };
void sub_46640(Teardown& s, Services& sv) {
 sv.freeUnit(0, s.u73b); sv.freeUnit(0, s.u73f); s.u73b = s.u73f = 0;
 if (s.cel7c0) { sv.freeCel(s.cel7c0); s.cel7c0 = 0; }
 if (s.cel7c4) { sv.freeCel(s.cel7c4); s.cel7c4 = 0; }
 if (s.obj77b) { reinterpret_cast<void(*)(int)>(**reinterpret_cast<void***>(&s.obj77b))(1); s.obj77b = nullptr; }
 static const int o[8] = {0,1,2,3,6,7,4,5}; relOrder(s.h, sv, o);
}

// 1.13c D2Client+0x46A20
struct Reset46A20 { uint16_t v23e8; uint16_t v9662; int32_t count; uint8_t* rec; /* 0x16-byte records, flags at +0x0,+0x1 relative to 37b9 */ };
void sub_46A20(Reset46A20& s, Teardown& t, Services& sv, void (*sub45EF0)()) {
 s.v23e8 = 0xffff; sub_46640(t, sv); sub45EF0(); s.v9662 = 0;
 for (int i = 0; i < s.count; ++i) { s.rec[i*0x16] = 0; s.rec[i*0x16+1] = 0; }
}

// 1.13c D2Client+0x47CD0
struct Cb47 { int32_t f78e; char f781; int32_t f786; };
bool sub_47CD0(const Cb47& s, int32_t p) { return s.f78e && s.f781 == 1 && p == s.f786; } // true => call FUN_6faf7ab0
// 1.13c D2Client+0x47D00 (identical body, other caller)
bool sub_47D00(const Cb47& s, int32_t p) { return sub_47CD0(s, p); }

// 1.13c D2Client+0x46880 ; ebx/ebp/esi are caller-supplied registers
bool sub_46880(bool havePlayer, int32_t ebx, int32_t ebp, int32_t* esi) {
 if (!havePlayer) return false;
 int32_t y = esi[1] + (-0x96 - ebx); esi[1] = y; esi[0] -= ebp;
 if (y < 0x14) esi[1] = 0x14;
 return true;
}

// table = 15-byte records, edi = struct{+5 table,+9 count,+0xd pick}
struct Rng { uint32_t lo, hi; };
static uint32_t rnd(Rng& r, uint32_t n) {
 if ((int32_t)n < 1) return 0;
 uint64_t v = (uint64_t)r.lo * 0x6ac690c5u + r.hi; r.lo = (uint32_t)v; r.hi = (uint32_t)(v >> 32);
 return (n & (n-1)) == 0 ? (n-1) & (uint32_t)v : (uint32_t)v % n;
}
// 1.13c D2Client+0x46910
void sub_46910(uint8_t* tbl, bool isDefaultTbl, uint32_t count, uint32_t& pick, Rng& rng,
               int32_t curId, int32_t unit, int32_t unit2, Services& sv) {
 for (int left = 10; left; ) {
  --left; pick = rnd(rng, count);
  if (pick > 1) {
   const uint8_t* r = tbl + pick*15;
   int32_t k = rd<int32_t>(r, 0xb);
   if (k == curId || k == 7) {
    if (r[2] == 0) return;
    if (sv.unitProp(unit, rd<int32_t>(r,7), 0) == rd<int32_t>(r,3)) {
     if (!isDefaultTbl) return;
     if (rd<int16_t>(r,0) != 0xff) return;
     if (sv.unitProp(unit2, 0xc, 0xd) != 1) return;
     pick = 2; return;
    }
   }
  }
  if (!left) { pick = 2; return; }
 }
}

// 1.13c D2Client+0x3FC20 (automap party/NPC marker pass); layout of 0x30-byte slots from the decompile.
struct Slot { int32_t pad0, x, y, flags, unit, kind; };
void sub_3FC20(Services& sv, int32_t ox, int32_t oy, const uint8_t* slots, uint32_t n,
               int32_t scrX, int32_t scrY) {
 for (uint32_t i = 0; i < n; ++i) {
  const uint8_t* p = slots + i*0x30;
  int32_t unit = rd<int32_t>(p,0x10), fl = rd<int32_t>(p,0x14), kind = rd<int32_t>(p,0x20);
  if (!unit) continue;
  int32_t px = (rd<int32_t>(p,0x0c) + ox)*5 + 3, py = (rd<int32_t>(p,0x10) + oy)*5 + 3;
  sv.isoToScreen(px, py);
  if (kind != 0xf && (fl & 0x1c000) == 0x4000) {
   uint32_t f = sv.flags(rd<int32_t>(p,0x18)); int32_t col = 0xff, mx = px;
   if (f & 4) col = 0x78; else if (fl & 0x800) { col = 0x78; mx = px + 8; }
   sv.marker(mx, col); sv.text(scrY, px - scrX, nullptr, rd<int32_t>(reinterpret_cast<const uint8_t*>((intptr_t)unit), 0x28));
  }
 }
}

// 1.13c D2Client+0x46230 ; NPC-menu entry builder. Branch table preserved, imported calls are seams.
struct NpcMenu {
 virtual ~NpcMenu() = default;
 virtual int32_t kind() = 0;                    // FUN_6faf6150()->[1], -1 if none
 virtual bool has() = 0;
 virtual int32_t type(int32_t) = 0;             // Ordinal_11088
 virtual bool begin(int32_t) = 0;               // FUN_6faf5bd0 == 1
 virtual bool allowed(int32_t) = 0;             // FUN_6faf5b40
 virtual bool check(int32_t) = 0;               // Ordinal_10705
 virtual int32_t send(int32_t mode, int32_t kind) = 0; // Ordinal_10107
 virtual int32_t prop() = 0;                    // Ordinal_10174(…,4,0)
};
// returns 0/1; *out receives the cost; mode 0 = no retailer buffer
int32_t sub_46230(NpcMenu& m, int32_t unit, bool withBuf, int32_t uiState, int32_t* out, bool* handledZero) {
 *out = 0; *handledZero = false;
 if (!unit || m.type(unit) == 4) return 0;
 if (!withBuf) return (*out = m.send(0, m.has() ? m.kind() : -1)), 1;
 if (!m.has()) return 0;
 uint32_t k = m.has() ? (uint32_t)m.kind() : 0xffffffffu;
 enum { BUY, NPC, TALK, NONE } r = NPC;
 if (k == 0xfc || k==0x93||k==0x94||k==0xb1||k==199||k==0xca||k==0xf6||k==0x195||k==0x200||k==0x201||k==0x202) r = NPC;
 else if (k==0x9a||k==0xb2||k==0xfd||k==0x101||k==0x1ff) r = TALK;
 else if (k==0xf4||k==0xf5||k==0x109) r = BUY;
 else if (k==0xfe||k==0xff) r = NPC;
 else return 0;
 if (r == BUY) {
  if (m.begin(unit)) { *handledZero = true; return 1; }
  *out = m.prop() ? 100 - 100 : 100; return 1; // unverified: retail yields 0x64 or 0x00 from ((prop!=0)&-100)+100
 }
 if (r == TALK && uiState == 4) { if (!m.allowed(unit)) return 0; *out = m.send(3, k); return 1; }
 if (!m.check(unit)) return 0;
 *out = m.send(1, k); return 1;
}

// 1.13c D2Client+0x43A70 (end-game-ok screen build/teardown). 6 or 12 tile layout; positions recovered, widget calls seams.
struct EndGame {
 virtual ~EndGame() = default;
 virtual bool alreadyBuilt() = 0;               // Ordinal_10043 != 0
 virtual bool menuReady() = 0;                  // Ordinal_10085
 virtual void loadAssets() = 0;
 virtual void tile(int32_t idx,int32_t x,int32_t y) = 0; // Ordinal_10041
 virtual void finish() = 0;
 virtual bool closing() = 0;                    // DAT_6fbcc38c
 virtual void freeAssets() = 0;
};
bool sub_43A70(EndGame& g, bool hiRes, int32_t w, int32_t h) {
 if (!g.alreadyBuilt()) {
  if (!g.menuReady()) return false;
  g.loadAssets();
  if (!hiRes) {
   g.tile(2,0x100,0); g.tile(3,0,h-1); g.tile(4,0x100,h-1);
   g.tile(5,w/2,0x100); g.tile(6,w/2+0x100,w/2); g.tile(7,h,w/2+0x100);
  } else {
   for (int i = 0; i < 12; ++i) g.tile(i+1, (i%4)*0x100, (i/4)*0x100);
  }
  g.finish();
 }
 if (!g.closing()) return false;
 g.freeAssets();
 return true;
}
} // namespace d2client_batch_3fc20
