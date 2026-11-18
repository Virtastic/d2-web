// 1.13c D2Client+0x4FF10: is the mouse inside the 22x34 button rect left of screen centre. Globals: w=0xDBC48 h=0xDBC4C my=0x11B824 mx=0x11B828.
#include <cstdint>
// 1.13c D2Client+0x4FF10
extern "C" uint32_t d2_client_4ff10(int32_t w, int32_t h, int32_t mx, int32_t my) {
    return (w / 2 - 0xc2 < mx && mx < w / 2 - 0xa0 && h - 0x2a < my && my < h - 8) ? 1 : 0;
}
