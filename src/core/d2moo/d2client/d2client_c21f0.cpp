// D2Client+0xC21F0: screen-state leave switch (EAX = state), ported against the retail virtual addresses and register conventions.
#include "d2client_c21f0.h"
namespace d2client_c21f0 {
namespace {
void cl(const Env& e, uint32_t tag, int32_t n) { e.close(n, e.o10048(), tag); }
void op(const Env& e, uint32_t tag, int32_t n) { e.open(n, e.o10048(), tag); }
// the shared tail of states 0xC / 0x17 / 0x19 / 0x1A: enter the in-game UI mode
void enter1(const Env& e) {
 e.wr32(0x6fbcc434, 1);
 if (e.rd32(0x6fbd349c) == 0) {
  e.wr32(0x6fbd349c, 1);
  if (e.rd32(0x6fbd3498) != 0) { cl(e, 0x6fba51ec, 2); e.wr32(0x6fbd3498, 0); }
  op(e, 0x6fba51bc, 4);
 }
}
// the common reset of states 0x17 / 0x19 / 0x1A
void reset(const Env& e) {
 e.wr32(0x6fbcc418, 0); e.wr32(0x6fbcc414, 0);
 e.o10006();
 e.f8a4d0(e.rd32(0x6fba9e18), e.rd32(0x6fba9e14), 0, e.rd32(0x6fbcc3c4));
}
}
// 1.13c D2Client+0xC21F0
void sub_C21F0(const Env& e, uint32_t st) {
 if (st > 0x25) return;
 switch (st) {
 case 0: cl(e, 0x6fb80580, 4); return;
 case 1: e.call0(0xbf720); cl(e, 0x6fb805b0, 4); e.call0(0xbe2e0); return;
 case 2: e.call0(0xbf720); cl(e, 0x6fb80628, 2); e.call0(0xbe2e0); return;
 case 3: cl(e, 0x6fb8061c, 1); return;
 case 4: cl(e, 0x6fb80604, 2); e.call0(0xbe2e0); e.call0(0x77da0); return;
 case 5:
  e.call0(0x6f4a0);
  if (e.rd32(0x6fbaadb0) == 0 && e.rd32(0x6fbaaddc) == 0 && e.rd32(0x6fbaade4) == 0 && e.rd32(0x6fbaade8) == 0 && e.rd32(0x6fbaadec) == 0 &&
      e.rd32(0x6fbaadf0) == 0 && e.rd32(0x6fbaadf4) == 0 && e.rd32(0x6fbaadf8) == 0 && e.rd32(0x6fbaae00) == 0) {
   e.wr32(0x6fbcc434, 1);
   if (e.rd32(0x6fbd349c) == 0) {
    e.wr32(0x6fbd349c, 1);
    if (e.rd32(0x6fbd3498) != 0) { cl(e, 0x6fba51ec, 2); e.wr32(0x6fbd3498, 0); }
    op(e, 0x6fba51bc, 4);
   }
  }
  if (e.rd32(0x6fbaada8)) op(e, 0x6fb80780, 5);
  return;
 case 6: cl(e, 0x6fb80640, 2); return;
 case 7: cl(e, 0x6fb80664, 2); return;
 case 8: case 0xe: e.call0(0xbe2e0); e.call0(0x492f0); return;
 case 9: cl(e, 0x6fb80680, 7); e.wr32(0x6fbcc2f8, 0); e.wr32(0x6fbcc2f4, 0); return;
 case 10: cl(e, 0x6fb80780, 5); e.wr32(0x6fbcb9ac, 0); return;
 case 0xb:
  if (e.callr(0x42090) || e.callr(0x420e0)) cl(e, 0x6fb80774, 1);
  cl(e, 0x6fb806d8, 0xd); e.s1(0x12dd0, 0); return;
 case 0xc: cl(e, 0x6fb8bc60, 8); e.call0(0x492f0); enter1(e); return;
 case 0xf: cl(e, 0x6fb8bcc0, 7); e.call0(0x1d8f0); e.call0(0xbe2e0); return;
 case 0x10: cl(e, 0x6fb80810, 2); e.call0(0xbe2e0); return;
 case 0x11: cl(e, 0x6fb807bc, 4); e.call0(0x1c720); return;
 case 0x12: e.call0(0xbe2e0); return;
 case 0x13: cl(e, 0x6fb80658, 1); return;
 case 0x14: cl(e, 0x6fb80828, 5); e.call0(0xaa380); e.call0(0xbe2e0); return;
 case 0x15: cl(e, 0x6fb80864, 2); return;
 case 0x16: cl(e, 0x6fb8087c, 2); e.call0(0x74f90); e.call0(0xbe2e0); return;
 case 0x17:
  if (e.callr(0x59830) != 0) return;
  reset(e); e.call0(0xbf720); cl(e, 0x6fb8bd18, 0xd); e.call0(0xbe2e0); e.s1(0x59740, 0); enter1(e); return;
 case 0x18: e.call0(0x7c730); return;
 case 0x19: e.call0(0xbf720); reset(e); e.call0(0x8f140); cl(e, 0x6fb8bdb8, 8); e.call0(0xbe2e0); enter1(e); return;
 case 0x1a: reset(e); e.call0(0x97840); cl(e, 0x6fb8be18, 7); e.call0(0xbe2e0); enter1(e); return;
 case 0x1f: cl(e, 0x6fb8be6c, 2); return;
 case 0x21: e.call0(0x358e0); cl(e, 0x6fb80924, 5); return;
 case 0x22: cl(e, 0x6fb80960, 2); return;
 case 0x23: cl(e, 0x6fb80978, 3); return;
 case 0x24: cl(e, 0x6fb8099c, 3); e.call0(0x8bbf0); return;
 case 0x25: cl(e, 0x6fb809c0, 2); return;
 default: return;
 }
}
}
