#include <cstdint>
#include <emscripten.h>

// MSVC 2003 Release CRT leaves of D2Gfx.dll that touch the error-reporting and SEH state. The three
// retail globals live in the module and are seams here (D2Gfx+0x10DF4 __error_mode, +0x10DF8 __app_type,
// +0x1106C _pfnmsg); the wasm host has no FS:[0] exception list, so it is passed in.

// 1.13c D2Gfx+0x03784 (57 B, __FF_MSGBANNER). When __error_mode is 1, or is 0 and __app_type is 1,
// write runtime message 0xFC through __NMSG_WRITE (0x360C), call the installed banner hook _pfnmsg if
// non-null, then write 0xFF. Void; the globals and both callees are parameters here.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2gfx_ff_msgbanner(uint32_t error_mode, uint32_t app_type,
                                             void (*nmsg_write)(int32_t),
                                             void (*banner_hook)(void)) {
    if (error_mode == 1 || (error_mode == 0 && app_type == 1)) {
        nmsg_write(0xfc);
        if (banner_hook != nullptr) banner_hook();
        nmsg_write(0xff);
    }
}

// 1.13c D2Gfx+0x038E0 (32 B, __global_unwind2). RtlUnwind(frame, 0x6FA838F8, NULL, NULL): the target is
// the CRT's own resume code at RVA 0x38F8 (retail address kept as its RVA, as in the sibling CRT ports);
// the import thunk 0x781E and the unwind itself are a host seam. Never returns here on wasm.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2gfx_global_unwind2(void* frame,
                                               void (*rtl_unwind)(void*, void*, void*, void*)) {
    rtl_unwind(frame, (void*)(uintptr_t)0x38F8, nullptr, nullptr);
}

// 1.13c D2Gfx+0x0398A (35 B, __abnormal_termination). Answer 1 only when the current exception record's
// resume label (record+4) is the CRT's LAB_6FA83900 (RVA 0x3900) and its saved frame word (record+8)
// equals the handler word at [record+0xC][0xC]; otherwise 0. Reads FS:[0], which wasm lacks, so the list
// head is the argument.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2gfx_abnormal_termination(const uint8_t* exception_list) {
    uint32_t resume = *(const uint32_t*)(exception_list + 4);          // retail 32-bit addresses
    uint32_t handler_word = *(const uint32_t*)(exception_list + 0xc);
    if (resume == 0x3900u &&
        *(const int32_t*)(exception_list + 8) ==
            *(const int32_t*)(uintptr_t)(handler_word + 0xc)) {
        return 1;
    }
    return 0;
}
