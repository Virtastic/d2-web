// 1.13c D2Client+0xC39E0 -- the in-game UI pass of the frame, as the sequence of D2Gfx calls it makes (G-trace seam, the
// tail of d2_client_frame_g). Order and gates are the decompile's; it runs after the room walk inside D2Client+0x44230:
//   C39E0: 10031 (resolution; != 2 at 640x480), every panel gated off on the play screen (aadc8/aada4/aadac/aae0c/...),
//          1610's 10031 gated off (cc3b8 == 0), then 29250:
//   29250: 10078, 10031 (== 2 would pick 271C0/270D0), 27290 control panel (5 pieces at 640: x 0 / w/2-0x9b + 128 i / w-0x75),
//          27BD0 life orb (clip only when the fill is non-zero, then the frame at 0x1c, h-5), 27A90 mana orb (10078, the clip
//          always, the frame at w-0x6e, h-9), 26970 (silent), 28320 run/walk button (w/2-0x91, h-10), 278B0 stamina bar
//          (10014 rect, width = cur*0x66/max, colour = the red palette index when width < 0x19), 26880 (w/2-8, h-0x10),
//          28DC0 belt items (one 10041 + 10013 per filled slot), 28CE0 -> 28BC0 -> 3E5E0 the two orb sockets (10019 at 0x75 and
//          w-0xa5, y h), 289A0 / 267B0 (silent);
//   back in C39E0: aad98 == 0 -> 50100, aad9c == 0 -> 50020 (each a lone 10031 when the resolution is not 2); the rest silent.
//   D2Win#10024 (D2Win+0x187E0): 10043 gate, 10080, 10085(0,0), 10021(0), the form list (no in-game forms: silent), 10040.
//   44230 tail: 5A9A0 (the FPS counter: +1 per frame within a second, else it restarts) leaves its ecx for the final
//   10040; 10021(0) first while cA46cc > 0.
// Register leftovers (ecx/edx a no-register D2Gfx export is entered with) are the retail values at 640x480 and come in fixed;
// the 10040 leftover is the FPS counter, driven by the caller's clock.
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
constexpr uint32_t PTR = 0xffffffffu;   // a cell-context pointer / -1: printed as '*'
uint32_t fpsCount = 0, fpsStart = 0;    // 0x11C2B4, 0x11C2B0
}
// w, h: the screen (0xDBC48/0xDBC4C); life, mana: the orb fills (0..80, 27BD0/27A90); stam_cur, stam_max: 278B0's inputs;
// a46cc: 0x11A46CC; clock_ms: GetTickCount for 5A9A0. Returns the G lines.
extern "C" EMSCRIPTEN_KEEPALIVE const char* d2_client_ui_g(int32_t w, int32_t h, int32_t life, int32_t mana, int32_t stam_cur,
                                                           int32_t stam_max, int32_t a46cc, uint32_t clock_ms) {
 out.clear();
 const uint32_t W = (uint32_t)w, H = (uint32_t)h, w2 = (uint32_t)(w / 2);
 rec(10031, 0, 0);                                                         // C39E0 head
 rec(10078, 1, 1); rec(10031, 1, 1);                                       // 29250
 rec(10031, 0, 1);                                                         // 27290
 rec(10041, H, 0, {PTR, 0, H, PTR, 5, 0});
 rec(10041, 0, 0, {PTR, w2 - 0x9b, H, PTR, 5, 0});
 rec(10041, 0, 0, {PTR, w2 - 0x1b, H, PTR, 5, 0});
 rec(10041, H, 0, {PTR, w2 + 0x65, H, PTR, 5, 0});
 rec(10041, W - 0x75, 0, {PTR, W - 0x75, H, PTR, 5, 0});
 if (life > 0) rec(10074, 0x1d, 0, {PTR, 0x1d, H - 13, 0, (uint32_t)life, 5});   // 27BD0
 rec(10041, 0, 0, {PTR, 0x1c, H - 5, PTR, 5, 0});
 rec(10078, 0xa, 0);                                                       // 27A90
 rec(10074, W - 0x6f, 0, {PTR, W - 0x6f, H - 13, 0, (uint32_t)mana, 5});
 rec(10041, H - 9, W - 0x6e, {PTR, W - 0x6e, H - 9, PTR, 5, 0});
 rec(10041, H - 10, 0, {PTR, w2 - 0x91, H - 10, PTR, 5, 0});                // 28320
 {                                                                          // 278B0
  int32_t width = stam_max > 0 ? (int32_t)((uint32_t)stam_cur * 0x66u) / stam_max : 0;
  const uint32_t x1 = w2 - 0x7f, y1 = H - 0x1b, col = 0x62;                // pal[0] (0xff,0,0) -> 0x62; width >= 0x19 is unmodelled
  rec(10014, y1, x1, {x1, y1, x1 + (uint32_t)width, y1 + 0x12, col, 2});
 }
 rec(10041, H - 0x10, 0, {PTR, w2 - 8, H - 0x10, PTR, 5, 0});              // 26880
 rec(10019, 0, 0, {PTR, 0x75, H, PTR, 5, 1});                              // 28CE0
 rec(10019, 0, 0, {PTR, W - 0xa5, H, PTR, 5, 1});
 rec(10031, 4, 0x30000); rec(10031, 4, 0x30000);                           // 50100, 50020
 rec(10043, 0, 0); rec(10080, 0, 0, {PTR, PTR}); rec(10085, 0, 0, {0, 0, 0, 0}); rec(10021, 0, 0, {0}); rec(10040, 0, 0);   // D2Win#10024
 const uint32_t delta = clock_ms - fpsStart;                               // 5A9A0: ecx keeps the counter, or the delta on a restart
 uint32_t ecx;
 if (delta < 0x3e9) ecx = ++fpsCount; else { fpsCount = 0; fpsStart = clock_ms; ecx = delta; }
 if (a46cc > 0) rec(10021, 0, 0, {0});
 rec(10040, ecx, 0);
 return out.c_str();
}
