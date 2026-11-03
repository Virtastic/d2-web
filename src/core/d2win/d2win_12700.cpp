// 1.13c D2Win+0x12700 -- GetPopupUnicodeTextWidthAndHeight
// ECX = text, EDX = &width, [esp+4] = &height; ret 4. Calls 0x11FB0 (EAX = text) and stores its result into [&width],
// then calls 0x11E80 (ESI = text) and stores its result into [&height]; returns the height (eax).
// Callees are seams: the probe mirrors them (src/core/fn/d2win_12700.cpp).
#include <cstdint>
#include <emscripten.h>

struct D2Win12700Env {
    int32_t (*lineWidth)(const uint16_t* text);     // 1.13c D2Win+0x11FB0
    int32_t (*blockHeight)(const uint16_t* text);   // 1.13c D2Win+0x11E80
};

extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2win_12700(const uint16_t* text, int32_t* outWidth, int32_t* outHeight, const D2Win12700Env* env) {
    *outWidth = env->lineWidth(text);
    *outHeight = env->blockHeight(text);
    return *outHeight;
}
