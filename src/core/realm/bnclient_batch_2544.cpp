// src/core/realm/bnclient_batch_2544.cpp -- ten UNMAPPED 1.13c BNClient.dll alpha-lane rows, transcribed
// from the cached decompiles in build-wasm/ghidra-bnclient/decomp/0x<RVA>.c (image base 0x6FF20000).
// These are the MSVC 2003 (VC7.1) Release CRT leaves the linker pulled into the module. Host/module
// state the comments reads through globals (the heap handle and mode, the lock table, the thread data
// block and its locale, the security cookie, the written-count pointer) arrives as explicit seam
// arguments after the retail arguments, in retail order. Where Ghidra shows a register argument as
// `unaff_*`/`in_*` (this calling convention holds FILE* in ECX, the count pointer in EAX/ESI and the
// character in AL), that argument is made explicit in the same position's role.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

// The 1.13c FILE (32-bit CRT): ptr 0x0, cnt 0x4, base 0x8, flag 0xC (same layout as
// src/core/realm/bnclient_batch2_crt.cpp, which owns 0x26AD __flsbuf).
struct BnCrtStream {
    uint32_t ptr;
    int32_t  cnt;
    uint32_t base;
    uint32_t flag;
};
typedef int32_t (*BnFlsbufFn)(int32_t ch, BnCrtStream* f);
typedef void*   (*BnNhMallocFn)(size_t size, int32_t nh_flag);
typedef void*   (*BnGetPtdFn)(void);
typedef void    (*BnLockFn)(int32_t locknum);
typedef uint32_t* (*BnSbhFindBlockFn)(void* mem);
typedef void    (*BnSbhFreeBlockFn)(uint32_t* block, void* mem);
typedef int32_t (*BnHeapFreeFn)(void* heap, uint32_t flags, void* mem);
typedef void*   (*BnUpdatetlocinfoFn)(void);
typedef int32_t (*BnWctombMtFn)(void* ptlocinfo, char* mb, uint16_t wch);
typedef void    (*BnAmsgExitFn)(int32_t code);

// 1.13c BNClient+0x02544 (18 B, caller 0x2854 _output among others): _malloc. Forward the size and the
// module global DAT_6ff3e604 (_newmode) to 0x2518 __nh_malloc. No allocation happens here.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_fn_bnclient_2544(size_t size, int32_t newmode, BnNhMallocFn nh_malloc) {
    return nh_malloc(size, newmode);
}

// 1.13c BNClient+0x027C6 (51 B, callers 0x2854, 0x27F9, 0x281D): _write_char. When the stream is not a
// string stream (flag & 0x40) or has a base, decrement cnt first; a negative cnt goes through 0x26AD
// __flsbuf, otherwise store the byte at ptr and advance it. A -1 from __flsbuf stores -1 in the written
// counter (EAX/ESI on retail) and returns; otherwise the counter advances by one.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_27c6(BnCrtStream* f, uint8_t ch, int32_t* written,
                                              BnFlsbufFn flsbuf) {
    if ((f->flag & 0x40) == 0 || f->base != 0) {
        f->cnt -= 1;
        int32_t rv;
        if (f->cnt < 0) {
            rv = flsbuf((int32_t)ch, f);
        } else {
            *(uint8_t*)(uintptr_t)f->ptr = ch;
            f->ptr += 1;
            rv = (int32_t)ch;
        }
        if (rv == -1) {
            *written = -1;
            return;
        }
    }
    *written += 1;
}

// 1.13c BNClient+0x027F9 (36 B, caller 0x2854): _write_multi_char. Emit the same character count times,
// stopping when the count runs out or the written counter becomes -1.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_27f9(BnCrtStream* f, uint8_t ch, int32_t count,
                                              int32_t* written, BnFlsbufFn flsbuf) {
    for (;;) {
        if (count < 1) return;
        count -= 1;
        d2_fn_bnclient_27c6(f, ch, written, flsbuf);
        if (*written == -1) return;
    }
}

// 1.13c BNClient+0x0281D (55 B, caller 0x2854): _write_string. A string stream with no base only adds
// count to the written counter; otherwise emit count bytes from the source, stopping on a -1 counter.
// The source pointer is the register the decompile does not recover (the character reaches write_char
// through AL); it is explicit here.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_281d(BnCrtStream* f, const uint8_t* src, int32_t count,
                                              int32_t* written, BnFlsbufFn flsbuf) {
    if ((f->flag & 0x40) == 0 || f->base != 0) {
        for (;;) {
            if (count < 1) return;
            count -= 1;
            d2_fn_bnclient_27c6(f, *src, written, flsbuf);
            ++src;
            if (*written == -1) return;
        }
    } else {
        *written += count;
    }
}

struct BnFreeEnv {
    uint32_t heap_mode;                      // DAT_6FF40BE4 (3 = small-block heap active)
    void* heap;                              // DAT_6FF40BE0, the process heap
    BnLockFn lock;                           // 0x3163 __lock
    BnLockFn unlock;                         // 0x30AE __unlock (thunk 0x3D47 passes 4)
    BnSbhFindBlockFn sbh_find_block;         // 0x2558 ___sbh_find_block
    BnSbhFreeBlockFn sbh_free_block;         // 0x2593 ___sbh_free_block
    BnHeapFreeFn heap_free;                  // kernel32 HeapFree
};

// 1.13c BNClient+0x03CF4 (104 B, many callers): _free. NULL is ignored. With the small-block heap active
// take __lock(4), look the block up, release it when found, __unlock(4); when the block was found the
// call is done. Otherwise (or on a small-block miss) HeapFree(heap, 0, memory).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_3cf4(void* memory, const BnFreeEnv* env) {
    if (memory == nullptr) return;
    if (env->heap_mode == 3) {
        env->lock(4);
        uint32_t* const block = env->sbh_find_block(memory);
        if (block != nullptr) env->sbh_free_block(block, memory);
        env->unlock(4);
        if (block != nullptr) return;
    }
    env->heap_free(env->heap, 0, memory);
}

// 1.13c BNClient+0x03E29 (14 B, inlined; many callers): __security_check_cookie. Equal to DAT_6FF3D4D0
// returns, otherwise 0x3DF8 __report_failure runs and never returns.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_3e29(uint32_t stack_cookie, uint32_t security_cookie,
                                              void (*report_failure)(void)) {
    if (stack_cookie == security_cookie) return;
    report_failure();
}

// 1.13c BNClient+0x060A0 (139 B, callers 0x2854, 0x42D7, 0x72CA, 0x47FD, 0x6FA7): _strlen. Count bytes
// up to the first NUL. Retail's alignment/dword scan produces the same length.
extern "C"
EMSCRIPTEN_KEEPALIVE size_t d2_fn_bnclient_60a0(const char* s) {
    const char* p = s;
    while (*p != '\0') ++p;
    return (size_t)(p - s);
}

struct BnWctombEnv {
    BnGetPtdFn getptd;                       // 0x3255 __getptd
    BnUpdatetlocinfoFn updatetlocinfo;       // 0x3792 ___updatetlocinfo
    BnWctombMtFn wctomb_mt;                  // 0x612B ___wctomb_mt
    void* global_ptlocinfo;                  // PTR_DAT_6FF3D21C
    uint32_t ptlocinfo_offset;               // _tfpecode offset in retail's _ptiddata
};

// 1.13c BNClient+0x0618B (39 B, caller 0x2854): _wctomb. Load the thread data block, take its locale
// pointer; when that differs from the module's global locale refresh it through ___updatetlocinfo, then
// convert through ___wctomb_mt. The offset of the pointer inside _ptiddata is a seam (Ghidra exposes
// only the field name).
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_bnclient_618b(char* mb, uint16_t wch, const BnWctombEnv* env) {
    void* ptlocinfo = *(void**)((uint8_t*)env->getptd() + env->ptlocinfo_offset);
    if (ptlocinfo != env->global_ptlocinfo) ptlocinfo = env->updatetlocinfo();
    return env->wctomb_mt(ptlocinfo, mb, wch);
}

// 1.13c BNClient+0x061C0 (149 B, caller 0x2854): __aulldvrm. Unsigned 64/64 divide; the dividend is
// (p1 low, p2 high), the divisor (p3 low, p4 high), and the returned EDX:EAX pair is the full quotient
// (the high dword is p2/p3 in the 32-bit-divisor branch, zero otherwise). Transcribed from the shifted
// divide and its one-step correction.
extern "C"
EMSCRIPTEN_KEEPALIVE uint64_t d2_fn_bnclient_61c0(uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4) {
    uint32_t quotient_high = p1;
    uint32_t divisor_high = p4;
    uint32_t dividend_high = p2;
    uint32_t divisor_low = p3;
    uint32_t quotient;
    if (p4 == 0) {
        quotient_high = p2 / p3;
        quotient = (uint32_t)((((uint64_t)(p2 % p3) << 32) | p1) / (uint64_t)p3);
    } else {
        uint32_t half;
        do {
            half = divisor_high >> 1;
            divisor_low = (divisor_low >> 1) | ((divisor_high & 1) << 31);
            const uint32_t next_high = dividend_high >> 1;
            quotient_high = (quotient_high >> 1) | ((dividend_high & 1) << 31);
            divisor_high = half;
            dividend_high = next_high;
        } while (half != 0);
        quotient = (uint32_t)((((uint64_t)dividend_high << 32) | quotient_high) / (uint64_t)divisor_low);
        const uint64_t product = (uint64_t)p3 * quotient;
        const uint32_t product_high = (uint32_t)(product >> 32);
        const uint32_t sum = product_high + (uint32_t)((uint64_t)quotient * p4);
        if (sum < product_high || p2 < sum || (p2 == sum && p1 < (uint32_t)product)) quotient -= 1;
        quotient_high = 0;
    }
    return ((uint64_t)quotient_high << 32) | quotient;
}

// 1.13c BNClient+0x07C56 (9 B, caller 0x2854): __fptrap. Tail call 0x2242 __amsg_exit with code 2.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_7c56(BnAmsgExitFn amsg_exit) {
    amsg_exit(2);
}
