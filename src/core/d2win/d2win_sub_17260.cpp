// 1.13c D2Win+0x17260 -- screenshot writer: pick first unused "ScreenshotNNN.jpg", JPEG-encode frame.
// Callees are weak seams (file probe/close, JPEG encoder ordinals 2/3/5); the 20 KB stack frame is a zeroed
// encoder state whose layout stays opaque. Hidden ESI input (frame width) is an explicit param.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <emscripten.h>
extern "C" {
__attribute__((weak)) int32_t Ordinal_10107(int32_t);
__attribute__((weak)) void Ordinal_10109(void);
__attribute__((weak)) void Ordinal_2(void*);
__attribute__((weak)) void d2win_Ordinal_3_free(void*);
__attribute__((weak)) void Ordinal_5(uint32_t, void*) {} // host seam: Fog+Ordinal_5 has no wasm analogue; no-op unless a strong definition is linked
extern int32_t DAT_6f8ffba4_screenshot_n;
int32_t DAT_6f8ffba4_screenshot_n;
EMSCRIPTEN_KEEPALIVE void d2win_sub_17260(int32_t frame, int32_t width) {  // 1.13c D2Win+0x17260
    char name[224];
    int32_t st[0x1390 + 4];
    uint8_t hdr[8];
    for (;;) {
        snprintf(name, sizeof name, "Screenshot%03d.jpg", (int)DAT_6f8ffba4_screenshot_n);
        DAT_6f8ffba4_screenshot_n++;
        if (!Ordinal_10107 || Ordinal_10107(0) == -1) break;
        Ordinal_10109();
    }
    memset(st, 0, sizeof st);
    if (Ordinal_2) Ordinal_2(st);
    st[0] = frame;
    st[3] = (int32_t)((((uint32_t)width * 3 + 3) & ~3u) - (uint32_t)width * 3);  // row pad to 4 bytes
    if (Ordinal_5) Ordinal_5(8, hdr);
    if (d2win_Ordinal_3_free) d2win_Ordinal_3_free(hdr);
}
}
