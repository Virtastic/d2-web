// Browser entry for the animated-image control's pointer test: runs the 1.13c D2Win+0x107C0 rule (d2win_sub_107C0) for a
// control anchored at (x, y) whose current image's first frame is w x h with offsets (ox, oy), against the point (mx, my).
// The cel-context imports are answered from that frame record; the pointer globals are restored after the call.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2WinBatch10030Env {
    int32_t (*d2cmp10086)(void*);
    void* (*alloc)(uint32_t, const char*, int32_t, int32_t);
    uint32_t (*renderFEE0)(void*);
    uint32_t (*render104B0)(void*);
    void* (*getCelFromCelContext)(void*);
    uint32_t (*cel4)(void*);
    uint32_t (*cel8)(void*);
    uint32_t (*celC)(void*);
    uint32_t (*cel10)(void*);
    uint32_t (*getTickCount)(void);
    const uint32_t* gdwBitMasks;
};
extern "C" int32_t d2win_sub_107C0(void* obj, const D2WinBatch10030Env* env);
extern "C" uint32_t d2win_g_6f901488, d2win_g_6f90148c;

namespace {
uint32_t g_cel[5];
void* seam_cel(void*) { return g_cel; }
uint32_t seam_4(void* c) { return ((uint32_t*)c)[1]; }
uint32_t seam_8(void* c) { return ((uint32_t*)c)[2]; }
uint32_t seam_c(void* c) { return ((uint32_t*)c)[3]; }
uint32_t seam_10(void* c) { return ((uint32_t*)c)[4]; }
}  // namespace

extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_win_anim_hit(int32_t mx, int32_t my, int32_t x, int32_t y, int32_t w, int32_t h, int32_t ox, int32_t oy) {
    static uint32_t header = 1, entry[4], obj[0x18];
    static const D2WinBatch10030Env env = { nullptr, nullptr, nullptr, nullptr, seam_cel, seam_4, seam_8, seam_c, seam_10, nullptr, nullptr };
    g_cel[0] = 0; g_cel[1] = (uint32_t)w; g_cel[2] = (uint32_t)h; g_cel[3] = (uint32_t)ox; g_cel[4] = (uint32_t)oy;
    entry[0] = (uint32_t)(uintptr_t)&header;
    std::memset(obj, 0, sizeof obj);
    obj[0] = 2; obj[3] = (uint32_t)x; obj[4] = (uint32_t)y;
    obj[0x10] = (uint32_t)(uintptr_t)entry;   // image table at +0x40, state 0 at +0x50
    const uint32_t sx = d2win_g_6f901488, sy = d2win_g_6f90148c;
    d2win_g_6f901488 = (uint32_t)mx; d2win_g_6f90148c = (uint32_t)my;
    const int32_t r = d2win_sub_107C0(obj, &env);
    d2win_g_6f901488 = sx; d2win_g_6f90148c = sy;
    return r;
}
