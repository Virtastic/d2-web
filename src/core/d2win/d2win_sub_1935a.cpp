// 1.13c D2Win+0x1935A (CRT _atodbl): parse string to double. Runs the string->80-bit-intermediate parser
// (0x19872, ret in a 12-byte LDBL12) and narrows it to a double (0x192FF __ld12tod) into *out.
#include <cstdint>
#include <emscripten.h>

// ponytail: callees are separate rows (19872 UNMAPPED, 192FF DEAD CRT); weak so this links until they land.
extern "C" __attribute__((weak)) void d2win_sub_19872(void* ld12, int32_t* consumed, const uint8_t* s, int, int, int, int);
extern "C" __attribute__((weak)) void d2win_ld12tod(const void* ld12, void* out);

extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_1935a(void* out, const uint8_t* str) {
    uint8_t ld12[12];
    int32_t consumed;
    d2win_sub_19872(ld12, &consumed, str, 0, 0, 0, 0);
    d2win_ld12tod(ld12, out);
}
