// src/core/d2win/d2win_sub_17800.cpp -- 1.13c D2Win+0x17800, resolved against the retail disassembly (the Ghidra decompile read the
// registers wrongly: there is no ESI input). Clamp the cursor to the screen from a control's x/y (u16 at +0xc/+0xe); callees are env seams.
#include <cstdint>

struct d2win_env17800 {
    void (*gfx_10029)(int32_t one);                 // D2gfx #10029 (thunk 0x77E6), stdcall(1)
    void (*gfx_10080)(int32_t* w, int32_t* h);      // D2gfx #10080 (thunk 0x7804): screen width, height
    int  (*win_10051)();                            // thunk 0x77BC, nonzero = cursor enabled
    void (*set_cursor_pos)(uint32_t x, uint32_t y); // user32 SetCursorPos (IAT slot 0x1B354)
    uint32_t* g_x;                                  // DAT_6f901488
    uint32_t* g_y;                                  // DAT_6f90148c
};

// 1.13c D2Win+0x17800 (stdcall(ctl), ret 4). x is clamped to width-1; y to height-1; SetCursorPos runs when y was clamped or x was.
extern "C" void d2win_sub_17800(const d2win_env17800* e, const uint8_t* ctl) {
    int32_t x = *(const uint16_t*)(ctl + 0xc), y = *(const uint16_t*)(ctl + 0xe);
    int32_t w = 0, h = 0;
    e->gfx_10029(1);
    e->gfx_10080(&w, &h);
    w -= 1; h -= 1;
    if (e->win_10051()) {
        bool xc = false;
        if (x > w) { x = w; xc = true; }
        if (y > h) { y = h; e->set_cursor_pos((uint32_t)x, (uint32_t)y); }
        else if (xc) e->set_cursor_pos((uint32_t)x, (uint32_t)y);
    }
    *e->g_y = (uint32_t)y;
    *e->g_x = (uint32_t)x;
}
