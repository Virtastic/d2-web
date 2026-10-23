// Prior art: none (D2MOO has no D2Client); derived from cached 1.13c decompile.
// Env-seam style: every import / global is a callback; the control block is a byte buffer (32-bit layout).
#include <cstdint>
#include <cstring>
namespace d2client_a3840 {
using U = uint32_t; using I = int32_t;
static U rd(const uint8_t* p, U o) { U v; std::memcpy(&v, p + o, 4); return v; }
static void wr(uint8_t* p, U o, U v) { std::memcpy(p + o, &v, 4); }
struct Env {
 void* user;
 void (*fillRect)(void*, I l, I t, I r, I b, I a, I b2);  // Ordinal 10014 (unverified), stdcall 6
 void (*border)(void*, const I rect[4]);                   // sub_BE4C0
 void (*drawCel)(void*, const U ctx[18], I x, I y, I a, I mode, I c); // Ordinal 10041 (D2Gfx), ctx = 72 bytes
 void (*child)(void*, U obj);                              // vtable[1] of list node's object
 U (*ord10184)(void*);                                     // Ordinal 10184 (unverified; font/colour get-set)
 void (*ord10150)(void*, I y, U v, I z);                   // Ordinal 10150 (unverified; text draw)
 void (*fatal)(void*);                                     // Ordinal 10265(0x499)+10024 then _exit(-1)
 U (*vtbl1)(void*, U obj);                                 // resolves obj->vtable[1] target
 U selCel;  // DAT_6fbcb9c8 (RVA unrecorded)
 U* tick;   // DAT_6fbbb20c counter (read, then ++)
};
// 1.13c D2Client+0xA3840 ; ESI = control block `w`
void sub_A3840(uint8_t* w, Env& e) {
 if (!w) { e.fatal(e.user); return; }
 U ctx[18] = {};
 I t = I(rd(w, 0x30)), l = I(rd(w, 0x2c));
 wr(w, 0x54, rd(w, 0x54) + 1);
 I b = t + I(rd(w, 0x38)), r = I(rd(w, 0x34)) - 1 + l;
 if (rd(w, 0xc) == 0) {
  e.fillRect(e.user, l, t, r + 1, b, 0, 1);
  I rc[4] = {l, t, r, b - 1};  // local_58..local_4c
  e.border(e.user, rc);
 } else if (rd(w, 8)) {
  ctx[0] = 0; ctx[13] = rd(w, 8);  // local_14 = ctx+0x34
  e.drawCel(e.user, ctx, l, b, -1, 5, 0);
 }
 for (U n = rd(w, 0xb08); n; n = rd(reinterpret_cast<uint8_t*>(uintptr_t(n)), 4)) {
  U o = rd(reinterpret_cast<uint8_t*>(uintptr_t(n)), 0);
  if (o) e.child(e.user, o);
 }
 U font = e.ord10184(e.user);
 I acc = 0;
 for (I i = 0; i < I(rd(w, 0x50)); ++i) {
  const U it = 0x168 + 0x110 * U(i);
  U col = rd(w, it + 4);
  e.ord10184(e.user);
  I x = I(rd(w, it - 0xc)) + I(rd(w, 0x2c));
  I y = I(rd(w, it - 0x10)) + acc + I(rd(w, 0x30));
  if (I(rd(w, 0x44)) == i) {
   if (rd(w, 0x5c) == 1) col = 3;
   if (rd(w, 0x5c) == 2) {
    U c2[18] = {}; c2[13] = e.selCel; c2[0] = *e.tick % 7;
    e.drawCel(e.user, c2, x - 0x18, y + 4, -1, 5, 0);
    // second call passes &uStack_60 (the colour word) as the cell context
    U c3[18] = {}; c3[0] = col;
    e.drawCel(e.user, c3, I(rd(w, it - 8)) + 2 + x, y + 4, -1, 5, 0);
    ++*e.tick;
   }
  }
  e.ord10150(e.user, y, col, 0);
  acc += I(rd(w, it - 0x10));
 }
 (void)font; e.ord10184(e.user);
}
}
