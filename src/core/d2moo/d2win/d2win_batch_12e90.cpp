// src/core/d2win/d2win_batch_12e90.cpp -- five 1.13c D2Win.dll rows transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2win/decomp/0x<RVA>.c) and checked instruction by
// instruction against the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base
// 0x6F8E0000). Alpha batch lane (OBJECTIVE v8.3): implementation + marker only, no spec, no vector.
//
// 0x12E90 draws one filled, word-wrapped text line anchored to the renderer's two-value limit:
// 0x11FB0 gives the widest line width, 0x11E80 the block height, D2Gfx #10080 the two limits
// (horizontal first, vertical second), D2Gfx #10014 the fill and 0x12940 the centered text.
// 0x11FB0 and 0x12940 are the rows owned by b91-d2win and stay seams here.
//
// Calling convention recovered from the disassembly:
//   0x12E90: text in ECX, limit in EDX; anchor, fill color, fill gamma, palette shift on the
//            stack, first anchor, ret 0x10.
//   0x17AD0: stdcall(control, value), ret 8.
//   0xED40:  destination in ECX, rep movsd of 0xC40 dwords from 0x6F932A38.
//   0x11E60: no arguments; reads the active font through 0x6F901518.
// Prior art: D2MOO 1.10f reference/D2MOO/source/D2Win (font record stride 0x14, pixel height at
// +0xA; no function bodies). The sibling row 0x12DE0 on b91-d2win reads the same state.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

static inline uint8_t rd8(const void* p, uint32_t o) { uint8_t v; std::memcpy(&v, (const uint8_t*)p + o, 1); return v; }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

struct D2WinBatch12E90Env {
    // 0x6F901518: font pointer table, stride 0x14; entry -> font struct, pixel height at +0xA.
    const uint8_t* const* fontTable;
    // 0x6F8FFD34: the active font index.
    uint32_t fontIndex;
    // 0x11FB0 (b91-d2win): widest line of the text, in pixels.
    int32_t (*maxLineWidth)(const uint16_t* text);
    // 0x11E80: total block height, one line per L'\n' plus one.
    int32_t (*textBlockHeight)(const uint16_t* text);
    // D2Gfx #10080 (interface slot +0x50): two out values, the horizontal limit first,
    // the vertical limit second.
    void (*getDimensions)(int32_t* horizontalLimit, int32_t* verticalLimit);
    // D2Gfx #10014 (D2FillArea): (left, top, right, bottom, color, gamma).
    void (*fillArea)(int32_t left, int32_t top, int32_t right, int32_t bottom,
                     int32_t color, int32_t gamma);
    // 0x12940 (b91-d2win): x in EAX; text, y, centerWidth, paletteShift, centered on the stack,
    // ret 0x14.
    void (*drawTextStrip)(int32_t x, const uint16_t* text, int32_t y, int32_t centerWidth,
                          int32_t paletteShift, int32_t centered);
    // D2CMP #10104.
    void (*d2cmp10104)(int32_t a0);
    // D2Gfx #10026 (D2ShowWindow).
    void (*d2gfx10026)(void);
};

// Retail module state, one symbol per DLL address so a probe can seed and read it.
extern "C" {
uint32_t d2win_g_6f9014c0;                        // 0x17C50
uint32_t d2win_g_6f901490;                        // 0x17C50
uint32_t d2win_g_6f932a38[0xC40];                 // 0xED40 source block (.data, seeded elsewhere)
}

static int32_t font_pixel_height(const D2WinBatch12E90Env* env) {
    return rd8(env->fontTable[env->fontIndex], 0xA);
}

// 1.13c D2Win+0x12E90 -- a filled, horizontally centered one-shot line drawn against the
// renderer's two limits. The width is the widest line plus 8; x is the limit pulled in until the
// text fits (never below zero); the bottom edge is the vertical limit less 31, pulled up to the
// larger of the anchor+2 and the block height. The fill spans the block height above the bottom
// edge; the text is drawn at that edge less three tenths of the font pixel height, centered.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_12E90(const uint16_t* text, int32_t limit, int32_t anchor, int32_t fillColor,
                     int32_t fillGamma, int32_t paletteShift, const D2WinBatch12E90Env* env) {
    const int32_t width = env->maxLineWidth(text) + 8;
    const int32_t height = env->textBlockHeight(text);

    int32_t horizontalLimit = 0;
    int32_t verticalLimit = 0;
    env->getDimensions(&horizontalLimit, &verticalLimit);

    int32_t x = limit > 0 ? limit : 0;
    if (x > horizontalLimit - width) x = horizontalLimit - width;

    const int32_t anchored = anchor + 2;
    const int32_t candidate = anchored > height ? anchored : height;
    int32_t bottom = verticalLimit - 31;
    if (candidate < bottom) bottom = candidate;

    env->fillArea(x, bottom - height, x + width, bottom, fillColor, fillGamma);
    env->drawTextStrip(x, text, bottom - (3 * font_pixel_height(env)) / 10, width,
                       paletteShift, 1);
}

// 1.13c D2Win+0x17C50 -- zero two module words, hand a 1 to D2CMP #10104, show the D2Gfx window,
// then report 1.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2win_sub_17C50(const D2WinBatch12E90Env* env) {
    d2win_g_6f9014c0 = 0;
    d2win_g_6f901490 = 0;
    env->d2cmp10104(1);
    env->d2gfx10026();
    return 1;
}

// 1.13c D2Win+0xED40 -- copy 0xC40 dwords (0x3100 bytes) of the DLL's .data block at 0x6F932A38
// to the caller's buffer.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_ED40(uint32_t* dst) {
    std::memcpy(dst, d2win_g_6f932a38, 0xC40 * sizeof(uint32_t));
}

// 1.13c D2Win+0x17AD0 -- store the second word at control+4 and report 1.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2win_sub_17AD0(void* control, uint32_t value) {
    wr32(control, 4, value);
    return 1;
}

// 1.13c D2Win+0x11E60 -- the pixel height byte (+0xA) of the active font record, zero-extended.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2win_sub_11E60(const D2WinBatch12E90Env* env) {
    return (uint32_t)font_pixel_height(env);
}
