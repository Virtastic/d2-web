// src/core/d2client/d2client_batch_crt_unwind_a4ac.cpp -- four UNMAPPED 1.13c D2Client.dll CRT leaves
// reimplemented from the decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane).
// Image base 0x6FAB0000; every RVA below is retail's. Kernel32 and not-yet-recovered callees are host
// seams AFTER the retail arguments, which keep their order (the d2client_batch_1e70.cpp pattern).
#include <cstdint>
#include <emscripten.h>

// 0x0A71B __unlock_fhandle is still UNMAPPED: the seam stands in for it.
struct D2ClientCrtUnwindEnv {
    void (*rtl_unwind)(void* frame, void* target_ip, void* record, void* value); // 0xBE1E RtlUnwind
    void (*unlock_fhandle)(int32_t fhandle);                                     // 0x0A71B
};

// 0x47AB _unlock(idx) -- already implemented in d2client_batch_3664.cpp.
extern "C" void d2_client_47ab(uint32_t idx);

// 1.13c D2Client+0x0911C (32 B, __global_unwind2): RtlUnwind(frame, 0x6FAB9134, NULL, NULL), the
// target IP being the instruction after the call (RVA 0x9134). Unwinding is a host operation.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_911c(void* frame, const D2ClientCrtUnwindEnv* env) {
    env->rtl_unwind(frame, (void*)(uintptr_t)0x6FAB9134u, nullptr, nullptr);
}

// 1.13c D2Client+0x0A4AC (16 B): the SEH finally fragment of __sopen (0x0A467). Its operands are two
// slots of the caller's frame: EBP-0x1C is the "fhandle held" flag, EBP-0x20 the fhandle. If the flag
// is set, __unlock_fhandle(fhandle); otherwise nothing (the slot is not even read).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_a4ac(const uint32_t* ebp, const D2ClientCrtUnwindEnv* env) {
    if ((int32_t)ebp[-7] != 0)               // *(int *)(EBP - 0x1C)
        env->unlock_fhandle((int32_t)ebp[-8]); // *(int *)(EBP - 0x20)
}

// 1.13c D2Client+0x0A816 (9 B): tail-call FUN_6fab47ab(10) -- release CRT lock 10.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_a816(void) {
    d2_client_47ab(10);
}

// 1.13c D2Client+0x0A8B0 (9 B): tail-call FUN_6fab47ab(0xB) -- release CRT lock 11.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_a8b0(void) {
    d2_client_47ab(0xB);
}
