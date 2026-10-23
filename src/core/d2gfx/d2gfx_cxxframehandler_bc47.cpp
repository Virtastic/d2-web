#include <cstdint>

// 1.13c D2Gfx+0xBC47: ___CxxFrameHandler(rec, reg, ctx, disp) forwards to
// ___InternalCxxFrameHandler(rec, reg, ctx, disp, funcInfo = EAX, 0, nullptr, 0).
typedef int (*InternalCxxFrameHandler)(void*, void*, void*, void*, const void*, int, void*, char);
extern "C" int d2gfx_cxxframehandler_bc47(void* rec, void* reg, void* ctx, void* disp, const void* funcInfo, InternalCxxFrameHandler internal) {
    return internal(rec, reg, ctx, disp, funcInfo, 0, nullptr, 0);
}
