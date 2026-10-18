// src/core/d2win/d2win_batch_12de0.cpp -- four 1.13c D2Win.dll rows transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2win/decomp/0x<RVA>.c) and checked instruction by
// instruction against the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base
// 0x6F8E0000). Alpha batch lane (OBJECTIVE v8.3): implementation + marker only, no spec, no vector.
//
// 0x12DE0 (draw a centered, filled tooltip-style line) calls the other three: 0x11FB0 (max line
// width) first, then D2Gfx Ordinal 10080 (GetDimensions), Ordinal 10014 (DrawRectangle), and
// Ordinal 10019 (CelDrawColor) through 0x12940 (vertical-strip text draw), which in turn asks
// 0x11F00 for the width of the line that follows each newline.
//
// Calling convention is retail's __fastcall, recovered from the disassembly:
//   0x11F00 / 0x11FB0: text in EAX.
//   0x12940: x in EAX; text, y, centerWidth, paletteShift, centered on the stack (ret 0x14).
//   0x12DE0: text in ECX, x in EDX; y, fill color, fill gamma, palette shift on the stack (ret 0x10).
// State and callees the decompile still visits by absolute address are seams on the Env struct,
// passed last, so the retail argument order is preserved. Prior art: D2MOO 1.10f
// reference/D2MOO/source/D2Win/src/D2WinFont.cpp (signatures and the L"c" color-code constant only;
// no bodies).
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

static inline uint8_t  rd8(const void* p, uint32_t o)  { uint8_t  v; std::memcpy(&v, (const uint8_t*)p + o, 1); return v; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

[[noreturn]] static void d2win_fatal(uint32_t) { _exit(-1); }

// D2Lang Unicode::strlen: UTF-16 code units up to the terminal zero.
static int32_t u16_len(const uint16_t* s) { int32_t n = 0; while (s[n] != 0) ++n; return n; }

struct D2WinBatch12DE0Env {
    // 0x6F8FFD34, the current font index (initialised to 1 in .data).
    uint32_t fontIndex;
    // 0x6F8FFD38, line spacing (initialised to 16 in .data and rewritten at runtime).
    int32_t lineSpacing;
    // 0x6F901518: font pointer table, stride 0x14; entry -> font struct, pixel height at +0xA.
    const uint8_t* const* fontTable;
    // 0x6F9AA66C / 0x6F9AA670: both hold L"c", the color-code letter. The width functions compare
    // it case-insensitively (D2Lang strnicmp), the draw loop exactly (strncmp).
    const uint16_t* colorCode;
    // 0x6F901508: glyph lookup, char in ECX, returns a struct with width at +3 and frame word at +8.
    const uint8_t* (*glyph)(uint16_t ch);
    // 0x6F9014F0: its first dword is copied into the draw block at offset 0x34.
    const uint32_t* drawParam;
    // D2Gfx Ordinal 10080 (interface slot +0x50).
    void (*getDimensions)(int32_t* pWidth, int32_t* pHeight);
    // D2Gfx Ordinal 10014 (interface slot +0xB8): (left, top, right, bottom, color, gamma).
    void (*fillArea)(int32_t left, int32_t top, int32_t right, int32_t bottom,
                     int32_t color, int32_t gamma);
    // D2Gfx Ordinal 10019 (interface slot +0x88): 0x48-byte block, x, y, gamma, mode 5, palette shift.
    void (*celDrawColor)(const void* block, int32_t x, int32_t y, uint32_t gamma,
                         int32_t mode, int32_t paletteShift);
};

static int32_t font_height(const D2WinBatch12DE0Env* env) {
    const uint8_t* font = env->fontTable[env->fontIndex];
    return rd8(font, 0xA);
}

static int32_t glyph_width(const D2WinBatch12DE0Env* env, uint16_t ch) {
    return rd8(env->glyph(ch), 3);
}

// D2Lang strnicmp(two UTF-16 strings, one char), ASCII case folding -- all this ever compares is
// the color-code letter. A NUL or any other char compares unequal.
static bool ci_is_color_code(const uint16_t* colorCode, uint16_t ch) {
    uint16_t want = colorCode[0];
    if (ch >= 'A' && ch <= 'Z') ch = (uint16_t)(ch + ('a' - 'A'));
    if (want >= 'A' && want <= 'Z') want = (uint16_t)(want + ('a' - 'A'));
    return ch == want && ch != 0;
}

// 1.13c D2Win+0x11F00 -- width of the first line of a Unicode string, stopping at L'\n' or NUL.
// A 0xFF byte introduces a three-unit color code; only when its letter is (case-insensitively)
// L'c' does the code contribute the width of the letter itself. Every 0xFF sequence consumes three
// units whether or not the letter matched, exactly as the retail loop does. NULL text is fatal
// (error id 0x23c).
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2win_sub_11f00(const uint16_t* text, const D2WinBatch12DE0Env* env) {
    if (!text) d2win_fatal(0x23c);
    int32_t width = 0;
    int32_t n = u16_len(text);
    int32_t i = 0;
    if (n > 0) {
        do {
            uint16_t ch = text[i];
            if (ch == 0x0A) return width;
            if (ch < 0x100 && ch == 0xFF) {
                if (ci_is_color_code(env->colorCode, text[i + 1]))
                    width += glyph_width(env, env->colorCode[0]);
                i += 3;
            } else {
                width += glyph_width(env, ch);
                ++i;
            }
        } while (i < n);
    }
    return width;
}

// 1.13c D2Win+0x11FB0 -- the widest line of a Unicode string. Same color-code rule as 0x11F00, but
// L'\n' closes a line and the widest is kept; a NUL inside the scanned range also ends the walk.
// NULL text is fatal (error id 0x20d).
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2win_sub_11fb0(const uint16_t* text, const D2WinBatch12DE0Env* env) {
    if (!text) d2win_fatal(0x20d);
    int32_t maxWidth = 0;
    int32_t lineWidth = 0;
    int32_t n = u16_len(text);
    int32_t i = 0;
    if (n > 0) {
        do {
            uint16_t ch = text[i];
            if (ch == 0) break;
            if (ch == 0x0A) {
                if (lineWidth > maxWidth) maxWidth = lineWidth;
                lineWidth = 0;
                ++i;
            } else if (ch < 0x100 && ch == 0xFF) {
                if (ci_is_color_code(env->colorCode, text[i + 1]))
                    lineWidth += glyph_width(env, env->colorCode[0]);
                i += 3;                 // three units per color code, matched or not
            } else {
                lineWidth += glyph_width(env, ch);
                ++i;
            }
        } while (i < n);
        if (lineWidth > maxWidth) maxWidth = lineWidth;
    }
    return maxWidth;
}

// The retail line advance: trunc((fontHeight * lineSpacing) * 0x99999999 / 2^34), i.e. the signed
// product divided by -10 and truncated toward zero. Reproduced through the exact 32x32->64 imul and
// the sar/sign-fold sequence so a negative spacing advances the way retail does.
static int32_t line_advance(const D2WinBatch12DE0Env* env) {
    int32_t product = font_height(env) * env->lineSpacing;
    int64_t wide = (int64_t)product * (int64_t)(int32_t)0x99999999;
    int32_t edx = (int32_t)(wide >> 32);
    edx >>= 2;
    uint32_t fold = (uint32_t)edx >> 31;
    return (int32_t)(fold + (uint32_t)edx);
}

// 1.13c D2Win+0x12940 -- draw a Unicode string glyph by glyph through D2Gfx Ordinal 10019 with the
// renderer's mode 5, gamma 0xFFFFFFFF and the caller's palette shift. The 0x48-byte draw block is
// zeroed once; offset 0x34 takes the dword at 0x6F9014F0 and offset 0 is the glyph frame word. A
// 0xFF followed by an exact L'c' and one more unit is a color code: the following unit minus '0'
// (kept only below 13, signed) becomes the palette shift and draws nothing; a 0xFF not followed by
// L'c' is drawn as an ordinary glyph, and the loop then steps back to it. L'\n' advances y by the
// retail line advance and, when centered, re-centers using the width of the line that follows it
// against the same x that the first-line centering produced.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_12940(int32_t x, const uint16_t* text, int32_t y, int32_t centerWidth,
                     int32_t paletteShift, int32_t centered, const D2WinBatch12DE0Env* env) {
    int32_t n = u16_len(text);
    int32_t origX = x;
    if (centered) origX = x + ((centerWidth - d2win_sub_11f00(text, env)) >> 1);

    uint8_t block[0x48];
    std::memset(block, 0, sizeof block);
    wr32(block, 0x34, *env->drawParam);

    int32_t i = 0;
    while (i < n) {
        uint16_t ch = text[i];
        if (ch < 0x100 && ch == 0xFF) {
            if (text[i + 1] == env->colorCode[0]) {
                if (i + 2 < n) {
                    int32_t shift = (int32_t)(uint16_t)text[i + 2] - 0x30;
                    if (shift >= 0xD) shift = 0;    // cmp ecx,0xD / jl: negative keeps
                    paletteShift = shift;
                    i += 3;
                } else {
                    paletteShift = 0;
                    i += 2;
                }
            } else {
                const uint8_t* glyph = env->glyph(ch);
                wr32(block, 0, rd16(glyph, 8));
                env->celDrawColor(block, x, y, 0xFFFFFFFFu, 5, paletteShift);
                x += rd8(glyph, 3);
                ++i;
            }
        } else if (ch == 0x0A) {
            x = origX;
            if (centered)
                x = origX + ((centerWidth - d2win_sub_11f00(text + i + 1, env)) >> 1);
            y += line_advance(env);
            ++i;
        } else {
            const uint8_t* glyph = env->glyph(ch);
            wr32(block, 0, rd16(glyph, 8));
            env->celDrawColor(block, x, y, 0xFFFFFFFFu, 5, paletteShift);
            x += rd8(glyph, 3);
            ++i;
        }
    }
}

// 1.13c D2Win+0x12DE0 -- a filled, horizontally centered one-shot line (the edit-box/tooltip
// caption path). Width is the widest line. x is pulled left so the text fits the screen; y is
// y0+2, clamped to screenH-31, then doubled if the text would start above the top (retail's own
// rule). The fill spans fontHeight above y; the text itself is drawn at y-2, centered on the
// measured width.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_12de0(int32_t x, const uint16_t* text, int32_t y0, int32_t fillColor,
                     int32_t fillGamma, int32_t paletteShift, const D2WinBatch12DE0Env* env) {
    int32_t width = d2win_sub_11fb0(text, env);
    int32_t height = font_height(env);
    int32_t screenW = 0, screenH = 0;
    env->getDimensions(&screenW, &screenH);
    if (x + width > screenW) x = screenW - width;
    int32_t y = y0 + 2;
    if (y > screenH - 31) y = screenH - 31;
    if (y - height < 0) y = y * 2;
    env->fillArea(x, y - height, x + width, y, fillColor, fillGamma);
    d2win_sub_12940(x, text, y - 2, width, paletteShift, 1, env);
}
