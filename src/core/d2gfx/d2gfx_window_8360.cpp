#include <cstdint>

// 1.13c D2Gfx+0x08360 (ordinal 10060): stdcall(res, force) -- change the resolution index. The null-table fatal (Fog 10265(0x1e3), 10024, _exit(-1)) is not modelled.
// *cur is +0x11260. Same index and !force -> 1. Else store it; in windowed mode (+0x1125C == 1) with a client area under 0x321x0x259, frame the new size:
// dim = D2Gfx+0x7FD0(res), *extra (+0x1D580) = frame height - client height (AdjustWindowRectEx style 0xCB0000, ex 0x40000), SetWindowPos(0xFFFFFFFE,0,0,cx,cy,0x16).
// Then slot +0x24 (its result is returned) and the contrast/gamma restore 0xAEE0. Win32 and the slots are parameters.
extern "C" uint32_t d2gfx_sub_8360(uint32_t res, uint32_t force, uint32_t* cur, uint32_t windowed, int32_t clientW, int32_t clientH, uint32_t* extra,
        void (*frame)(uint32_t res, int32_t* cx, int32_t* cy, int32_t* baseH), void (*setPos)(int32_t cx, int32_t cy),
        uint32_t (*slot24)(), void (*restore)()) {
    if (force == 0 && res == *cur) return 1;
    *cur = res;
    if (windowed == 1 && clientW < 0x321 && clientH < 0x259) {
        int32_t cx, cy, base; frame(res, &cx, &cy, &base);
        *extra = (uint32_t)(cy - base);
        setPos(cx, cy);
    }
    uint32_t r = slot24();
    restore();
    return r;
}
