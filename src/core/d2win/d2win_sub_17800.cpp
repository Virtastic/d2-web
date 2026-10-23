// src/core/d2win/d2win_sub_17800.cpp -- 1.13c D2Win+0x17800, transcribed from the cached Ghidra decompile.
// Clamp-and-set cursor from a control's x/y (u16 at +0xc/+0xe); callees are env seams.
#include <cstdint>

struct d2win_env17800 {
    void (*gfx_10029)();                            // D2gfx #10029 (thunk 0x77E6)
    void (*gfx_10080)(int32_t* a, int32_t* b);      // D2gfx #10080 (thunk 0x7804), out-params on stack
    int  (*win_10051)();                            // thunk 0x77BC, nonzero = cursor enabled
    void (*set_cursor_pos)(uint32_t x, uint32_t y); // user32 SetCursorPos
    uint32_t* g_x;                                  // DAT_6f901488
    uint32_t* g_y;                                  // DAT_6f90148c
};

// 1.13c D2Win+0x17800
extern "C" void d2win_sub_17800(const d2win_env17800* e, const uint8_t* ctl, uint32_t esi) {
    uint32_t x = *(const uint16_t*)(ctl + 0xc), y = *(const uint16_t*)(ctl + 0xe);
    int32_t o1 = 0, o2 = 0;
    e->gfx_10029();
    e->gfx_10080(&o1, &o2);
    uint32_t lim = esi - 1, Y = 0, X = x, ny = y;
    if (e->win_10051()) {
        if ((int32_t)lim < (int32_t)x) X = lim;
        if (y != 0 || (Y = y, (int32_t)lim < (int32_t)x)) { e->set_cursor_pos(X, Y); ny = Y; }
    }
    *e->g_y = ny;
    *e->g_x = X;
}
