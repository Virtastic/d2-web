// src/core/realm/bnclient_batch2_crt.cpp -- three UNMAPPED 1.13c BNClient.dll rows reimplemented from the
// decompiles in build-wasm/ghidra-bnclient/decomp/0x<RVA>.c (alpha lane). Base 0x6FF20000; every global
// is named at its retail RVA. Host services retail imports (the kernel32 file/console calls, the CRT
// locale tables and the message writer) are seams after the retail arguments, which keep their order
// (src/core/realm/mcp_crt_mem.cpp pattern). The rows are MSVC 2003 (VC7.1) Release CRT leaves.
#include <cstdint>
#include <emscripten.h>

// The 1.13c FILE (32-bit CRT): ptr 0x0, cnt 0x4, base 0x8, flag 0xC, file 0x10, charbuf 0x14, bufsiz 0x18,
// tmpfname 0x1C. Same layout as src/core/fn/crt_leaves_b19.cpp and src/core/d2gfx/d2gfx_writestr.cpp.
struct BnCrtStream32 {
    uint32_t ptr;
    int32_t  cnt;
    uint32_t base;
    uint32_t flag;
    int32_t  file;
    uint32_t charbuf;
    int32_t  bufsiz;
    uint32_t tmpfname;
};
static_assert(__builtin_offsetof(BnCrtStream32, ptr) == 0x0 && __builtin_offsetof(BnCrtStream32, cnt) == 0x4 &&
              __builtin_offsetof(BnCrtStream32, base) == 0x8 && __builtin_offsetof(BnCrtStream32, flag) == 0xC &&
              __builtin_offsetof(BnCrtStream32, file) == 0x10 && __builtin_offsetof(BnCrtStream32, charbuf) == 0x14 &&
              __builtin_offsetof(BnCrtStream32, bufsiz) == 0x18 && __builtin_offsetof(BnCrtStream32, tmpfname) == 0x1C &&
              sizeof(BnCrtStream32) == 32, "1.13c MSVC 2003 FILE layout");

struct BnFlsbufEnv {
    void (*getbuf)(BnCrtStream32* f);                                   // 0x5ECD __getbuf
    int32_t (*write)(int32_t handle, const void* buf, uint32_t count);  // 0x5E22 __write
    int32_t (*isatty)(int32_t handle);                                  // 0x5F11 __isatty
    int32_t (*lseek)(int32_t handle, int32_t offset, int32_t origin);   // 0x5BBB __lseek
    uint8_t (*ioinfo_flags)(int32_t handle);                            // ioinfo[handle] flag byte (+4)
    const BnCrtStream32* stdout_stream;                                 // 0x6FF3D660
    const BnCrtStream32* stderr_stream;                                 // 0x6FF3D680
};

// 1.13c BNClient+0x026AD (281 B, callers 0x138C __vsnprintf, 0x159A _sprintf, 0x27C6 write_char):
// __flsbuf(ch, file) -- MSVC 2003 `_flsbuf`: put one character out, flushing the stream's buffer first.
// No _IOWRT (0x2) or _IORW (0x80), or _IOSTRG (0x40): flag |= _IOERR (0x20), return -1. _IOREAD (0x1):
// cnt = 0, requires _IOEOF (0x10) else the error, ptr = base and clear _IOREAD. Then cnt = 0, flag =
// (flag & ~0x10) | _IOWRT. A stream with no _IOFBF/_IOLBF bits (0x100) is written unbuffered: one byte
// through __write. Otherwise ptr = base + 1, count = old ptr - base, cnt = bufsiz - 1; a count >= 1 flushes
// old ptr-base bytes through __write, a count < 1 only seeks an FAPPEND handle (ioinfo flag 0x20) to the
// end; then base[0] = ch. Return ch & 0xFF only when __write returned the whole count, else -1 with
// flag |= _IOERR.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t bn_flsbuf(int32_t ch, BnCrtStream32* f, const BnFlsbufEnv* env) {
    uint32_t flag = f->flag;
    const int32_t handle = f->file;
    if ((flag & 0x82) == 0 || (flag & 0x40) != 0) {
        f->flag = flag | 0x20;
        return -1;
    }
    if ((flag & 0x01) != 0) {                       // _IOREAD
        f->cnt = 0;
        if ((flag & 0x10) == 0) {                   // not _IOEOF: an unread/write transition is an error
            f->flag = flag | 0x20;
            return -1;
        }
        f->ptr = f->base;
        f->flag = flag & 0xfffffffeu;
    }
    flag = f->flag;
    f->cnt = 0;
    f->flag = (flag & 0xffffffefu) | 0x02u;         // clear _IOEOF, set _IOWRT
    if ((flag & 0x10c) == 0 &&
        ((f != env->stdout_stream && f != env->stderr_stream) || env->isatty(handle) == 0)) {
        env->getbuf(f);
    }
    int32_t rc = 0;
    int32_t count;
    if ((f->flag & 0x108) == 0) {                   // unbuffered: __write(handle, &ch, 1)
        const uint8_t c = (uint8_t)ch;
        count = 1;
        rc = env->write(handle, &c, 1);
    } else {
        const uint32_t base = f->base;
        const uint32_t p = f->ptr;
        f->ptr = base + 1;
        count = (int32_t)(p - base);
        f->cnt = f->bufsiz - 1;
        if (count < 1) {
            if ((env->ioinfo_flags(handle) & 0x20) != 0) env->lseek(handle, 0, 2);   // FAPPEND: seek to end
        } else {
            rc = env->write(handle, (const void*)(uintptr_t)base, (uint32_t)count);
        }
        *(uint8_t*)(uintptr_t)base = (uint8_t)ch;
    }
    if (rc == count) return ch & 0xff;
    f->flag |= 0x20;
    return -1;
}

struct BnCrtGetStringType {
    int32_t (*get_string_type_a)(uint32_t info_type, const uint8_t* bytes, int32_t count,
                                 uint16_t* out, uint32_t codepage);   // 0x6D18 ___crtGetStringTypeA
};

// 1.13c BNClient+0x03852 (119 B, callers 0x7C5F, 0x1C76, 0x16D9, 0x19FE, 0x1E1F): ___isctype_mt(c, mask)
// -- test the ctype mask of one character. For c in [-1, 0xFF] the answer is the ctype table word at
// index c & mask (the locale's table pointer lives at locale+0x48; index -1 is the EOF entry, so retail's
// table pointer is 2 bytes into the 0x100-word array). Otherwise the code is a multibyte sequence: when
// the table word of its high byte has bit 15 clear it is one byte (c & 0xFF), else two high byte first;
// those bytes go through ___crtGetStringTypeA (CT_CTYPE1) and the returned word & mask is the answer, 0
// when that call fails. The table pointer is an argument so the rule stays a pure function of it.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t bn_isctype_mt(const uint16_t* ctype, int32_t c, uint32_t mask,
                                            uint32_t codepage,
                                            const BnCrtGetStringType* env) {
    if ((uint32_t)(c + 1) < 0x101u) return ctype[c] & mask;
    uint8_t bytes[2];
    int32_t count;
    if ((ctype[(c >> 8) & 0xff] & 0x8000u) == 0) {
        bytes[0] = (uint8_t)c;
        count = 1;
    } else {
        bytes[0] = (uint8_t)(c >> 8);
        bytes[1] = (uint8_t)c;
        count = 2;
    }
    uint16_t flags = 0;
    if (env->get_string_type_a(1, bytes, count, &flags, codepage) == 0) return 0;
    return flags & mask;
}

struct BnAmsgExitEnv {
    void (*ff_msgbanner)(void);           // 0x4975 __FF_MSGBANNER
    void (*nmsg_write)(int32_t code);     // 0x47FD the runtime-message writer
    void (*exit_fn)(uint32_t code);       // PTR___exit 0x6FF3D080, 0xFF, terminal
};

static uint32_t g_bn_amsg_exit_done;      // +0x1E374
static uint32_t g_bn_amsg_term_done;      // +0x1E378

// 1.13c BNClient+0x02242 (51 B, callers 0x3255 __getptd, 0x7C56, 0x3163, 0x1E59): __amsg_exit(code).
// When _C_Exit_Done (0x1E374) is 1, or it is 0 and the termination-done flag (0x1E378) is 1, write the
// runtime message banner; then write the runtime message for `code` and terminate the process via the
// __exit pointer with status 0xFF. Both writers and the exit are host seams; the flag test is the rule.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_amsg_exit(int32_t code, const BnAmsgExitEnv* env) {
    if (g_bn_amsg_exit_done == 1 || (g_bn_amsg_exit_done == 0 && g_bn_amsg_term_done == 1))
        env->ff_msgbanner();
    env->nmsg_write(code);
    env->exit_fn(0xff);
}
