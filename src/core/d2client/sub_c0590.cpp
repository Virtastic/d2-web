// D2Client+0xC0590: scale of the unit's pending-event progress, 0..0x80 (env-seam, unverified).
#include <cstdint>
namespace d2client_c0590 {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P);
 P player;                                   // *0x6fbcbbfc
 void (*f62a0)(P);                           // D2Client+0xA62A0 (arg: player; other inputs are unrecovered registers)
 int32_t (*f21580)(int32_t id, int32_t* edx); // D2Client+0x21580 percent; edx = register left after the call (unrecovered)
 int32_t (*o10913)();                        // D2Common ordinal via thunk 0xC196 (unverified ordinal)
 int32_t (*o10973)();                        // D2Common ordinal via thunk 0xC1C6 (unverified ordinal)
};
// 1.13c D2Client+0xC0590  (unit arrives in EDI)
int32_t sub_C0590(const Env& e, P u) {
 if (!u || e.rd32(u) != 1 || (e.rd32(u + 0xc4) & 0x10000) || e.rd32(u + 0x10) == 0 || e.rd32(u + 0x10) == 0xc) return 0;
 int32_t s = 0x80;
 if ((e.rd32(u + 0xc4) >> 2) & 1) e.f62a0(e.player);
 int32_t edx = 0;
 int32_t p = e.f21580((int32_t)e.rd32(u + 0xc), &edx);
 if (edx == 0 && p > 0) s = (p << 7) / 100;
 else {
  int32_t d = e.o10913();
  if (d == 0) goto clamp;
  s = (e.o10973() << 7) / d;
 }
 if (s < 3) return 2;
clamp:
 return s > 0x7f ? 0x80 : s;
}
}
