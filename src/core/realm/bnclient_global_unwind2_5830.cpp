// src/core/realm/bnclient_global_unwind2_5830.cpp -- alpha-lane row, implementation + marker only.
// Transcribed from build-wasm/ghidra-bnclient/decomp/0x5830.c.
#include <cstdint>
#include <emscripten.h>

// RtlUnwind(target_frame, target_ip, exception_record, return_value): the kernel32 import (0x7FBE thunk).
typedef void (*BnRtlUnwindFn)(uint32_t target_frame, uint32_t target_ip, uint32_t record, uint32_t retval);

// 1.13c BNClient+0x05830 (32 B): MSVC CRT __global_unwind2(frame) = RtlUnwind(frame, <own return label
// 0x6FF25848>, NULL, NULL). wasm has no SEH: the unwind is the explicit seam, the label is kept as data.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_5830(uint32_t frame, BnRtlUnwindFn rtl_unwind) {
    rtl_unwind(frame, 0x6ff25848u, 0, 0);
}
