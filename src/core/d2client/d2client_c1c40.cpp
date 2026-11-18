// D2Client+0xC1C40: screen-state change switch (EAX = state), ported against the retail virtual addresses and register conventions.
#include "d2client_c1c40.h"
namespace d2client_c1c40 {
namespace {
struct T { uint32_t tag; int32_t n; };
// state to (tag, EAX count) of the plain "open" states, from the retail call sites
const T kOpen[0x26] = {
 {0x6fb80580,4},{0x6fb805b0,4},{0x6fb80628,2},{0x6fb8061c,1},{0x6fb80604,2},{0,0},{0x6fb80640,2},{0x6fb80664,2},{0,0},{0x6fb80680,7},{0x6fb80780,5},{0,0},{0x6fb8bc60,8},
 {0,0},{0,0},{0x6fb8bcc0,7},{0x6fb80810,2},{0x6fb807bc,4},{0,0},{0x6fb80658,1},{0x6fb80828,5},{0x6fb80864,2},{0x6fb8087c,2},{0x6fb8bd18,0xd},{0,0},{0x6fb8bdb8,8},{0x6fb8be18,7},
 {0,0},{0,0},{0,0},{0,0},{0x6fb8be6c,2},{0,0},{0x6fb80924,5},{0x6fb80960,2},{0x6fb80978,3},{0x6fb8099c,3},{0x6fb809c0,2}};
void op(const Env& e, uint32_t tag, int32_t n) { e.open(n, e.o10048(), tag); }
void cl(const Env& e, uint32_t tag, int32_t n) { e.close(n, e.o10048(), tag); }
// states 0xC / 0x19 / 0x1A: enter the in-game UI mode
void enter2(const Env& e) {
 e.wr32(0x6fbcc434, 2);
 if (e.rd32(0x6fbd349c) == 0) {
  e.wr32(0x6fbd349c, 1);
  if (e.rd32(0x6fbd3498) != 0) { cl(e, 0x6fba51ec, 2); e.wr32(0x6fbd3498, 0); }
  op(e, 0x6fba51bc, 4);
 }
}
// states 5 / 0x17: leave it
void leave2(const Env& e) {
 e.wr32(0x6fbcc434, 0);
 if (e.rd32(0x6fbd349c) == 1) {
  e.wr32(0x6fbd349c, 0); cl(e, 0x6fba51bc, 4);
  if (e.rd32(0x6fbd3498) == 0) { op(e, 0x6fba51ec, 2); e.wr32(0x6fbd3498, 1); }
 }
}
}
// 1.13c D2Client+0xC1C40
void sub_C1C40(const Env& e, uint32_t st) {
 if (st > 0x25) return;
 switch (st) {
 case 1: e.call0(0x8bb90); break;
 case 5:
  e.call0(0x6f5b0); leave2(e);
  if (e.rd32(0x6fbaada8)) cl(e, 0x6fb80780, 5);
  return;
 case 9: op(e, 0x6fb80680, 7); e.wr32(0x6fbcc2f8, 1); e.wr32(0x6fbcc2f4, 0); return;
 case 10: op(e, 0x6fb80780, 5); e.wr32(0x6fbcb9ac, 1); return;
 case 0xb: e.f12dd0(1); e.call0(0xbe920); return;
 case 0xc: enter2(e); e.call0(0x911a0); e.fbfa10(0x6fb805e0); op(e, 0x6fb8bc60, 8); return;
 case 0xf: e.fbfa10(0x6fb807ec); op(e, 0x6fb8bcc0, 7); return;
 case 0x16: op(e, 0x6fb8087c, 2); e.call0(0x772e0); return;
 case 0x17:
  leave2(e); e.wr32(0x6fbc77fc, 0); e.wr32(0x6fbc77ec, 0); e.wr8(0x6fbcc2d0, 1); e.wr32(0x6fbbb348, 0);
  e.fbfa10(0x6fb80894); op(e, 0x6fb8bd18, 0xd); return;
 case 0x18: e.call0(0x7c770); return;
 case 0x19: enter2(e); e.call0(0x8cbc0); e.fbfa10(0x6fb808b8); op(e, 0x6fb8bdb8, 8); return;
 case 0x1a: enter2(e); e.call0(0x8cb40); e.fbfa10(0x6fb808dc); op(e, 0x6fb8be18, 7); return;
 case 0x1f: e.fbfa10(0x6fb80900); op(e, 0x6fb8be6c, 2); return;
 default: break;
 }
 if (kOpen[st].tag) op(e, kOpen[st].tag, kOpen[st].n);
}
}
