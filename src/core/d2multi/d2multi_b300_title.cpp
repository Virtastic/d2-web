// 1.13c D2Multi+0xB300 (character box): the title word put before the character name, from D2Win #10205
// (0xBD00) which calls #10132 (0xBB50). Stack arguments at the call (0xB44C): class (+0x1EA), progression
// ((flags >> 8) & 0x1F), flags & 4 (adds three to a non-zero bucket), flags & 0x20 (selects the 5/10/15
// thresholds and is passed on as the third argument of 0xBB50).
#include <cstdint>
#include <emscripten.h>
extern "C" const char* d2win_sub_BB50(int32_t cls, int32_t title, int32_t hc);

extern "C" EMSCRIPTEN_KEEPALIVE
const char* d2_lobby_char_title(int32_t cls, int32_t progression, int32_t flag4, int32_t flag20) {
    int32_t i;
    if (progression < (flag20 ? 5 : 4)) i = 0;
    else if (progression < (flag20 ? 10 : 8)) i = 1;
    else i = (progression >= (flag20 ? 15 : 12)) ? 3 : 2;
    if (flag4 && i) i += 3;
    return d2win_sub_BB50(cls, i, flag20 != 0);
}
