#include <cstdint>
#include <cstring>
#include <emscripten.h>

// Each function takes the storage of the retail global(s) it touches; the probes pin them with blobs.
// 1.13c D2Client+0x4BE80: stores arg0 (stdcall, 3 args, ret 0xC), returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_4be80(uint32_t* g330, uint32_t a) { *g330 = a; return 1; }
// 1.13c D2Client+0x52600: eax=p; global = p ? dword [p+0xC] : -1.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_52600(uint32_t* g2134, const uint8_t* p) {
    if (!p) { *g2134 = 0xffffffffu; return; }
    memcpy(g2134, p + 0xc, 4);
}
// 1.13c D2Client+0x42BB0: ecx=p; if byte [p+4] set, both flags := 1.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_42bb0(uint32_t* g17c8, uint32_t* g2a8, const uint8_t* p) {
    if (p[4] != 0) { *g17c8 = 1; *g2a8 = 1; }
}
// 1.13c D2Client+0x45D30: ecx=v; append cx to the 9-word list at 0x119792 while its word counter (0x1197A6, at words+0x14) < 9.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_45d30(uint8_t* words, uint32_t v) {
    uint16_t n; memcpy(&n, words + 0x14, 2);
    if (n < 9) { uint16_t w = (uint16_t)v; memcpy(words + 2 * n, &w, 2); n++; memcpy(words + 0x14, &n, 2); }
}
