// 1.13c Storm+0x07B1C -- CRT _write_multi_char: emit ch count times via write_char until the stream error flag is -1.
#include <cstdint>
void Storm_write_multi_char(int32_t* state, int ch, int count, void (*write_char)(int32_t*, int)) {
    while (count-- > 0) {
        write_char(state, ch);
        if (*state == -1) return;
    }
}
