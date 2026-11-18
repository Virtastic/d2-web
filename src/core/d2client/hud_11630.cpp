#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x11630 (no args): lay out three 0x1A-byte button records at [0x6FBA8B98 + 0x1A*i] and set y [0x6FBD33F4].
// Retail pulls the three label widths w[i] from D2Win (SetFont(13), D2Lang string of the WORD at rec+0, text width);
// here they are inputs. W=[0x6FB8BC48], H=[0x6FBA9E18], G=[0x6FBCC4FC], D=[0x6FBCC500], P=[0x6FBCC534].
// k 0..11: record i=k/4, field k%4 -> +0x0A,+0x0E,+0x12,+0x16 ints (0x6FBA8BA2,A6,AA,AE); k 12: y.
// M==0 faults in retail (idiv); returns 0 here.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_11630(int32_t W, int32_t H, int32_t w0, int32_t w1, int32_t w2,
                                                         int32_t G, int32_t D, int32_t P, uint32_t k) {
    const int32_t w[3] = { w0, w1, w2 };
    const int32_t c = (W - 0x26C) / 2, s = (int32_t)((uint32_t)(W - c * 2) / 3u), by = (H - 0x1A4) / 2;
    if (k == 12) {
        int32_t M = G;
        for (int i = 0; i < 15; i++) { if (--M < 1) { M = 0; break; } }
        return M ? ((0xF6 - 12 * D) * P) / M + 0x2F + by : 0;
    }
    const int32_t i = (int32_t)(k / 4) % 3, x = s * i + c + s / 2, h = w[i] / 2;
    const int32_t o[4] = { x - h - 10, x + h + 10, by + 0x149, by + 0x16F };
    return o[k & 3];
}
