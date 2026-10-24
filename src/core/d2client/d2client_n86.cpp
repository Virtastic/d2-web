// Prior art: none (D2MOO has no D2Client module); derived from the Ghidra decompile.
// Every imported-DLL call, ordinal and unresolved D2Client callee is a seam (Env::call), unverified.
// Seam ids: ordinal n -> n; D2Client callee RVA r -> R(r); Win32/CRT -> W_*.
// Values that the decompile shows as lost register inputs (unaff_*, in_EAX) are explicit parameters.
#include <cstdint>
#include <cstring>
#include <initializer_list>
namespace d2client_n86 {
using W = int64_t;
constexpr uint32_t R(uint32_t rva) { return 0x1000000u + rva; }
enum : uint32_t { W_Lock = 0x2000001, W_Unlock, W_Sleep, W_DeleteCS, W_Debug, W_Exit, W_Wsprintf, W_Win2Uni,
                  U_Strcat, U_Strcpy, U_Strlen, U_Init };
struct Env { virtual ~Env() = default; virtual W call(uint32_t id, std::initializer_list<W> a) = 0; };
template<class T> T rd(const void* p, unsigned o) { T v; std::memcpy(&v, static_cast<const uint8_t*>(p)+o, sizeof v); return v; }
template<class T> void wr(void* p, unsigned o, T v) { std::memcpy(static_cast<uint8_t*>(p)+o, &v, sizeof v); }
template<class T> T* rdp(const void* p, unsigned o) { return rd<T*>(p, o); }
[[noreturn]] static void fatal(Env& e, W id) { W s = e.call(10265, {id}); e.call(10024, {(W)0, s}); e.call(W_Exit, {-1}); __builtin_unreachable(); }

// 1.13c D2CLIENT+0x431B0
struct ServerLoopG { int32_t stop, run, thrFlag, sel; uint8_t* sess; };
int32_t sub_431B0(Env& e, ServerLoopG& g) {
 int32_t last = 0, busy = 0;
 e.call(W_Lock, {}); int32_t run = g.run, stop = g.stop; e.call(W_Unlock, {});
 for (;;) {
  if (stop) { e.call(10006, {}); e.call(10024, {0, 1}); e.call(W_DeleteCS, {}); e.call(W_Debug, {}); return 1; }
  bool idle = false;
  if (run == 0) idle = true;
  else {
   e.call(10040, {}); busy = (int32_t)e.call(10008, {1});
   if (busy) { e.call(10024, {0, 0}); idle = true; } else e.call(W_Sleep, {10});
  }
  if (idle) {
   if (busy == 0) e.call(W_Sleep, {10});
   else {
    e.call(W_Sleep, {0x1e});
    int32_t cb = rd<int32_t>(g.sess, 0x221);
    if (cb) {
     int32_t v = (int32_t)e.call(10053, {1});
     if (v != last) {
      if (last < g.thrFlag) { last = v; if (g.thrFlag <= v) e.call(R(cb + 0x90), {}); }
      else {
       last = v;
       if (v < g.thrFlag) {
        uint8_t b = rd<uint8_t>(g.sess, 0x20c) == 1; if (rd<uint8_t>(g.sess, 0x20c) == 2) b = 2;
        if (rd<uint16_t>(g.sess, 0x1eb) & 4) b |= 4;
        if (rd<uint16_t>(g.sess, 0x1eb) & 0x20) b |= 8;
        e.call(R(cb + 0x34), {(W)(g.sess + 0x259), b});
       }
      }
     }
    }
   }
  }
  e.call(W_Lock, {}); run = g.run; stop = g.stop; e.call(W_Unlock, {});
 }
}

// 1.13c D2CLIENT+0x46230  (out=in_EAX, opt=stack ptr, buf/bufCap=stack text buffer)
struct NpcG { int32_t* rng; int32_t p973b, c390, a97a8, c34; };
int32_t sub_46230(Env& e, NpcG& g, int32_t* out, uint8_t* unit, int32_t* opt, char16_t* buf, int32_t bufCap) {
 *out = 0;
 if (!unit || e.call(11088, {}) == 4) return 0;
 auto cur = [&]() -> int64_t { W p = e.call(R(0x46150), {}); return p ? rd<int32_t>((void*)p, 4) : -1; };
 auto send = [&](int32_t mode) { return (int32_t)e.call(10107, {(W)g.rng, (W)unit, g.c390, g.p973b, cur(), mode}); };
 int32_t r;
 enum { Gen, NpcMenu, Buy } path = Gen;
 if (!opt) { r = send(g.a97a8 ? 2 : 0); goto done; }
 {
  e.call(R(0x46150), {});
  if (!e.call(R(0x46150), {})) return 0;
  uint32_t c = (uint32_t)cur(); e.call(10695, {rd<int32_t>(unit, 4)});
  if (c < 0xfd) {
   if (c != 0xfc) switch (c) {
    case 0x93: case 0x94: case 0xb1: case 199: case 0xca: case 0xf6: break;
    case 0x9a: case 0xb2: path = NpcMenu; break;
    case 0xf4: case 0xf5: path = Buy; break;
    default: return 0;
   }
  } else if (c < 0x10a) {
   if (c == 0x109) path = Buy;
   else switch (c) { case 0xfd: case 0x101: path = NpcMenu; break; case 0xfe: case 0xff: break; default: return 0; }
  } else switch (c) { case 0x195: case 0x200: case 0x201: case 0x202: break; case 0x1ff: path = NpcMenu; break; default: return 0; }
  if (path == Buy) {
   if (e.call(R(0x45BD0), {}) == 1) { *opt = 0; return 1; }
   r = (e.call(10174, {g.p973b, 4, 0}) != 0) ? 0 : 100; goto done;
  }
  if (path == NpcMenu && g.c34 == 4) {
   if (e.call(10202, {(W)unit, 0x10, 0x147b, 0}) == 0) {  // "..\\Source\\D2Client\\UI\\npcmenu.cpp"
    e.call(U_Strcat, {(W)buf, e.call(10003, {})}); e.call(R(0xBF5F0), {1}); return 1; }
   if (e.call(R(0x45B40), {}) == 0) return 0;
   r = send(3); goto done;
  }
  if (e.call(10705, {(W)unit}) == 0) return 0;
  r = send(1);
 }
done:
 *out = r;
 W s = e.call(10003, {});
 if (bufCap < (int32_t)e.call(U_Strlen, {s}) + 10) return 0;
 e.call(U_Strcpy, {(W)buf, s});
 char16_t num[40] = {}; e.call(W_Win2Uni, {(W)num, r, 0x13}); e.call(U_Strcat, {(W)buf, (W)num});
 return 1;
}

// 1.13c D2CLIENT+0x46910  (edi = state: +5 table, +9 count, +0xd pick; entry stride 0xf)
struct PickG { uint8_t* rngOwner; int32_t p973b, p973f; uint8_t* defTable; };
void sub_46910(Env& e, PickG& g, uint8_t* edi) {
 int32_t owner = g.rngOwner ? rd<int32_t>(g.rngOwner, 4) : -1;
 for (int n = 9; ; --n) {
  uint32_t cnt = rd<uint32_t>(edi, 9), pick;
  if ((int32_t)cnt < 1) pick = 0;
  else {
   uint64_t seed = (uint64_t)rd<uint32_t>(g.rngOwner, 0x20) * 0x6ac690c5ull + rd<uint32_t>(g.rngOwner, 0x24);
   wr<uint64_t>(g.rngOwner, 0x20, seed);
   pick = (cnt & (cnt - 1)) == 0 ? ((cnt - 1) & (uint32_t)seed) : (uint32_t)((seed & 0xffffffffu) % cnt);
  }
  wr<uint32_t>(edi, 0xd, pick);
  if ((int32_t)pick > 1) {
   uint8_t* tbl = rdp<uint8_t>(edi, 5); uint8_t* ent = tbl + pick * 0xf;
   int32_t k = rd<int32_t>(tbl, 0xb + pick * 0xf);
   if (k == owner || k == 7) {
    if (rd<uint8_t>(ent, 2) == 0) return;
    int32_t v = (int32_t)e.call(10174, {g.p973b, rd<int32_t>(ent, 7), 0});
    if (v == rd<int32_t>(tbl, rd<int32_t>(edi, 0xd) * 0xf + 3)) {
     if (tbl != g.defTable) return;
     if (rd<int16_t>(g.defTable, rd<int32_t>(edi, 0xd) * 0xf) != 0xff) return;
     if (e.call(10174, {g.p973f, 0xc, 0xd}) != 1) return;
     wr<int32_t>(edi, 0xd, 2); return;
    }
   }
  }
  if (n == 0) { wr<int32_t>(edi, 0xd, 2); return; }
 }
}

// 1.13c D2CLIENT+0x46A60
struct DelG { uint8_t** buckets /*0x80*/; int32_t a97ac, a97b0; uint8_t* list /*bf30 node: {unit*, next}*/; };
void sub_46A60(Env& e, DelG& g, uint32_t id) {
 uint8_t* p = g.buckets[id & 0x7f];
 for (; p; p = rdp<uint8_t>(p, 0xe4)) if (rd<uint32_t>(p, 12) == id) {
  if (rd<int32_t>(p, 0) != 1) fatal(e, 0x5a);
  e.call(R(0x460A0), {}); break;
 }
 if (g.a97ac && g.a97b0)
  for (uint8_t* n = g.list; n; n = rdp<uint8_t>(n, sizeof(void*)))
   if (rd<int32_t>(rdp<uint8_t>(n, 0), 8) == g.a97b0) { e.call(R(0x88550), {(W)rdp<uint8_t>(n, 0)}); g.a97ac = g.a97b0 = 0; return; }
 g.a97ac = g.a97b0 = 0;
}

// 1.13c D2CLIENT+0x47920
struct ResetG { int32_t a975f; uint8_t a9663; int32_t cc2f8, cc2f4; };
void sub_47920(Env& e, ResetG& g) {
 if (g.a975f) { e.call(R(0xA3D20), {}); g.a975f = 0; }
 g.a9663 = 3; e.call(R(0x8F660), {}); e.call(R(0x94970), {}); g.cc2f8 = 0; g.cc2f4 = 0;
}

// 1.13c D2CLIENT+0x47970  (in_EAX = packet: +1 byte slot, +2 int id)
struct PktG { uint8_t* slots /*bba608, 0x80 each*/; int32_t p973b; uint8_t cc33e; };
void sub_47970(Env& e, PktG& g, const uint8_t* pkt) {
 uint8_t sl = pkt[1];
 uint8_t* u = (uint8_t*)e.call(R(0xA4E20), {rd<int32_t>(pkt, 2), (W)(g.slots + sl * 0x80), sl});
 if (!u) return;
 if (rd<int32_t>(u, 4) == 0x216) { e.call(R(0x88A70), {(W)u, 0, 0, 0}); return; }  // both arms of the 0x3c test make the same call
 if (u == (uint8_t*)e.call(R(0x46150), {})) return;
 e.call(R(0x6B300), {(W)u, 3, 0, 0, 0});
 if (rd<int32_t>(u, 4) != 0x14b || e.call(R(0x46150), {}) || !g.p973b) return;
 if (e.call(10174, {g.p973b, 0xc, 8}) != 0 || e.call(10174, {g.p973b, 0xc, 1}) != 0) return;
 g.cc33e = 0; e.call(R(0xA5360), {});
 if (g.cc33e == 0) e.call(R(0x88A70), {(W)u, 0, 0, 0});
}

// 1.13c D2CLIENT+0x4BE70  ordinal 10004 QueryInterface: returns the fixed interface table
void* const* QueryInterface(void* const* ifaceTable) { return ifaceTable; }

// 1.13c D2CLIENT+0x57A90  (ebp/esi = lost register inputs forwarded to Ordinal_10440/10074)
int32_t sub_57A90(Env& e, const uint8_t* dt, uint8_t* unit, int32_t idx, W p3, W ebp, W esi) {
 if (idx < 0 || idx >= rd<int32_t>(dt, 0xba0)) return 0;
 const uint8_t* rec = rdp<uint8_t>(dt, 0xb98) + idx * 0x23c;
 int32_t cls = rd<int16_t>(rec, 0xec);
 if (cls < 0 || cls >= rd<int32_t>(dt, 0xb6c)) return 0;
 int32_t a[28] = {};
 if (!e.call(R(0xA1340), {(W)(a + 1)})) return 0;
 if (unit) wr<uint32_t>(unit, 0xc4, rd<uint32_t>(unit, 0xc4) | 0x40);
 e.call(10786, {(W)unit, rd<int32_t>(rec, 100), idx, p3});
 int32_t tail = a[24]; std::memset(a, 0, 23 * 4);
 a[0] = 1; a[12] = tail; a[4] = 0xbf; a[1] = (int32_t)(intptr_t)unit; a[11] = idx;
 W p = e.call(R(0xB30F0), {(W)a});
 if (!p) return 0;
 e.call(10440, {p, ebp}); e.call(10074, {p, esi});
 return 1;
}

// 1.13c D2CLIENT+0x4A4F0
struct UiG {
 int32_t w46c8, w46c4, cc3dc, cc3e0, cc3f8, cc3fc, cc2f8, cc2f4; uint8_t a9663, a9729, a9721;
 int32_t a971d, a9725, a97ac, a97b0, p973b, ada0, d5d0, c3a68, a23e8; uint8_t* bbfc; int32_t* aad80;
 uint8_t* ptrTab20[1]; uint8_t* t37a8; int32_t n3b9c; uint8_t* defTbl; int32_t a966b;
};
void sub_4A4F0(Env& e, UiG& g, uint8_t* u, int32_t uStack4, uint8_t* uiTypeTab[]) {
 g.w46c8 = g.w46c4 = 0x10; g.cc3dc = g.cc3e0 = g.cc3f8 = g.cc3fc = g.cc2f8 = g.cc2f4 = 0;
 if (!u) { e.call(R(0x48150), {}); return; }
 g.a9663 = 1; g.a9729 = 0; g.a966b = (int32_t)e.call(10973, {(W)g.bbfc, 0xe, 0});
 e.call(R(0xBE350), {0, 0});
 if (uStack4 == 0) {
  if (u == (uint8_t*)e.call(R(0x46150), {})) return;
  if (e.call(R(0x46150), {})) { e.call(R(0x48150), {}); return; }
 } else e.call(R(0x46150), {});
 g.a9721 = 1; g.a971d = rd<int32_t>(u, 12); g.a9725 = rd<int32_t>(u, 4);
 int32_t cls = rd<int32_t>(u, 4); wr<uint32_t>(u, 0xc4, rd<uint32_t>(u, 0xc4) & ~2u);
 e.call(R(0x45CC0), {(W)g.defTbl, cls, g.p973b});
 if (g.a97ac) { if (g.a97b0) { e.call(R(0x88BF0), {g.a97b0}); g.a97b0 = 0; } g.a97ac = 0; }
 uint32_t c = rd<uint32_t>(u, 4); bool call060 = true; uint32_t arg = 0;
 if (c < 0xfd) {
  if (c == 0xfc || c == 0xc6) arg = g.a23e8 != -1;
  else if (c == 0x96) { if (e.call(10973, {(W)g.bbfc, 0xc, 0}) > 7) e.call(R(0x4A210), {}); arg = g.a23e8 != -1; }
  else call060 = false;
 } else if (c == 0x16f || c == 0x203) arg = g.a23e8 != -1; else call060 = false;
 if (call060) e.call(R(0x4A060), {arg});
 int32_t prevAda0 = g.ada0; e.call(10227, {});
 if (g.d5d0 != 0x26) fatal(e, 0x139b);
 auto closeOut = [&]() { g.a9663 = 0; wr<uint32_t>(u, 0xc4, rd<uint32_t>(u, 0xc4) | 2); e.call(R(0x143E0), {}); g.a9721 = 0; e.call(R(0x46A60), {rd<int32_t>(u, 12)}); };
 if (g.c3a68) { closeOut(); return; }
 for (int32_t i = 0; i < g.d5d0; ++i) if (g.aad80[i] == 1) {
  int32_t k = rd<int32_t>(uiTypeTab[i], 0x20);
  if (k == 0) continue;
  if (k == 1) e.call(R(0xD2790), {i, 1, 0});
  else if (k == 4) { if (g.a9721) e.call(R(0x47B20), {}); }
  else { closeOut(); return; }
 }
 if (!g.bbfc || (rd<int32_t>(g.bbfc, 0) == 0 && e.call(R(0xA5440), {}) == 0)) g.ada0 = 1;
 if (prevAda0 && !g.ada0) { e.call(R(0xBE2E0), {}); e.call(R(0x492F0), {}); }
 e.call(R(0x48B20), {});
 if (uStack4) return;
 int32_t flag = 0, idx = 0;
 for (int32_t i = 0; i < g.n3b9c; ++i) if (rd<int32_t>(g.t37a8, i * 0x16) == cls) {
  uint8_t* r = g.t37a8 + i * 0x16;
  if (r[0x12] == 1) { r[0x12] = 0; flag = 2; }
  if (r[0x15] != 1) return;
  r[0x15] = 0; break;
 }
 (void)idx;
 if (e.call(R(0x9CCE0), {flag}) == 0) return;
 e.call(R(0x22EB0), {});
 g.a97b0 = (int32_t)e.call(R(0x88A70), {(W)g.bbfc, 0, 1, 0});
 if (flag == 2) { char m[4] = {0x4d, (char)cls, (char)(cls >> 8), 0}; e.call(R(0x143E0), {(W)m}); g.a97ac = 1; return; }
 g.a97b0 = 0;
}

// 1.13c D2CLIENT+0x4B5D0
struct ScrG { int32_t a9781, a975f, a9663, cbc34, a9729, a9737, b834, a978e, a9786; uint8_t* a975b; uint8_t** buckets;
  int32_t b828, b824, cc2f8, cc2f4; uint8_t d22f8, d22f9[4]; uint8_t* bbfc; };
void sub_4B5D0(Env& e, ScrG& g, W cb) {
 W who = rd<int32_t>(g.bbfc, 0x60);
 if (g.a9781 > 6) {
  if (g.a975f) { e.call(R(0xA3D20), {}); g.a975f = 0; }
  int32_t id;
  if (g.a9663 == 5) {
   g.a9663 = 6; g.cbc34 = 7;
   if (!g.a975b) {
    W v = 0;
    if (g.a9729 == 2 && (v = e.call(11017, {who}), g.a9737 == 1) && v && g.b834 == 0) e.call(R(0x162D0), {});
    g.a975b = (uint8_t*)e.call(R(0xA3E40), {g.b828, g.b824, cb, 0, 0, 1, 1, 0, 0});
    if (g.a975b) {
     wr<uint32_t>(g.a975b, 0x5c, 1);
     e.call(R(0xA36F0), {(W)g.a975b, e.call(10003, {}), 0x15, 0, 1, 0, 0});
     e.call(R(0xA36F0), {(W)g.a975b, e.call(10003, {}), 0xf, 0, 1, cb, 1});
     return;
    }
    id = 0x529; g.a975b = nullptr;
   } else id = 0x1399;
  } else id = 0x13ad;
  fatal(e, id);
 }
 if (g.a9781 == 0 || g.a9781 == 1) {
  e.call(R(0x88A70), {0, 0, 0, 0});
  if (g.a9781 == 0) { /* d2 clears a9753 */ }
  uint8_t* p = g.buckets[g.a9786 & 0x7f];
  while (p && rd<int32_t>(p, 12) != g.a9786) p = rdp<uint8_t>(p, 0xe4);
  if (!p) { g.a978e = 1; return; }
  if (rd<int32_t>(p, 0) != 4) fatal(e, 0x5a);
  if (g.a9781 == 1) { g.a978e = 1; return; }
 } else if (g.a9781 == 5) e.call(R(0x5AC00), {});
 g.a978e = 0;
 if (g.a975f) { e.call(R(0xA3D20), {}); g.a975f = 0; }
 g.cc2f8 = 0; g.cc2f4 = 0;
 g.d22f8 = 0; std::memset(g.d22f9, 0, 4); g.a9663 = 3;
 e.call(R(0x8F660), {}); e.call(R(0x94970), {});
}

// 1.13c D2CLIENT+0x55B20  (dst/hasPrefix/hasDst = stack locals the decompile leaves unnamed)
void sub_55B20(Env& e, const uint8_t* dt, uint8_t* unit, W thisU, W p4, W p5, char16_t* dst, int32_t sep, int32_t hasDst) {
 char16_t tmp[0x100], tmp2[0x100];
 e.call(U_Init, {(W)tmp, 0x80}); e.call(U_Init, {(W)tmp2, 0x80});
 if (e.call(11088, {(W)unit}) == 0xb) { e.call(R(0x54880), {(W)unit, (W)unit, 0}); return; }
 W a = e.call(10930, {(W)unit, p4, p5});
 if (!a) return;
 int32_t t = unit ? rd<int32_t>(unit, 0) : 6, cls = unit ? rd<int32_t>(unit, 8) : 0, id = unit ? rd<int32_t>(unit, 12) : -1;
 W h = e.call(11013, {cls, 0, 0, t, id});
 e.call(10308, {h, a});
 if (thisU) { W b = e.call(10930, {(W)unit, thisU, p5}); if (b) e.call(10308, {h, b}); }
 if (unit && rd<int32_t>(unit, 0) == 4 && rd<int32_t>(unit, 0x60)) {
  for (W n = e.call(10460, {rd<int32_t>(unit, 0x60)}); n; ) {
   W it = e.call(11147, {n}); if (!it) break;
   n = e.call(10464, {n});
   W c = e.call(10930, {it, hasDst, sep}); if (c) e.call(10308, {h, c});
  }
 }
 W s1 = e.call(10003, {}), s2 = e.call(10003, {}), s3 = e.call(10003, {});
 if (hasDst && e.call(10744, {(W)unit, 0x39}) && e.call(10910, {(W)unit, 0x7a, 0}) == 0) {
  e.call(R(0x54780), {(W)tmp2}); e.call(U_Strcat, {(W)dst, (W)tmp2});
 }
 e.call(R(0x54000), {});
 int32_t any = 0, nStat = rd<int32_t>(dt, 0xbdc);
 for (int32_t i = 0; i < nStat; ++i) {
  int32_t sid = rd<int16_t>(rdp<uint8_t>(dt, 0xbd8), i * 2);
  if (sid < 0 || sid >= rd<int32_t>(dt, 0xbd4)) continue;
  uint16_t rec[0x800]; int32_t n = (int32_t)e.call(11110, {h, sid, (W)rec, 0x1ff});
  for (int32_t k = 0; k < n; ++k) {
   uint16_t layer = rec[k * 4]; int32_t val = rd<int32_t>(rec, 4 + k * 8);
   if (!val || e.call(R(0x54280), {sid, (W)dst}) != 0 || !e.call(R(0x54E10), {(W)unit, h, layer})) continue;
   if (sid == 0x17 && e.call(10680, {h, 0x15, 0})) continue;
   if (sid == 0x18 && e.call(10680, {h, 0x16, 0})) continue;
   if (sep) e.call(U_Strcat, {(W)tmp, s1});
   if (any && !sep) { e.call(U_Strcat, {(W)dst, s3}); e.call(U_Strcat, {(W)dst, s2}); }
   any = 1; e.call(U_Strcat, {(W)dst, (W)tmp});
  }
 }
 if (unit && rd<int32_t>(unit, 0) == 4) {
  const uint8_t* mon = (const uint8_t*)e.call(10695, {rd<int32_t>(unit, 4)});
  if (mon[0x113] == 0 && mon[0x112] != 0 && e.call(10973, {(W)unit, 0x98, 0}) < 1 && e.call(11116, {(W)unit}) == 0) {
   e.call(10003, {});
   if (any && !sep) { e.call(U_Strcat, {(W)dst, 0}); e.call(U_Strcat, {(W)dst, 0}); }
   e.call(U_Strcat, {(W)dst, 0});
   if (sep) e.call(U_Strcat, {(W)dst, 0});
  }
 }
 e.call(11108, {h});
}

// 1.13c D2Client+0x56330  (eax = unit, esi = dst, stack = ebp-arg passed through to 56210/56280; ret 4)
// Each of 4 rows: dst += S(0xf9e); tmp = S(id) + S(0xf9b); helper(mode, v, tmp); modes/ids 2/2b42, 1/2b41, 1/2b44, 0/2b43.
void sub_56330(Env& e, uint8_t* unit, char16_t* dst, W arg) {
 char16_t tmp[0x200]; e.call(U_Init, {(W)tmp, 0x100});
 if (!e.call(10744, {(W)unit, 0x35})) fatal(e, 0x21a);
 W rec = e.call(10695, {unit ? rd<int32_t>(unit, 4) : -1});
 if (!rec) fatal(e, 0x21c);
 dst[0] = 0;
 static const int mode[4] = {2, 1, 1, 0}; static const int sid[4] = {0x2b42, 0x2b41, 0x2b44, 0x2b43};
 bool a = e.call(10744, {(W)unit, 0x14}) != 0;
 if (!a && !e.call(10744, {(W)unit, 0x4a})) return;
 W v = e.call(10806, {rd<int32_t>((void*)rec, 0xf0)}); if (!v) fatal(e, a ? 0x223 : 0x245);
 for (int i = 0; i < 4; ++i) {
  e.call(U_Strcat, {(W)dst, e.call(10003, {0xf9e})}); tmp[0] = 0;
  e.call(U_Strcat, {(W)tmp, e.call(10003, {sid[i]})}); e.call(U_Strcat, {(W)tmp, e.call(10003, {0xf9b})});
  if (a) e.call(R(0x56210), {mode[i], v, (W)dst, (W)unit, arg, (W)tmp});
  else e.call(R(0x56280), {mode[i], v, (W)dst, arg, (W)tmp});
 }
 for (int i = 0; i < 2; ++i) e.call(U_Strcat, {(W)dst, e.call(10003, {0xf9e})});
}
}  // namespace d2client_n86
