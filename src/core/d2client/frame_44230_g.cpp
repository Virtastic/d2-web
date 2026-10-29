// 1.13c D2Client+0x44230 -- the per-frame render entry, as the sequence of D2Gfx calls it makes (G-trace seam, like
// d2_client_loading_g in d2client_batch_c0450.cpp). Order and gates are the decompile's:
//   714F0 (state 0: silent); Ordinal_10043 gate; 3B0C0 -> 2A9F0 -> [2A0D0, 10013] when the view rect (cc3c4+0xc/+0x10) is non-empty and
//   mode cc414 != 3; 10038 colour; 10085(437E0 = class row +0x218, 0, 0, 0); 8B110 (same view gate): 10020, 10008, 10013,
//   A9E30 -> A8800 10047, 14050 -> 10047, 10013; then 739E0's first call 10031; after the walk: 10021(0) while cA46cc > 0, 10040.
// Register leftovers (the ecx/edx a D2Common thunk or a prior store left) are not recoverable from C: they come in as arguments.
#include <cstdint>
#include <cstdio>
#include <string>
#include <initializer_list>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif
namespace {
std::string out;
void rec(int ord, uint32_t ecx, uint32_t edx, std::initializer_list<uint32_t> a = {}) {
 char b[24]; std::snprintf(b, sizeof b, "%d %x,%x", ord, ecx, edx); out += b;
 for (uint32_t v : a) { if (v >= 0x100000u) out += ",*"; else { std::snprintf(b, sizeof b, ",%x", v); out += b; } }
 out += '\n';
}
}
// phase 0: calls up to the room walk (10031 included). a..: view_nonempty, mode (cc414), class_row_218 (437E0), colour (10038 r=g=b),
// then the leftovers: 2A9F0's 10013 ecx/edx, 8B110's ecx (10020/10008/10013), A8800's 10047 ecx/edx, 14050's ecx.
// phase 1: after the walk: a46cc (>0: 10021(0) and decrement), then 10040. Returns the G lines.
extern "C" EMSCRIPTEN_KEEPALIVE const char* d2_client_frame_g(int32_t phase, int32_t view_nonempty, int32_t mode, uint32_t class_218, uint32_t colour,
                                                              uint32_t l_ecx, uint32_t l_edx, uint32_t e8b, uint32_t a_ecx, uint32_t a_edx, uint32_t e14, uint32_t gate_ecx) {
 out.clear();
 if (phase == 0) {
  rec(10043, gate_ecx, 0);                                                          // 44230: Ordinal_10043() == 0 -> draw
  if (view_nonempty && mode != 3) { rec(10013, 0x12, 0x12); rec(10013, l_ecx, l_edx); }   // 3B0C0 -> 2A9F0 (2A0D0 then 10013)
  rec(10038, colour, colour, {colour, colour, colour});
  rec(10085, 0, 0, {class_218, 0, 0, 0});
  if (view_nonempty && mode != 3) {                                                 // 8B110
   rec(10020, e8b, 0); rec(10008, e8b, 0); rec(10013, e8b, 0);
   rec(10047, a_ecx, a_edx);                                                        // A9E30 (frame counter advanced) -> A8800
   rec(10047, e14, 0); rec(10013, e14, 0);                                          // 14050 head
  }
  rec(10031, 0, 0);                                                                 // 739E0 head
 } else {
  if (view_nonempty > 0) rec(10021, 0, 0, {0});   // view_nonempty carries cA46cc here
  rec(10040, 0, 0);
 }
 return out.c_str();
}
