// 1.13c D2Client+0x61780 (188 B, ret 8): EBX = unit, stack (kind, tree_root). node = 5F6B0(); x = Ordinal_10651(unit)/10 + 1; y = Ordinal_11142(unit)/10 - 3
// (C division). All three must fit int16 or retail aborts (Ordinal_10265/10024, _exit: not ported). Node +4 = kind, +6 = x, +8 = y; then 61320(ecx=node, edx=tree_root).
#include <emscripten.h>
#include <cstdint>
typedef uint32_t u32;
struct D2c61780Env {
    u32 (*f5f6b0)(void);
    int32_t (*ord10651)(u32 unit);
    int32_t (*ord11142)(u32 unit);
    u32 (*f61320)(u32 node, u32 tree_root);
};
extern "C" EMSCRIPTEN_KEEPALIVE
u32 d2c_61780(u32 unit, int32_t kind, u32 tree_root, const D2c61780Env* e) {
    u32 node = e->f5f6b0();
    int32_t x = e->ord10651(unit) / 10 + 1;
    int32_t y = e->ord11142(unit) / 10 - 3;
    if (x < -0x8000 || x > 0x7fff || y < -0x8000 || y > 0x7fff || kind < -0x8000 || kind > 0x7fff) return 0xFFFFFFFFu;   // retail: fatal exit
    *(int16_t*)(uintptr_t)(node + 4) = (int16_t)kind;
    *(int16_t*)(uintptr_t)(node + 6) = (int16_t)x;
    *(int16_t*)(uintptr_t)(node + 8) = (int16_t)y;
    return e->f61320(node, tree_root);
}
