// 1.13c D2Client+0x4B5D0: the trade reply handler (0x2A, reached from 0x4BC80 while 0x6FBC9663 == 5).
// Result codes 0..6 are a success cleanup; a code above 6 closes the trade, sets 0x6FBC9663 = 6 and
// 0x6FBCBC34 = 7, and opens the NPC text menu (0x6FB53E40 at 0x6FBCB828/0x6FBCB824, box+0x5c = 1) with the
// refusal string in colour 1 (red, spacing 0x15) over a selectable white 0xD4B "Continue" (callback 0x4AAF0).
// This returns the refusal string id for a result code, 0 for no box.
#include <cstdint>
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_4b5d0_refusal_string(int32_t code) {
    static const uint16_t kRefusal[10] = { 0xd1e, 0xd1e, 0xd21, 0xd1c, 0xd20, 0xd22, 0xd21, 0xd21, 0xd23, 0xd1d };   // codes 7..16
    if (code <= 6) return 0;
    return (code >= 7 && code <= 16) ? kRefusal[code - 7] : 0xd21;
}
