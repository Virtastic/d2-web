// src/core/d2game/d2game_batch_crt_tiny_6730.cpp -- six 1.13c D2Game.dll CRT leaves
// transcribed from the cached decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c), in RVA order:
// ___heap_select (0x6730), __global_unwind2 (0x77F0), __NLG_Notify1 (0x78BD), sub_78DE (0x78DE),
// ___initmbctable (0x7DB5), and _abort (0x96F8). The five batch helpers are NAMED; sub_78DE has its
// own mapped spec and a retail recording vector reservation.
//
// Prior art: the byte-identical copies in src/core/realm/mcp_crt_batch_b52.cpp (0x233D heap_select),
// src/core/realm/mcp_crt_batch_b58.cpp (0x34DE initmbctable), src/core/realm/mcp_crt_batch_b56.cpp
// (0x28FC global_unwind2), src/core/d2client/d2client_batch_crt_unwind_a4ac.cpp (0x911C
// global_unwind2), src/core/d2client/d2client_batch_1e70.cpp (0x91E9 NLG_Notify1),
// src/core/d2gfx/d2gfx_batch_b45.cpp (0x39B6 NLG_Notify) and src/core/realm/bnclient_batch_5906.cpp
// (0x5906 NLG_Notify). Host model follows them: the platform/version words are retail .data, so
// ___heap_select takes the caller's values; kernel and not-yet-recovered CRT callees are seams after
// the retail arguments in retail order; the __NLG notifier globals are host statics at their retail
// RVA. __NMSG_WRITE (0x69DC) is ours (src/core/d2game/d2game_batch_5a8d.cpp) and is called through its
// extern "C" definition; __setmbcp (0x7C65) and _raise (0x9EDD) are still UNMAPPED and arrive as seams.
#include <cstdint>
#include <unistd.h>
#include <emscripten.h>

extern "C" void d2_fn_d2game_69dc(int32_t code, const uint32_t* table);   // 0x69DC __NMSG_WRITE

// 1.13c D2Game+0x078DE: map a Windows code page to its CRT locale identifier.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_78de(uint32_t codepage) {
    switch (codepage) {
        case 932: return 0x0411;  // Japanese
        case 936: return 0x0804;  // Simplified Chinese
        case 949: return 0x0412;  // Korean
        case 950: return 0x0404;  // Traditional Chinese
        default: return 0;
    }
}

// Seams for the callees the decompiles name but src/ does not own yet. The 19-entry {code, text}
// table is retail data at 0x6FD1F7E8 and stays out of git (as in 0x5A8D); 0xA26E is the verified
// KERNEL32!RtlUnwind import thunk.
struct D2GameBatchTinyEnv {
    int32_t (*setmbcp)(int32_t);                                                 // 0x7C65 __setmbcp
    int32_t (*raise_)(int32_t);                                                  // 0x9EDD _raise
    void    (*rtlUnwind)(void* frame, void* target, void* record, void* value);  // 0xA26E RtlUnwind
    const uint32_t* nmsgTable;                                                   // 0x6FD1F7E8
};

// 1.13c D2Game+0x06730 (___heap_select): on NT5 and later -- the OS platform word at 0x6FD2FD24 == 2
// (VER_PLATFORM_WIN32_NT) and the major version at 0x6FD2FD30 > 4 -- the CRT uses the OS heap (1);
// anything else selects the small-block heap (3).
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_6730(uint32_t osPlatform, uint32_t winMajor) {
    return (osPlatform == 2 && winMajor > 4) ? 1 : 3;
}

// 1.13c D2Game+0x077F0 (__global_unwind2): RtlUnwind(frame, 0x6FC27808, NULL, NULL). The target is
// the instruction after the call (RVA 0x7808, stored as its retail VA); unwinding itself is a host
// operation and the KERNEL32 import thunk (0xA26E) is the seam.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_77f0(void* frame, const D2GameBatchTinyEnv* env) {
    env->rtlUnwind(frame, (void*)(uintptr_t)0x6FC27808u, nullptr, nullptr);
}

// +0x6FD1F9A0/A4/A8: the __NLG notifier's saved EAX, ECX argument and EBP.
static uint32_t g_nlg_eax, g_nlg_param, g_nlg_ebp;

// 1.13c D2Game+0x078BD (__NLG_Notify1): the fastcall sibling of __NLG_Notify -- store the ECX
// argument at +0x6FD1F9A4, EAX at +0x6FD1F9A0 and EBP at +0x6FD1F9A8. ECX/EAX/EBP cannot ride a C
// signature, so they are the seams after the retail argument.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_78bd(uint32_t ecx, uint32_t eax, uint32_t ebp) {
    g_nlg_param = ecx;
    g_nlg_eax = eax;
    g_nlg_ebp = ebp;
}

// +0x6FD3B358: the multibyte-table initialised flag.
static uint32_t g_mbctableDone;

// 1.13c D2Game+0x07DB5 (___initmbctable): if the flag is clear, run __setmbcp(-3) (the host default
// codepage setup, 0x7C65) and set it; always return 0.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_7db5(const D2GameBatchTinyEnv* env) {
    if (g_mbctableDone == 0) {
        env->setmbcp(-3);
        g_mbctableDone = 1;
    }
    return 0;
}

// 1.13c D2Game+0x096F8 (_abort): report message 10 (__RT_ABORT) through __NMSG_WRITE, raise SIGABRT
// (0x16), then __exit(3). __exit = doexit(3, quick=1, retcaller=0), which terminates the process; on
// the wasm host that is _exit(3).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_96f8(const D2GameBatchTinyEnv* env) {
    d2_fn_d2game_69dc(10, env->nmsgTable);
    env->raise_(0x16);
    _exit(3);
}
