// The play path's door into D2Sound's pure sound-placement rows. The JavaScript mixer keeps only Web Audio output;
// the gain/pan bytes and the pan's decibel level come from these proved functions.
#include <cstdint>
#include <emscripten.h>

void D2Sound_8B50(int32_t* left, int32_t* right, float x, float y, float z);
float D2Sound_8830(float a, float b, float c);

static int32_t ftolTrunc(float v) { return (int32_t)(int64_t)v; }

// 1.13c D2sound+0x8B50 (ordinal 10047): gain byte in bits 0..7, pan byte in bits 8..15.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_sound_spatial(float x, float y, float z) {
    int32_t l = 0, r = 0;
    D2Sound_8B50(&l, &r, x, y, z);
    return (l & 0xFF) | ((r & 0xFF) << 8);
}

// 1.13c D2sound+0x6D80 (ordinal 10028): the SetPan level in hundredths of a decibel for a pan byte.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_sound_pan_level(int32_t pan) {
    if (pan > 0x80) return -ftolTrunc(D2Sound_8830(0.0f, 127.0f, (float)(0xFF - pan)));
    if (pan < 0x80) return ftolTrunc(D2Sound_8830(0.0f, 127.0f, (float)pan));
    return 0;
}
