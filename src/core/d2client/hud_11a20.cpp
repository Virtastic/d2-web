#include <cstdint>
#include <emscripten.h>

extern "C" uint32_t d2_client_11410(uint32_t f, uint32_t k);
extern "C" int32_t d2_client_11630(int32_t W, int32_t H, int32_t w0, int32_t w1, int32_t w2,
                                   int32_t G, int32_t D, int32_t P, uint32_t k);

// 1.13c D2Client+0x11A20 (no args): menu init = 0x11410 (layout by flag), sprintf "DATA\GLOBAL\UI\Menu\TextSlid" into a dead
// local, [0x6FBCC544] = sub_2B420(0) (stubbed in capture: returns 0xB420CAFE), 0x11630 (button rects + y), then
// [0x6FBCC534] = [0x6FBCC538] = 0. Inputs: flag, W, H, P(before), widths w[3] (D2Win text metrics).
// k: 0 ptr, 1 G, 2 D, 3 X, 4..15 rects o[12], 16 p534, 17 p538, 18 p544, 19 y, 20 2B420 calls, 21 its arg,
// 22/23 SetFont args (13, 119 as recorded, constant across the corpus), 24+j sprintf string byte j (0 past the end).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_11a20(uint32_t flag, int32_t W, int32_t H, int32_t P,
                                                         int32_t w0, int32_t w1, int32_t w2, uint32_t k) {
    static const char s[] = "DATA\\GLOBAL\\UI\\Menu\\TextSlid";
    const int32_t G = (int32_t)d2_client_11410(flag, 1), D = (int32_t)d2_client_11410(flag, 2);
    if (k < 4) return (int32_t)d2_client_11410(flag, k);
    if (k < 16) return d2_client_11630(W, H, w0, w1, w2, G, D, P, k - 4);
    if (k == 18) return (int32_t)0xB420CAFE;
    if (k == 19) return d2_client_11630(W, H, w0, w1, w2, G, D, P, 12);
    if (k == 20) return 1;
    if (k == 22) return 13;
    if (k == 23) return 119;
    if (k >= 24 && k < 24 + sizeof(s)) return (uint8_t)s[k - 24];
    return 0;
}
