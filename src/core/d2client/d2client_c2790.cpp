// D2Client+0xC2790: open/close/toggle one HUD panel (ECX = panel, EDX = mode, stack = a3, ret 4), ported against the retail virtual addresses.
#include "d2client_c2790.h"
namespace d2client_c2790 {
namespace {
const uint32_t kPanel = 0x6fbaad80, kAd94 = 0x6fbaad94, kC9854 = 0x6fbc9854, kCbbfc = 0x6fbcbbfc, kC418 = 0x6fbcc418, kC414 = 0x6fbcc414;
const uint32_t kW = 0x6fb8bc48, kH = 0x6fba9e14, kHy = 0x6fba9e18, kC3c4 = 0x6fbcc3c4, kCurY = 0x6fbcb824, kCurX = 0x6fbcb828;
int32_t q4(int32_t v) { return (int32_t)(v + ((v >> 31) & 3)) >> 2; }   // retail cdq / and 3 / add / sar 2
}
// 1.13c D2Client+0xC2790
uint32_t sub_C2790(const Env& e, uint32_t p, uint32_t mode, int32_t a3) {
 if (p >= 0x26) { e.fatal(0x13ff); return 0; }
 if (mode > 2) { e.fatal(0x1400); return 0; }
 const uint32_t slot = kPanel + p * 4;
 uint32_t old = e.rd32(slot);
 uint32_t o = e.f_bf8c();
 if ((o == 0 || e.rd32(kC9854) == 0) && (p == 0x23 || p == 0x24 || p == 0x25)) return 0;
 if (e.f_71a80(mode) == 0) return 0;
 uint32_t ptr = e.rd32(kCbbfc);
 if (mode == 0) {
  if (ptr == 0 || (e.rd32(ptr) == 0 && e.f_a5440() == 0)) e.wr32(slot, 1);
 } else if (mode == 1) {
  e.wr32(slot, 0);
 } else {
  if (ptr == 0) e.wr32(slot, e.rd32(slot) == 0);
  else if (e.rd32(ptr) == 0 && e.f_a5440() == 0) e.wr32(slot, e.rd32(slot) == 0);
  else if (p == 5) e.wr32(kAd94, e.rd32(kAd94) == 0);
 }
 uint32_t now = e.rd32(slot);
 if (old == 0) { if (now != 0) { e.f_c1c40(p); if (p == 5) e.f_6eb50(); } }
 else if (now == 0) e.f_c21f0(p);
 const uint32_t curY = e.rd32(kCurY); const int32_t x = (int32_t)e.rd32(kCurX);
 if (p - 1 > 0x24) return 1;
 // the retail jump table: byte index p-1 selects one of the groups below
 enum { G1 = 0, G2 = 1, G4 = 2, GB = 3, GC = 4, GE = 5, GN = 6 };
 static const uint8_t grp[37] = {0,1,6,2,6,6,6,6,6,6,3,4,6,5,1,1,6,6,6,1,6,1,6,6,4,4,4,4,4,6,6,4,6,6,6,1,1};
 const int g = grp[p - 1];
 if (g == GN) return 1;
 if (g == GB) { if (now == 0) e.f_11f80(); return 1; }
 const int32_t q = q4((int32_t)e.rd32(kW));
 // the 0x8A4D0 call after 0xD1B0: the screen height and the other registers are read after the thunk returns, as retail does; kind 0 = (H, 0), 1 = (H - H/4, -H/4), 2 = (H + H/4, H/4)
 auto go = [&](int kind) {
  const int32_t H = (int32_t)e.rd32(kH), hq = q4(H);
  e.f_8a4d0(e.rd32(kHy), kind == 1 ? H - hq : kind == 2 ? H + hq : H, 0, e.rd32(kC3c4), kind == 1 ? -hq : kind == 2 ? hq : 0); };
 auto zero = [&]() { e.wr32(kC418, 0); e.wr32(kC414, 0); e.f_d1b0(0); go(0); };   // the "panel closed" state
 auto lf = [&]() { e.wr32(kC418, (uint32_t)-q); e.wr32(kC414, 1); e.f_d1b0(-q); go(1); };   // sliding left, mode 1
 auto rt = [&]() { e.wr32(kC418, (uint32_t)q); e.wr32(kC414, 2); e.f_d1b0(q); go(2); };   // sliding right, mode 2
 auto lock = [&]() {   // 72aba: mode 3 with the overlay words cleared
  e.wr32(kC414, 3); e.f_d1b0(0); go(0);
  e.wr32(0x6fbcc3dc, 0); e.wr32(0x6fbcc3e0, 0); e.wr32(0x6fbcc3f8, 0); e.wr32(0x6fbcc3fc, 0); };
 auto half = [&]() { const int32_t W = (int32_t)e.rd32(kW); return (W - (W >> 31)) >> 1; };
 auto quarter = [&]() { return q4((int32_t)e.rd32(kW)); };
 if (g == GC) {   // 0xC, 0x19-0x1D, 0x20
  e.wr32(kC418, 0);
  if (now != 0) { lock(); return 1; }
  e.wr32(kC414, 0); e.f_d1b0(0); go(0); return 1;
 }
 if (g == GE) {   // 0xE
  if (now != 0) { lf(); return 1; }
  zero(); return 1;
 }
 if (g == G1 || g == G4) {   // 1 (with 8BBF0 first) and 4
  if (g == G1) e.f_8bbf0();
  const uint32_t k[7] = {0x6fbaad88, 0x6fbaadd0, 0x6fbaadbc, 0x6fbaadc0, 0x6fbaae10, 0x6fbaae14, 0x6fbaadd8};
  bool busy = false;
  for (int i = 0; i < 7; i++) if (e.rd32(k[i]) != 0) { busy = true; break; }
  if (!busy) {
   if (now == 0) {
    zero();
    if (a3 == 0) return 1;
    if (x >= half()) return 1;
    e.f_161b0(curY, quarter() + x); return 1;
   }
   lf();
   if (a3 == 0) return 1;
   if (x <= quarter()) return 1;
   e.f_161b0(curY, x - quarter()); return 1;
  }
  if (now == 0) { rt(); return 1; }
  e.wr32(kC418, 0); lock(); return 1;
 }
 // G2: 2, 0xF, 0x10, 0x14, 0x16, 0x24, 0x25
 if (e.rd32(0x6fbaad84) != 0 || e.rd32(0x6fbaad90) != 0) {
  if (now == 0) { lf(); return 1; }
  e.wr32(kC418, 0); lock(); return 1;
 }
 if (now == 0) {
  zero();
  if (a3 == 0) return 1;
  if (x <= half()) return 1;
  e.f_161b0(curY, x - quarter()); return 1;
 }
 rt();
 if (a3 == 0) return 1;
 { const int32_t W = (int32_t)e.rd32(kW), qq = quarter(); if (x >= W - qq) return 1; e.f_161b0(curY, qq + x); } return 1;
}
}
