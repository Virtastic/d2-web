// 1.13c D2Win+0x15A10 -- redraw a control relative to the cursor: if the control record (*p) has
// type word 1, call sub_13DA0(p, mouseX - rec[0x10] - rec[3], rec[6] - rec[0x11] - rec[4] + mouseY).
// Null record (string 0x7E5) or other type (0x7E6) is fatal via d2win_exit.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

extern "C" void d2win_exit(uint32_t code);
extern "C" uint32_t d2win_sub_13da0(void* p, int32_t x, int32_t y);   // D2Win+0x13DA0
extern uint32_t d2win_g_6f901488, d2win_g_6f90148c;                   // cursor x, y

extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_15a10(void* p) {
    uint8_t* r = nullptr;
    if (p) std::memcpy(&r, p, sizeof r);
    int32_t w[0x12] = {};
    if (r) std::memcpy(w, r, sizeof w);
    if (r && w[0] == 1) {
        d2win_sub_13da0(p, (int32_t)d2win_g_6f901488 - w[0x10] - w[3],
                        w[6] - w[0x11] - w[4] + (int32_t)d2win_g_6f90148c);
        return;
    }
    d2win_exit((uint32_t)-1);
}
