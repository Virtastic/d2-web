// src/core/d2client/d2client_batch_2c7e0.cpp -- signed number formatter.
// 1.13c D2Client+0x2C7E0
// the signed number formatter called while building stat descriptions (decompile 0x2C7E0.c).
// Prior art used: D2MOO 1.10f has no D2Client counterpart; the 1.13c decompile at this RVA is
// authoritative, with the narrow-to-Unicode conversion delegated through the D2Lang IAT.
#include <cstdint>
#include <emscripten.h>

struct D2Client2C7E0Env {
    void (*toUnicode)(uint16_t* destination, const char* source, int32_t count); // D2Lang IAT +0xCEAC4
};

// `thisValue` is the retail ECX value. The numeric input arrives in EAX; `destination` is
// the stack argument. Retail uses `%d` when ECX is null or the value is negative, and `+%d`
// otherwise, then converts at most 31 narrow characters to Unicode.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2client_2c7e0(
    uint32_t thisValue, int32_t value, uint16_t* destination,
    const D2Client2C7E0Env* env) {
    char text[32];
    uint32_t magnitude = value < 0 ? 0u - static_cast<uint32_t>(value)
                                   : static_cast<uint32_t>(value);
    char reversed[10];
    int32_t digits = 0;
    do {
        reversed[digits++] = static_cast<char>('0' + magnitude % 10u);
        magnitude /= 10u;
    } while (magnitude != 0);

    int32_t length = 0;
    if (value < 0) text[length++] = '-';
    else if (thisValue != 0) text[length++] = '+';
    while (digits != 0) text[length++] = reversed[--digits];
    text[length] = '\0';

    env->toUnicode(destination, text, 0x20);
    return 1;
}
