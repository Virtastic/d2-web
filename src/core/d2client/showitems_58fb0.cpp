// Prior art: none (D2MOO has no D2Client module); derived from the Ghidra decompile.
// Every ordinal / D2Client callee is a seam (Env::call), unverified. Lost register inputs (unaff_EDI etc.) are
// explicit: edi = the label-top base carried in from the caller (CONCAT13 byte in uVar15 is the Ordinal_10190 result).
#include <cstdint>
#include <cstring>
#include <initializer_list>
namespace d2client_showitems58fb0 {
using W = int64_t;
constexpr uint32_t R(uint32_t rva) { return 0x1000000u + rva; }
struct Env { virtual ~Env() = default; virtual W call(uint32_t id, std::initializer_list<W> a) = 0; };
template<class T> T rd(const void* p, unsigned o) { T v; std::memcpy(&v, static_cast<const uint8_t*>(p)+o, sizeof v); return v; }

// label slot = 0x48 ints at DAT 0xBCFEF8: [0..3] l,t,r,b  [4] unit  [5] 0  [0x45] flag  [0x46] mode  [0x47] colour
struct ShowG {
 int32_t* slots;      // 0xBCFEF8
 int32_t& cnt;        // 0xBC7800
 int32_t* const p_bbfc_flag;  // 0xBCBBFC
 int32_t a995c, a9960, c418, c414, scrW, scrH;  // 0xBC995C 0xBC9960 0xBCC418 0xBCC414 0xB8BC48 0xB8BC4C
 int32_t& cc2f8; int32_t& cc2f4;                // 0xBCC2F8 0xBCC2F4
 int32_t b828, b824;                            // 0xBCB828 0xBCB824
};
// 1.13c D2CLIENT+0x58FB0
void sub_58FB0(Env& e, ShowG& g, int32_t edi) {
 e.call(10013, {});
 if (!*g.p_bbfc_flag) return;
 int32_t room = (int32_t)e.call(10331, {});
 if (!room) return;
 e.call(R(0x51A80), {});
 int32_t out[7] = {};  // apiStack_20[0]=selected unit, [1]=cnt?, [2]=list, [3]=n, [6..7]=cursor xy (iStack_28/24)
 e.call(10383, {room, (W)(intptr_t)out});
 uint8_t col = (uint8_t)e.call(10190, {0, 0x40, 0x80});
 int32_t n = 0; bool placed = false; g.cnt = 0;
 uint32_t nRooms = (uint32_t)out[3];
 for (uint32_t ri = 0; ri < nRooms; ri++) {
  int32_t* room_ = (int32_t*)(intptr_t)rd<int32_t>((const void*)(intptr_t)out[2], ri * 4);
  for (int32_t* u = (int32_t*)(intptr_t)room_[0x74/4]; u; u = (int32_t*)(intptr_t)u[0x3a]) {
   if (!(((uint32_t)u[0x32] >> 7) & 1) || u[0] != 4 || n >= 0x20) continue;
   int32_t* s = g.slots + n * 0x48;
   s[4] = (int32_t)(intptr_t)u;
   int32_t x, y;
   if (out[1] == 0) {
    int32_t d = g.a9960 - g.c418;
    x = (int32_t)e.call(10651, {(W)(intptr_t)u}) - d;
    y = (int32_t)e.call(11142, {}) - 0x10 - (g.a995c - 8);
   } else {
    W a = e.call(11080, {}), b = e.call(11168, {(W)(intptr_t)u});
    if (!e.call(10012, {a, b})) continue;
    int32_t pt[2] = {};
    e.call(10023, {a, b, 0, (W)(intptr_t)&pt[0], (W)(intptr_t)&pt[1]});
    x = pt[0]; y = pt[1];
    if (g.c414 == 1) x -= g.scrW / 4; else if (g.c414 == 2) x += g.scrW / 4;
   }
   int32_t minX = 0, maxX = g.scrW;
   if (g.c414 == 1) maxX = g.scrW / 2 - 0x20; else if (g.c414 == 2) minX = g.scrW / 2;
   if (!(minX <= x && x <= maxX && -9 < y && y <= g.scrH - 0x10)) continue;
   s[5] = 0;
   int32_t rect[4] = {};
   e.call(R(0x6FEC0), {});
   e.call(10177, {(W)(intptr_t)rect});
   int32_t w = rect[2] + 8;           // text width + padding
   int32_t l = x - (w >> 1), r = w + l, t = y + 4, b = y;
   bool ok = false;
   for (int32_t mode = 0; mode < 0xc && !ok; mode++)
    ok = e.call(R(0x58DE0), {minX, (W)(intptr_t)&l, (W)(intptr_t)&t, (W)(intptr_t)&r, (W)(intptr_t)&b, mode, maxX}) != 0;
   if (!(r <= maxX && minX <= l && ok)) continue;
   s[0] = l; s[1] = t; s[2] = r; s[3] = b;
   if (!placed) {
    int32_t cx = out[5], cy = out[6];
    if (cx < l || r < cx || cy < t || b + 4 < cy) {
     if (u == (int32_t*)(intptr_t)out[0]) {   // hovered unit: highlighted
      s[0x45] = col; s[0x46] = 5; s[0x47] = 0;
      e.call(10085, {0, 0, 0}); g.cc2f8 = 0; g.cc2f4 = 0; placed = true; goto next;
     }
    } else { s[0x47] = 0; s[0x45] = col; s[0x46] = 5; e.call(R(0x51E30), {}); placed = true; goto next; }
   }
   {
    s[0x45] = 0; s[0x46] = 1;
    uint32_t c = 0;
    switch ((uint32_t)e.call(10106, {(W)(intptr_t)u})) {
     case 4: c = 3; break; case 5: c = 2; break; case 6: c = 9; break;
     case 7: c = 4; break; case 8: c = 8; break; case 9: c = 10; break;
     default:
      if ((e.call(10202, {(W)(intptr_t)u, 0x800, 0x106}) && e.call(10757, {(W)(intptr_t)u}) && (char)e.call(11151, {(W)(intptr_t)u})) ||
          e.call(10202, {(W)(intptr_t)u, 0x400000, 0x10c})) c = 5;
    }
    uint8_t* rec = (uint8_t*)(intptr_t)e.call(10695, {u[1]});
    if (rec[0x12a] && rd<int32_t>(rec, 0x80) != 0x2067656c) c = 4;
    s[0x47] = (int32_t)c;
   }
  next:
   n++; g.cnt = n; (void)edi;
  }
 }
 for (int32_t i = 0; i < g.cnt; i++) { int32_t* s = g.slots + i * 0x48; e.call(10013, {s[3], s[0x45], s[0x46], s[0x47]}); }
 if (!placed) { g.cc2f8 = 0; g.cc2f4 = 0; e.call(10085, {0, 0, 0}); }
}
}  // namespace d2client_showitems58fb0
