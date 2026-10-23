// Prior art: none (D2MOO has no D2Client); derived from cached 1.13c decompile.
// Env-seam style: every import / global is a callback; the window is a byte buffer (32-bit layout).
#include <cstdint>
#include <cstring>
namespace d2client_a6d60 {
using U = uint32_t;
static U rd(const uint8_t* p, U o) { U v; std::memcpy(&v, p + o, 4); return v; }
static void wr(uint8_t* p, U o, U v) { std::memcpy(p + o, &v, 4); }
struct Env {
 void* user;
 void (*f8630)(void*);              // sub_A8630 (called with ESI/none)
 U (*ord10327)(void*, uint8_t*, U); // Ordinal 10327 (unverified)
 void (*ord10043)(void*, U, U);     // Ordinal 10043 (unverified)
 void (*f23120)(void*);             // sub_23120
 void (*f22B20)(void*, uint8_t*);   // sub_22B20
 void (*f2c580)(void*);             // sub_7C580
 void (*f19130)(void*);             // sub_19130 (case 0)
 void (*f4c900)(void*);             // sub_4C900 (case 1)
 void (*f24990)(void*);             // sub_24990 (case 2)
 void (*ord10234)(void*, uint8_t*); // Ordinal 10234 (unverified)
 void (*f0e70)(void*);              // sub_B0E70 (case 3)
 void (*f82550)(void*);             // sub_82550 (case 4)
 void (*ord11095)(void*, U, uint8_t*); // Ordinal 11095 (unverified)
 void (*ord11016)(void*, uint8_t*); // Ordinal 11016 (unverified)
 void (*f21540)(void*);             // sub_21540
 void (*fABD40)(void*);             // sub_ABD40
 void (*fC0250)(void*);             // sub_C0250 (current-window teardown)
 void (*ord10124)(void*, uint8_t*); // Ordinal 10124 (unverified)
 U dat_bcc3b8;                      // DAT_6fbcc3b8 (RVA unrecorded)
 uint8_t** curWin;                  // DAT_6fbcbbfc (RVA unrecorded)
};
// 1.13c D2Client+0xA6D60 ; p = window (uint32 array)
void sub_A6D60(uint8_t* p, Env& e) {
 if (rd(p, 0x19 * 4)) { e.f8630(e.user); wr(p, 0x19 * 4, 0); }
 if (e.ord10327(e.user, p, 0)) e.f8630(e.user);
 U c = rd(p, 0x15 * 4);
 if (c && rd(reinterpret_cast<uint8_t*>(uintptr_t(c)), 0x30)) {
  e.ord10043(e.user, 100, 0);
  c = rd(p, 0x15 * 4);
  if (c) wr(reinterpret_cast<uint8_t*>(uintptr_t(c)), 0x30, 0);
 }
 e.f23120(e.user);
 e.f22B20(e.user, p);
 auto clr29 = [&] { if (rd(p, 0x29 * 4)) { e.f2c580(e.user); wr(p, 0x29 * 4, 0); } };
 switch (rd(p, 0)) {
 case 0: clr29(); e.f19130(e.user); e.f23120(e.user); break;
 case 1: clr29(); e.f4c900(e.user); e.f23120(e.user); break;
 case 2: clr29(); e.f24990(e.user); e.ord10234(e.user, p); e.f23120(e.user); break;
 case 3: e.f0e70(e.user); break;
 case 4: e.f82550(e.user); e.ord11095(e.user, 0, p); e.ord10234(e.user, p); break;
 case 5: if (e.dat_bcc3b8) e.ord11016(e.user, p); e.ord10234(e.user, p); break;
 }
 e.f21540(e.user);
 e.fABD40(e.user);
 if (p == *e.curWin) { if (*e.curWin) e.fC0250(e.user); *e.curWin = nullptr; }
 e.ord10124(e.user, p);
}
}
