// Prior art: none (D2MOO has no D2Client); derived from the 1.13c decompile and the retail disassembly.
// Env-seam style: every callee goes through one host call carrying its register and stack inputs; the window is a byte buffer (32-bit layout).
#include <cstdint>
#include <cstring>
namespace d2client_a6d60 {
using U = uint32_t;
static U rd(const uint8_t* p, U o) { U v; std::memcpy(&v, p + o, 4); return v; }
static void wr(uint8_t* p, U o, U v) { std::memcpy(p + o, &v, 4); }
// One callee call: rva (D2Client RVA), the register inputs it reads, and its stack words (stdcall ordinals).
struct Call { U rva, eax, ecx, edi, esi, s0, s1; };
struct Env {
 void* user;
 U (*host)(void*, const Call*);
 const U* dat_bcc3b8;   // D2Client+0x11C3B8
 U* curWin;             // D2Client+0x11BBFC
};
// 1.13c D2Client+0xA6D60 ; p = window (uint32 array)
void sub_A6D60(uint8_t* p, Env& e) {
 const U w = U(uintptr_t(p));
 auto call = [&](U rva, U eax = 0, U ecx = 0, U edi = 0, U esi = 0, U s0 = 0, U s1 = 0) { Call c{rva, eax, ecx, edi, esi, s0, s1}; return e.host(e.user, &c); };
 U esi = rd(p, 0x19 * 4);                                  // 0x64
 if (esi) { call(0xA8630, 0, 0, 0, esi); wr(p, 0x19 * 4, 0); }
 U r = call(0xC5C2, 0, 0, 0, 0, w, 0);                      // Ordinal 10327(w, 0)
 if (r) { esi = r; call(0xA8630, 0, 0, 0, esi); }
 U c = rd(p, 0x15 * 4);                                     // 0x54
 if (c && rd(reinterpret_cast<uint8_t*>(uintptr_t(c)), 0x30)) {
  call(0xBF86, 0, 0, 0, 0, 0x64, 0);                        // Ordinal 10043(100, 0), EDX = a module constant
  c = rd(p, 0x15 * 4);
  if (c) wr(reinterpret_cast<uint8_t*>(uintptr_t(c)), 0x30, 0);
 }
 call(0x23120, 0, 0, w);
 call(0x22B20, 0xFFFFFFFFu, w);
 auto clr29 = [&] { if (rd(p, 0xA4)) { call(0x7C580); wr(p, 0xA4, 0); } };
 switch (rd(p, 0)) {
 case 0: clr29(); call(0x19130, w); call(0x23120, 0, 0, w); break;
 case 1: clr29(); call(0x4C900, w); call(0x23120, 0, 0, w); break;
 case 2: clr29(); call(0x24990, 0, 0, w); call(0xC5DA, 0, 0, 0, 0, w); call(0x23120, 0, 0, w); break;
 case 3: call(0xB0E70, 0, 0, 0, w); break;
 case 4: call(0x82550, w); call(0xC64C, 0, 0, 0, 0, 0, w); call(0xC5DA, 0, 0, 0, 0, w); break;
 case 5: if (*e.dat_bcc3b8) call(0xC3CA, 0, 0, 0, 0, w); call(0xC5DA, 0, 0, 0, 0, w); break;
 default: break;
 }
 call(0x21540, 0, 0, w);
 call(0xABD40, 0, 0, w);
 const U cur = *e.curWin;
 if (w == cur) { if (cur) call(0xC0250); *e.curWin = 0; }
 call(0xC5E6, 0, 0, 0, 0, w);                               // Ordinal 10124(w)
}
}
extern "C" void d2c_sub_a6d60(uint8_t* p, d2client_a6d60::Env* e) { d2client_a6d60::sub_A6D60(p, *e); }
