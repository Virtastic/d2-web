// 1.13c D2Client+0x60AF0 (332 B): automap marker for a party unit. EDI = unit (may be 0), no stack args.
// room := C1F0([11BBFC]); a := C1DE(room); both owners (C1EA of the unit's [+0x24] and of a) must match;
// C2F8(&x,&y) rewrites the unit's [+0x28]/[+0x2C] in place; screen x = X/div - orgX + 8, y = Y/div - orgY - 8;
// inside the clip box: 12 D102 segments from the table at D2DE8 (offset pairs doubled), then if [F16BC]: D280(EAX=unit; 2, y, x) and 601A0(EBX=D280 result).
#include <cstdint>
typedef int32_t i32; typedef uint32_t u32;
struct D2c60af0Env {
    u32 (*c1f0)(u32 a);                 // Ordinal_10331, stdcall 1
    u32 (*c1de)(u32 a);                 // stdcall 1
    u32 (*c1ea)(u32 a);                 // stdcall 1, low byte compared
    void (*c2f8)(i32* x, i32* y);       // stdcall 2: x = &[unit+0x28] copy, y = &[unit+0x2C] copy
    void (*d102)(i32 x0, i32 y0, i32 x1, i32 y1, u32 col, u32 k);   // stdcall 6
    u32 (*d280)(u32 unit, i32 x, i32 y, u32 mode);                  // EAX = unit, stack x, y, 2 (ret 0xC)
    void (*f601a0)(u32 ebx);
};
static inline u32 r32(u32 a) { return *(const u32*)(uintptr_t)a; }
// 1.13c D2Client+0x60AF0
void d2c_60af0(u32 unit, u32 g_bbfc, i32 g_div, i32 g_ox, i32 g_oy, const i32* g_clip, uint8_t g_col, const i32* g_seg, u32 g_flag, const D2c60af0Env* e) {
    u32 a = e->c1de(e->c1f0(g_bbfc));
    uint8_t bl = (uint8_t)e->c1ea(unit ? r32(unit + 0x24) : 0);
    if ((uint8_t)e->c1ea(a) != bl) return;
    i32 px = unit ? (i32)r32(unit + 0x28) : 0, py = unit ? (i32)r32(unit + 0x2c) : 0;
    e->c2f8(&px, &py);
    i32 x = px / g_div - g_ox + 8, y = py / g_div - g_oy - 8;
    if (x < g_clip[0] || x > g_clip[2] || y < g_clip[1] || y > g_clip[3]) return;
    for (int i = 0; i < 12; i++)
        e->d102(x + g_seg[2 * i] * 2, y + g_seg[2 * i + 1] * 2, x + g_seg[2 * i + 2] * 2, y + g_seg[2 * i + 3] * 2, g_col, 0xff);
    if (g_flag) e->f601a0(e->d280(unit, x, y, 2));
}
