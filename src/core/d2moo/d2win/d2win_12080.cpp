// 1.13c D2Win+0x12080 -- GetUnicodeTextNDrawWidth
// ECX = text (UTF-16, NUL-terminated), EDX = n (signed). Returns the sum of byte +3 of the glyph record the getter at
// DAT_6F901508 returns for each character, for the first n characters or up to the NUL; a line feed (0x0A) adds nothing.
// The getter is a seam: its record pointer is the only thing it contributes (src/core/fn/d2win_12080.cpp mirrors it).
#include <cstdint>
#include <emscripten.h>

struct D2Win12080Env {
    const uint8_t* (*glyph)(uint32_t ch);   // DAT_6F901508 (ECX = character); the record's byte +3 is the advance
};

extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2win_12080(const uint16_t* text, int32_t n, const D2Win12080Env* env) {
    int32_t sum = 0;
    for (int32_t i = 0; text[0] != 0 && i < n; ++i, ++text) {
        if (text[0] != 10) sum += env->glyph(text[0])[3];
    }
    return sum;
}
