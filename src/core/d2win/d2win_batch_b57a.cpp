// src/core/d2win/d2win_batch_b57a.cpp -- D2Win.dll VC7.1 CRT leaves (alpha lane: implementation + marker,
// no spec/vector), from build-wasm/ghidra-d2win/decomp. Win32 callees are seams (function pointers).
// Not ported: 0x5CB8 (VirtualFree/HeapFree heap), 0x2A1F/0x2A93/0x4165 (TLS, locks, kernel32),
// 0x379C (WriteFile/MessageBox), 0x42BA (SEH chain).
#include <cstdint>
#include <initializer_list>
#include <emscripten.h>

// Locale info fields used by the two *_mt routines (retail offsets in the comment).
struct D2WinB57Loc {
    uint32_t codepage;          // +0x04
    uint32_t lc_handle;         // +0x14
    uint32_t ctype_ascii;       // +0x24
    int32_t  mb_cur_max;        // +0x28
    const uint16_t* pctype;     // +0x48
};

struct D2WinB57NhEnv {
    void*   (*heap_alloc)(uint32_t size);   // 0x3F3B __heap_alloc
    int32_t (*callnewh)(uint32_t size);     // 0x6768 __callnewh
};

// 1.13c D2Win+0x3FB6
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2win_nh_malloc_3fb6(uint32_t size, int32_t nhflag, const D2WinB57NhEnv* env) {
    if (size >= 0xffffffe1u) return nullptr;
    for (;;) {
        void* p = env->heap_alloc(size);
        if (p != nullptr) return p;
        if (nhflag == 0) return nullptr;
        if (env->callnewh(size) == 0) return nullptr;
    }
}

struct D2WinB57ReportEnv {
    void (*report)(uint32_t code);          // 0x6CF4
    void (*exit_process)(uint32_t code);    // ExitProcess
};

// 1.13c D2Win+0x407B
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_report_failure_407b(const D2WinB57ReportEnv* env) {
    env->report(1);
    env->exit_process(3);
}

// 1.13c D2Win+0x2840
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2win_ascii_stricmp_2840(const char* a, const char* b) {
    for (;;) {
        uint8_t x = (uint8_t)*a++, y = (uint8_t)*b++;
        uint8_t fx = (uint8_t)(x - 'A' < 26u ? x + 32 : x);
        uint8_t fy = (uint8_t)(y - 'A' < 26u ? y + 32 : y);
        if (fx != fy) return fx < fy ? -1 : 1;
        if (y == 0) return 0;
    }
}

struct D2WinB57MsgEnv {
    int32_t mode_a;                     // DAT_6f901034
    int32_t mode_b;                     // DAT_6f901038
    void (*write_msg)(int32_t id);      // 0x379C
    void (*user_hook)();                // DAT_6f9012bc, may be null
};

struct D2WinB57NmsgEnv {
    int32_t mode_a, mode_b;              // DAT_6f901034 / DAT_6f901038
    const int32_t* ids;                  // DAT_6f8ff3b8 table, 0x13 {id,msg} pairs
    const char* const* msgs;
    void (*write_stdout)(const char* s, uint32_t n);      // GetStdHandle(-12)+WriteFile
    int32_t (*module_name)(char* buf, uint32_t cap);      // GetModuleFileNameA(0,..,0x104); 0 = fail
    void (*message_box)(const char* text, const char* title, uint32_t flags);  // ___crtMessageBoxA
};

// 1.13c D2Win+0x379C  (_NMSG_WRITE)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_NMSG_WRITE_379c(int32_t id, const D2WinB57NmsgEnv* env) {
    uint32_t i = 0;
    do { if (id == env->ids[i]) break; i++; } while (i < 0x13);
    if (id != env->ids[i]) return;
    const char* msg = env->msgs[i];
    uint32_t mlen = (uint32_t)__builtin_strlen(msg);
    if (env->mode_a == 1 || (env->mode_a == 0 && env->mode_b == 1)) {
        env->write_stdout(msg, mlen);
    } else if (id != 0xfc) {
        char prog[0x105]; prog[0x104] = 0;
        if (env->module_name(prog, 0x104) == 0) __builtin_strcpy(prog, "<program name unknown>");
        const char* p = prog;
        uint32_t plen = (uint32_t)__builtin_strlen(p);
        if (plen + 1 > 0x3c) {              // CRT: tail of the path behind "..."
            p = prog + plen + 1 - 0x3c;
            __builtin_memcpy(prog + plen + 1 - 0x3c, "...", 3);
        }
        char out[0x104 + 0x80 + 0x100 + 64];
        char* o = out;
        auto cat = [&](const char* s) { while ((*o = *s++)) o++; };
        cat("Runtime Error!\n\nProgram: "); cat(p); cat("\n\n"); cat(msg);
        env->message_box(out, "Microsoft Visual C++ Runtime Library", 0x12010);
    }
}

// 1.13c D2Win+0x3914
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_FF_MSGBANNER_3914(const D2WinB57MsgEnv* env) {
    if (env->mode_a == 1 || (env->mode_a == 0 && env->mode_b == 1)) {
        env->write_msg(0xfc);
        if (env->user_hook) env->user_hook();
        env->write_msg(0xff);
    }
}

struct D2WinB57WcEnv {
    // WideCharToMultiByte(codepage, 0, &wc, 1, out, mb_cur_max, 0, &usedDefault); returns bytes
    int32_t (*wc2mb)(uint32_t codepage, uint16_t wc, char* out, int32_t outlen, int32_t* used_default);
    int32_t* errno_ptr;                 // 0x41F0 __errno
};

// 1.13c D2Win+0x3FF4
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2win_wctomb_mt_3ff4(const D2WinB57Loc* loc, char* out, uint16_t wc, const D2WinB57WcEnv* env) {
    if (out == nullptr) return 0;
    if (loc->lc_handle == 0) {
        if (wc < 0x100) { *out = (char)wc; return 1; }
    } else {
        int32_t used = 0;
        int32_t n = env->wc2mb(loc->codepage, wc, out, loc->mb_cur_max, &used);
        if (n != 0 && used == 0) return n;
    }
    *env->errno_ptr = 0x2a;
    return -1;
}

struct D2WinB57LowerEnv {
    uint32_t (*isctype)(const D2WinB57Loc* loc, uint32_t c, uint32_t mask);   // 0x4B8D
    // 0x47B7 __crtLCMapStringA(lcid, 0x100, src, srclen, dst, 3, codepage, 1); returns bytes written
    uint32_t (*lcmap)(uint32_t lcid, uint32_t flags, const uint8_t* src, uint32_t srclen, uint8_t* dst, uint32_t dstlen, uint32_t codepage, uint32_t xl);
};

// 1.13c D2Win+0x274D
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2win_tolower_mt_274d(const D2WinB57Loc* loc, uint32_t c, const D2WinB57LowerEnv* env) {
    if (loc->lc_handle == 0 || (loc->ctype_ascii != 0 && c < 0x80)) {
        if ((int32_t)c > 0x40 && (int32_t)c < 0x5b) return c + 0x20;
        return c;
    }
    if (c < 0x100) {
        uint32_t up = loc->mb_cur_max < 2 ? (loc->pctype[c] & 1) : env->isctype(loc, c, 1);
        if (up == 0) return c;
    }
    uint8_t src[4] = {0, 0, 0, 0}, dst[4] = {0, 0, 0, 0};
    uint32_t n;
    if ((((const uint8_t*)loc->pctype)[1 + ((c >> 8) & 0xff) * 2] & 0x80) == 0) {
        src[0] = (uint8_t)c; n = 1;
    } else {
        src[0] = (uint8_t)(c >> 8); src[1] = (uint8_t)c; n = 2;
    }
    uint32_t r = env->lcmap(loc->lc_handle, 0x100, src, n, dst, 3, loc->codepage, 1);
    if (r == 0) return c;
    if (r != 1) return ((uint32_t)dst[0] << 8) | dst[1];
    return dst[0];
}
