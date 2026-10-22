// D2Client options-list scrollbar click/drag (x87 body done in double; FPU precision of retail unverified).
#include <cstdint>
namespace d2client_64fd0 {
typedef int32_t I; typedef uint32_t U;
struct Env {
 U (*rd32)(U addr);  void (*wr32)(U addr, U v);
 I (*hitTest_64080)();            // returns active entry idx
 void (*call)(U fn);              // guest call, no args (cb 0x110 returns via ret32)
 U (*call_ret)(U fn);
 void (*refresh_38a70)();         // FUN_6fb38a70(0,0,0,0)
 U g_58, g_60, g_6c;              // DAT_6fbcc058 / 060 / 06c addresses
 U screenW;                       // DAT_6fb8bc48 value
};
// 1.13c D2Client+0x64FD0  (ECX unused-but-saved ebx = ecx; stack arg = mouse Y)
void scrollbar_64fd0(const Env& e, I y, U ecx) {
 U ebx = ecx; I idx;
 if (e.rd32(e.g_6c) == 0) { idx = e.hitTest_64080(); if (idx != (I)e.rd32(e.g_58)) return; }
 else idx = (I)e.rd32(e.g_58);
 U ent = e.rd32(e.g_60) + (U)idx * 0x550;
 ebx = e.rd32(ent + 0x124);
 U cb = e.rd32(ent + 0x110);
 if (!cb || e.call_ret(cb)) {
  if (e.rd32(ent) == 2) {
   I h = (I)e.screenW / 2, top = h - 0x3b, t2 = h - 0x3c;
   if (e.rd32(ent + 0x53c) == 0) { top = h - 0x90; t2 = h - 0x91; }
   if (e.rd32(e.g_6c) != 0 || (top < y && y < top + 0x121)) {
    I v;
    if (y < t2 + 0xc) v = 0;
    else if (y > t2 + 0xc + 0x109) v = (I)e.rd32(ent + 0x120) - 1;
    else {
     double a = (double)(y - (t2 + 0xc));
     I n = (I)e.rd32(ent + 0x120) - 1;
     double b = (double)n; if (n < 0) b += 4294967296.0;
     v = (I)(a / (132.5 / b) + 1.0);   // _ftol truncates; b==0 -> inf, retail UB
     v /= 2;                            // cdq/sub/sar: round toward zero
    }
    e.wr32(ent + 0x124, (U)v);
    e.wr32(e.g_6c, 1);
   }
  }
 }
 if (ebx != e.rd32(e.rd32(e.g_60) + (U)idx * 0x550 + 0x124)) {
  U cb2 = e.rd32(e.rd32(e.g_60) + (U)idx * 0x550 + 0x114);
  if (cb2) e.call(cb2);
  e.refresh_38a70();
 }
}
}
