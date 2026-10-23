// 1.13c D2Win+0x11520 (127 B, __fastcall ECX=box): tear down a type-9 (video) control's playback.
// Type != 9 is fatal (Fog #10265(0xA2) -> #10024 -> _exit(-1)). If box+8 has both gdwBitMasks[2] and
// gdwBitMasks[0] set: when the playing flag (0x6F9A9E20) is set, re-run 0x114A0 with the saved path
// (0x6F9A9D18) if that path is non-empty, then clear the flag; then call the import #10024 on box+0x140.
// Returns 1; returns 0 when the mask test fails. Seams: the import and 0x114A0 via env.
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

struct D2Win11520Env {
    const uint32_t* bitMasks;                       // gdwBitMasks
    void (*sub114A0)(void* box, const char* path);  // 0x114A0
    void (*import10024)(void* p);                   // Ordinal_10024(box+0x140)
};
extern "C" {
extern char d2win_g_6f9a9d18[0x104];  // defined in gfx/d2win_batch_93c0.cpp
uint32_t d2win_g_6f9a9e20;
}

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_11520(void* box, const D2Win11520Env* env) {
    uint8_t* b = (uint8_t*)box;
    uint32_t type, flags;
    std::memcpy(&type, b, 4);
    if (type != 9) _exit(-1);
    std::memcpy(&flags, b + 8, 4);
    if ((env->bitMasks[2] & flags) && (env->bitMasks[0] & flags)) {
        if (d2win_g_6f9a9e20) {
            if (d2win_g_6f9a9d18[0]) env->sub114A0(box, d2win_g_6f9a9d18);
            d2win_g_6f9a9e20 = 0;
        }
        env->import10024(b + 0x140);
        return 1;
    }
    return 0;
}
