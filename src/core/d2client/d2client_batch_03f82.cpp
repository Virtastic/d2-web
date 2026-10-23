// src/core/d2client/d2client_batch_03f82.cpp -- ten UNMAPPED 1.13c D2Client.dll CRT rows (and later
// 0x080C5 __filbuf) reimplemented
// from the decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane). Base 0x6FAB0000;
// every RVA below is retail's and every global this file owns is a static named at its retail RVA.
// These are MSVC 2003 Release CRT bodies the linker pulled into the DLL. Recovered callees are called
// directly (__getptd 0x72F7, __errno 0x579C, _free 0x4891, 0x908F); the rest are host seams AFTER the
// retail arguments, which keep their order (the d2client_batch_1e70.cpp pattern).
//
// __CRT_INIT@12 (0x04475) is reimplemented in crt_init_4475.cpp. entry (0x045F6) stays UNMAPPED: it is
// the image's process-startup / TLS-callback dispatcher (a DllMain pointer call plus the CRT init
// above), i.e. host process state, not a game rule. It is not stubbed here.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>
#include <Windows.h>   // DeleteCriticalSection: host (kernel) state, a no-op under wasm
#include "client_host.h"

// The CRT's FILE fields these rows touch (retail: _ptr +0, _cnt +4, _base +8, _flag +0xC, _file +0x10,
// _charbuf +0x14, _bufsiz +0x18, _tmpfname +0x1C), in the tree's packed wasm64 form: the three
// pointer fields widen, while the one-byte inline _charbuf keeps its retail width and three bytes
// of explicit padding preserve the following field offsets.
#pragma pack(push, 1)
struct D2cFile {
    char*    ptr;       // +0x00
    int32_t  cnt;       // +0x04
    char*    base;      // +0x08
    int32_t  flag;      // +0x0C
    int32_t  file;      // +0x10  file descriptor
    uint8_t  charbuf;   // +0x14 (retail inline fallback byte storage; do not widen)
    uint8_t  charbuf_pad[3]; // +0x15..+0x17
    int32_t  bufsiz;    // +0x18
    char*    tmpfname;  // +0x1C
};
#pragma pack(pop)
static_assert(offsetof(D2cFile, cnt) == 0x04 + 1 * (sizeof(void*) - 4), "FILE._cnt");
static_assert(offsetof(D2cFile, base) == 0x08 + 1 * (sizeof(void*) - 4), "FILE._base");
static_assert(offsetof(D2cFile, flag) == 0x0C + 2 * (sizeof(void*) - 4), "FILE._flag");
static_assert(offsetof(D2cFile, file) == 0x10 + 2 * (sizeof(void*) - 4), "FILE._file");
static_assert(offsetof(D2cFile, charbuf) == 0x14 + 2 * (sizeof(void*) - 4), "FILE._charbuf");
static_assert(sizeof(((D2cFile*)nullptr)->charbuf) == 1, "FILE._charbuf is one retail byte");
static_assert(offsetof(D2cFile, bufsiz) == 0x18 + 2 * (sizeof(void*) - 4), "FILE._bufsiz");
static_assert(offsetof(D2cFile, tmpfname) == 0x1C + 2 * (sizeof(void*) - 4), "FILE._tmpfname");

// +0x1239A0  DAT_6fbd39a0: file-descriptor table; fd >> 5 selects a block of 0x20 records of 0x24
// bytes, and byte +4 of a record is its OS device flags (0x80 = text newline translation, 0x20 = the
// console, 0x04 = append).
uint32_t g_pioinfo[0x20];   // shared with the b56 fd-table rows (_write, __close_lk)
// +0x0DB790  DAT_6fb8b790: the record used for the console handle -1 in __flsbuf.
static uint8_t g_console_dev[0x24];
// +0x0DB218 / +0x0DB238  _iob[1] (stdout) and _iob[2] (stderr); __flsbuf asks __isatty about them.
static uintptr_t g_std_files[2];

static uint8_t* d2c_fd_record(int32_t handle) {
    return (uint8_t*)(uintptr_t)(g_pioinfo[(uint32_t)handle >> 5] +
                                 ((uint32_t)handle & 0x1F) * 0x24);
}

// Scenario setup: the CRT's fd table and the two stdio streams __flsbuf special-cases.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_ioinfo_pin(uint32_t slot, uint32_t base) {
    if (slot < 0x20) g_pioinfo[slot] = base;
}
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_iob_pin(uint32_t out, uint32_t err) {
    g_std_files[0] = out;
    g_std_files[1] = err;
}
// Read-only view of the fd table for the CRT rows in d2client_batch_057ae.cpp (__close_lk 0x605D,
// __close 0x60E0): one module object, so the two TUs must not each own a table.
extern "C"
EMSCRIPTEN_KEEPALIVE uint8_t* d2_client_ioinfo_record(int32_t handle) {
    return d2c_fd_record(handle);
}

typedef uint32_t (*D2cOnexitFn)(uint32_t func);
// 1.13c D2Client+0x03F82 (18 B, callers 0x3537, 0xDF00): _atexit. __onexit (0x3F4A) registers the
// function pointer and returns it, or 0 on failure; _atexit maps a non-null result to 0 and null to -1.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_3f82(uint32_t func, D2cOnexitFn onexit_) {
    return (int32_t)(onexit_(func) != 0) - 1;
}

// --- the ctype family (0x3F94/0x3FD3/0x400D/0x404C) ---------------------------------------------
// _ptiddata +0x64 holds the thread's pthreadlocinfo (0x03F94: `mov 0x64(%eax),%eax`). The locale
// block's lc_category[1].refcount is +0x28 (`cmpl $0x1,0x28(%eax)`) and lc_category[3].refcount, the
// ctype table pointer, is +0x48; each table entry is a 16-bit flag word. Above one locale reference
// every test routes through ___isctype_mt (0x707B, still UNMAPPED, so a seam) instead.
uint32_t g_default_locale;   // +0x0DB63C  PTR_DAT_6fb8b63c: the CRT default locale pointer (shared with b56)

// __getptd is NAMED in d2client_batch_908f.cpp; the env shape is repeated so the TUs agree at the
// C ABI (function pointers only).
struct D2cGetptdEnv {
    uint32_t (*get_last_error)();
    void     (*set_last_error)(uint32_t);
    void*    (*tls_get_value)(uint32_t);
    int32_t  (*tls_set_value)(uint32_t, void*);
    void*    (*calloc_)(uint32_t, uint32_t);
    uint32_t (*get_current_thread_id)();
    void     (*amsg_exit)(int32_t);
};
extern "C" void* d2_client_72f7(const D2cGetptdEnv* env);

typedef void*    (*D2cUpdatetlocinfoFn)(void);
typedef uint32_t (*D2cIsctypeMtFn)(void* self, void* locale, int32_t c, uint32_t mask);

static uint32_t d2c_ctype_lookup(int32_t c, uint32_t mask, const D2cGetptdEnv* env,
                                 D2cUpdatetlocinfoFn updatetlocinfo, D2cIsctypeMtFn isctype_mt) {
    const uint8_t* ptd = (const uint8_t*)d2_client_72f7(env);
    const uint8_t* locale = (const uint8_t*)(uintptr_t)*(const uint32_t*)(ptd + 0x64);
    if ((uintptr_t)locale != (uintptr_t)g_default_locale) locale = (const uint8_t*)updatetlocinfo();
    if (*(const int32_t*)(locale + 0x28) > 1)
        return isctype_mt(nullptr, (void*)locale, c, mask);
    const uint32_t table = *(const uint32_t*)(locale + 0x48);
    return *(const uint16_t*)(uintptr_t)(table + (uint32_t)c * 2) & mask;
}

// 1.13c D2Client+0x03f94 (63 B, caller 0x38CD0): _isalpha -- ctype mask 0x103 (_ALPHA, _UPPER/_LOWER).
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_3f94(int32_t c, const D2cGetptdEnv* env,
                                            D2cUpdatetlocinfoFn updatetlocinfo,
                                            D2cIsctypeMtFn isctype_mt) {
    return (int32_t)d2c_ctype_lookup(c, 0x103, env, updatetlocinfo, isctype_mt);
}

// 1.13c D2Client+0x03fd3 (58 B, callers 0x61BC, 0x38CD0, 0xCB1C5, 0xCC903): _iscntrl -- mask 4.
// (Retail's byte read of the 16-bit entry is the compiler narrowing its own table; the mask keeps the
// low bit only, so the 16-bit read above is identical.)
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_3fd3(int32_t c, const D2cGetptdEnv* env,
                                            D2cUpdatetlocinfoFn updatetlocinfo,
                                            D2cIsctypeMtFn isctype_mt) {
    return (int32_t)d2c_ctype_lookup(c, 4, env, updatetlocinfo, isctype_mt);
}

// 1.13c D2Client+0x0400d (63 B, caller 0x61BC): _isxdigit -- mask 0x80.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_400d(int32_t c, const D2cGetptdEnv* env,
                                            D2cUpdatetlocinfoFn updatetlocinfo,
                                            D2cIsctypeMtFn isctype_mt) {
    return (int32_t)d2c_ctype_lookup(c, 0x80, env, updatetlocinfo, isctype_mt);
}

// 1.13c D2Client+0x0404c (58 B, caller 0x61BC): _isspace -- mask 8.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_404c(int32_t c, const D2cGetptdEnv* env,
                                            D2cUpdatetlocinfoFn updatetlocinfo,
                                            D2cIsctypeMtFn isctype_mt) {
    return (int32_t)d2c_ctype_lookup(c, 8, env, updatetlocinfo, isctype_mt);
}

extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_ctype_pin(uint32_t default_locale) {
    g_default_locale = default_locale;
}
// The default locale pointer, read by ___updatetlocinfo_lk (0x71C2 in d2client_batch_057ae.cpp).
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_default_locale(void) {
    return g_default_locale;
}

// --- stdio reads --------------------------------------------------------------------------------
typedef int32_t (*D2cFilbufFn)(void* file);
typedef int32_t (*D2cReadFn)(int32_t fd, void* buf, uint32_t count);

// 1.13c D2Client+0x040C5 (233 B): __fread_lk. total = _ElementSize * _Count (a zero product returns
// 0 without touching the stream). Draw from the read buffer while the stream is buffered (`_flag &
// 0x10C`) and `_cnt` is non-zero, at most `_cnt` bytes per pass. Otherwise refill: a short request
// (`remaining < buf chunk`) pulls one byte through __filbuf (0x80C5) and reloads `_bufsiz`; a long one
// reads `remaining - remaining % chunk` bytes through __read (0x8383) straight into the destination.
// EOF sets 0x10 (a read error 0x20) and ends the loop; the return is bytes consumed / _ElementSize.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_40c5(void* dst, uint32_t element_size, uint32_t count,
                                             void* file, D2cFilbufFn filbuf, D2cReadFn read_) {
    const uint32_t total = element_size * count;
    if (total == 0) return 0;
    D2cFile* f = (D2cFile*)file;
    uint32_t remaining = total;
    uint8_t* out = (uint8_t*)dst;
    uint32_t chunk = (f->flag & 0x10C) == 0 ? 0x1000u : (uint32_t)f->bufsiz;
    while (remaining != 0) {
        if ((f->flag & 0x10C) == 0 || f->cnt == 0) {
            if (remaining < chunk) {
                const int32_t ch = filbuf(file);
                if (ch == -1) return (total - remaining) / element_size;
                *out++ = (uint8_t)ch;
                chunk = (uint32_t)f->bufsiz;
                --remaining;
            } else {
                uint32_t size = remaining;
                if (chunk != 0) size = remaining - remaining % chunk;
                const int32_t n = read_(f->file, out, size);
                if (n == 0) {
                    f->flag |= 0x10;
                    return (total - remaining) / element_size;
                }
                if (n == -1) {
                    f->flag |= 0x20;
                    return (total - remaining) / element_size;
                }
                remaining -= (uint32_t)n;
                out += n;
            }
        } else {
            const uint32_t size = (uint32_t)f->cnt <= remaining ? (uint32_t)f->cnt : remaining;
            std::memcpy(out, f->ptr, size);
            remaining -= size;
            f->cnt -= (int32_t)size;
            f->ptr += size;
            out += size;
        }
    }
    return total / element_size;
}

// 1.13c D2Client+0x080C5 (225 B, callers 0x040C5 __fread_lk, 0x061A6 __inc): __filbuf, the refill
// behind getc. A stream that is neither in use nor at EOF (flag 0x83) or that is a string stream
// (0x40) returns -1. A write-only stream (0x02) takes the error bit 0x20 and returns -1. Otherwise the
// stream becomes read-in-use (0x01) and gets a buffer: __getbuf when it has none (no 0x10C bits), else
// _ptr = _base. One __read of _bufsiz bytes from _base fills it; the count lands in _cnt. A read of
// 0 (EOF) or -1 (error) sets 0x10 resp. 0x20, clears _cnt and returns -1. On data, a stream that is
// not read-only-and-not-write (0x82 clear) asks the fd record (console record for fd -1): device flags
// 0x82 mark the UTF-8/text console translation and set stream flag 0x2000. A 0x200 buffer on a
// malloc'ed (0x08) non-user (0x400) stream grows to 0x1000. Then _cnt = count - 1, the byte at _ptr
// is returned and _ptr advances. __read (0x8383) is a host seam AFTER the retail argument; __getbuf
// (0x9F56) is recovered below and is passed as the callback.
typedef void (*D2cGetbufFn)(void* file);   // same as the write-flush section's below
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_80c5(D2cFile* f, D2cReadFn read_, D2cGetbufFn getbuf) {
    const uint32_t flag = (uint32_t)f->flag;
    if ((flag & 0x83) == 0 || (flag & 0x40) != 0) return -1;
    if ((flag & 2) != 0) {
        f->flag = (int32_t)(flag | 0x20);
        return -1;
    }
    f->flag = (int32_t)(flag | 1);
    if ((flag & 0x10C) == 0) {
        getbuf(f);
    } else {
        f->ptr = f->base;
    }
    const int32_t n = read_(f->file, f->base, (uint32_t)f->bufsiz);
    f->cnt = n;
    if (n != 0 && n != -1) {
        if ((f->flag & 0x82) == 0) {
            const int32_t handle = f->file;
            const uint8_t* rec = handle == -1 ? g_console_dev : d2c_fd_record(handle);
            if ((rec[4] & 0x82) == 0x82) f->flag |= 0x2000;
        }
        if (f->bufsiz == 0x200 && (f->flag & 8) != 0 && (f->flag & 0x400) == 0)
            f->bufsiz = 0x1000;
        f->cnt = n - 1;
        const uint8_t byte = (uint8_t)*f->ptr;
        f->ptr += 1;
        return (int32_t)byte;
    }
    f->flag |= n != 0 ? 0x20 : 0x10;
    f->cnt = 0;
    return -1;
}

// --- stdio buffer allocation --------------------------------------------------------------------
typedef void* (*D2cMallocGetbufFn)(uint32_t size);
extern uint32_t g_streamCount;   // +0x0FAA50, d2client_batch_0312a.cpp

// 1.13c D2Client+0x09f56 (68 B, callers 0x080C5 __filbuf, 0x04AE8 __flsbuf, 0x0A975): __getbuf.
// Bump the stream counter, then _malloc(0x1000) into _base. On failure _flag |= 4 (F_ERR) and the
// stream falls back to the address of its inline _charbuf field with _bufsiz = 2; on success
// _flag |= 8 (the buffer is malloc'ed) with _bufsiz = 0x1000. Either way _cnt = 0, _ptr = _base.
// _malloc (0x7D5F) is recovered, but takes its own host env; it is a seam here like __read and __write.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_9f56(D2cFile* f, D2cMallocGetbufFn malloc_) {
    ++g_streamCount;
    char* base = (char*)malloc_(0x1000);
    f->base = base;
    if (base == nullptr) {
        f->flag |= 4;
        f->base = (char*)&f->charbuf;   // retail stores &_charbuf, not its value
        f->bufsiz = 2;
    } else {
        f->flag |= 8;
        f->bufsiz = 0x1000;
    }
    f->cnt = 0;
    f->ptr = f->base;
}

// 1.13c D2Client+0x0A975 (108 B, callers 0x061BC and 0x09F56): _ungetc.
// 1.13c D2Client+0x0a975 (108 B): FID_conflict__ungetc / __ungetc_lk.
// Push one byte back into a CRT FILE buffer.  The malloc callback is a host seam for
// the only nested call, __getbuf (0x09F56), and follows the retail FILE argument.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_a975(int32_t ch, D2cFile* f, D2cMallocGetbufFn malloc_) {
    uint32_t flag = (uint32_t)f->flag;
    if (ch == -1 || (((flag & 1u) == 0) && ((flag & 0x80000000u) == 0 || (flag & 2u) != 0)))
        return -1;
    if (f->base == nullptr) d2_client_9f56(f, malloc_);
    if (f->ptr == f->base) {
        if (f->cnt != 0) return -1;
        ++f->ptr;
    }
    --f->ptr;
    if (((uint32_t)f->flag & 0x40u) == 0) {
        *f->ptr = (char)ch;
    } else if ((uint8_t)*f->ptr != (uint8_t)ch) {
        ++f->ptr;
        return -1;
    }
    ++f->cnt;
    f->flag = (int32_t)(((uint32_t)f->flag & ~0x10u) | 1u);
    return ch & 0xFF;
}

// --- stdio positioning --------------------------------------------------------------------------
typedef int32_t (*D2cLseekFn)(int32_t fd, int32_t offset, int32_t origin);
extern "C" int32_t* d2_client_579c(void);   // __errno, d2client_batch_4857.cpp
extern "C" void d2_client_4891(uint32_t p, D2cHost host);   // _free, d2client_free_4891.cpp

// 1.13c D2Client+0x041FA (354 B): __ftell_lk. The stream's logical position is the OS position
// (__lseek, 0x86EC, SEEK_CUR) minus the unread remainder of the pushback buffer. With `_flag & 0x108`
// clear the answer is just `pos - _cnt`. Otherwise count the newline translations of the buffered
// bytes when the fd is in text mode, and, when `_flag & 1` (the buffer was written into), fold in the
// buffer end: either the file ended exactly at `pos` (count newlines over `_base.._base+buffered+cnt`,
// plus the 0x20 append translation) or the stream is repositioned and the buffer is rebased to its
// size. An unreadable random-access stream (no read/write bits and `_flag` sign clear) sets EINVAL.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_41fa(void* file, D2cLseekFn lseek_) {
    D2cFile* f = (D2cFile*)file;
    const int32_t handle = f->file;
    if (f->cnt < 0) f->cnt = 0;
    int32_t pos = lseek_(handle, 0, 1);
    if (pos < 0) return -1;
    const uint32_t flag = (uint32_t)f->flag;
    if ((flag & 0x108) == 0) return pos - f->cnt;
    int32_t buffered = (int32_t)(f->ptr - f->base);
    if ((flag & 3) == 0) {
        if ((flag & 0x80) == 0) {
            *d2_client_579c() = 0x16;   // EINVAL
            return -1;
        }
    } else {
        const uint8_t* rec = d2c_fd_record(handle);
        if ((rec[4] & 0x80) != 0 && f->base < f->ptr) {
            for (const char* p = f->base; p < f->ptr; ++p)
                if (*p == '\n') ++buffered;
        }
    }
    if (pos == 0) return buffered;
    if ((flag & 1) != 0) {
        if (f->cnt == 0) {
            buffered = 0;
        } else {
            int32_t end = (int32_t)(f->ptr - f->base) + f->cnt;   // raw, not newline-augmented
            if ((d2c_fd_record(handle)[4] & 0x80) != 0) {
                if (lseek_(handle, 0, 2) == pos) {
                    const char* p = f->base;
                    const char* stop = p + end;
                    for (; p < stop; ++p)
                        if (*p == '\n') ++end;
                    if ((((uint32_t)f->flag >> 8) & 0x20) != 0) ++end;
                } else {
                    lseek_(handle, pos, 0);
                    int32_t size = 0x200;
                    if (0x200u < (uint32_t)end || ((uint32_t)f->flag & 8) == 0 ||
                        ((uint32_t)f->flag & 0x400) != 0) {
                        size = f->bufsiz;
                    }
                    end = size;
                    if ((d2c_fd_record(handle)[4] & 4) != 0) ++end;
                }
            }
            pos -= end;
        }
    }
    return buffered + pos;
}

// --- CRT lock table -----------------------------------------------------------------------------
// +0x0DB0C8  DAT_6fb8b0c8: 0x24 eight-byte records { [0] = CRITICAL_SECTION*, [1] = init flag }.
// +0x0FAA900  DAT_6fbaa900: the static CRITICAL_SECTION blocks those records point at (0x18 each).
static uint32_t g_locks[0x24][2];
static uint8_t  g_lock_objects[0x24 * 0x18];

typedef int32_t (*D2cInitCritSecFn)(void* cs, uint32_t spin);
struct D2cInitCritSecEnv {
    uint32_t (*get_module_handle)(const char* name);
    D2cInitCritSecFn (*get_proc_address)(uint32_t mod, const char* name);
};
extern "C" int32_t d2_client_908f(void* cs, uint32_t spin, const D2cInitCritSecEnv* env);

// 1.13c D2Client+0x0470d (73 B, caller 0x74DE __mtinit): __mtinitlocks. For every slot whose flag word
// is 1 (the statically-initialised locks), point it at its 0x18-byte block and initialise it through
// ___crtInitCritSecAndSpinCount (0x908F) with spin count 4000. Returns 1.
// ponytail: 0x908F returns void in this tree and the wasm host has no failing initialiser, so retail's
// failure arm (clear the slot, return 0) is unreachable; add it if 0x908F ever reports failure.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_470d(const D2cInitCritSecEnv* env) {
    for (uint32_t i = 0; i < 0x24; ++i) {
        if (g_locks[i][1] == 1) {
            void* cs = g_lock_objects + i * 0x18;
            g_locks[i][0] = (uint32_t)(uintptr_t)cs;
            d2_client_908f(cs, 4000, env);
        }
    }
    return 1;
}

// 1.13c D2Client+0x04756 (85 B, caller 0x72C7 __mtterm): __mtdeletelocks. First pass: the heap locks
// (flag word != 1) are deleted and their blocks freed; second pass: the static locks (flag word == 1)
// are deleted in place. The table runs 0x0DB0C8..0x0DB1E8, i.e. 0x24 records.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_4756(D2cHost host) {
    for (uint32_t i = 0; i < 0x24; ++i) {
        if (g_locks[i][0] != 0 && g_locks[i][1] != 1) {
            DeleteCriticalSection((LPCRITICAL_SECTION)(uintptr_t)g_locks[i][0]);
            d2_client_4891(g_locks[i][0], host);
            g_locks[i][0] = 0;
        }
    }
    for (uint32_t i = 0; i < 0x24; ++i) {
        if (g_locks[i][0] != 0 && g_locks[i][1] == 1)
            DeleteCriticalSection((LPCRITICAL_SECTION)(uintptr_t)g_locks[i][0]);
    }
}

// --- stdio write flush --------------------------------------------------------------------------
typedef int32_t (*D2cIsattyFn)(int32_t fd);
typedef void    (*D2cGetbufFn)(void* file);
typedef int32_t (*D2cWriteFn)(int32_t fd, const void* buf, uint32_t count);

// 1.13c D2Client+0x04AE8 (281 B, callers 0x3146, 0x3704, 0x4C01): __flsbuf. A stream that is neither
// readable nor writable, or that is already in error (0x40), takes `_flag |= 0x20` and returns -1. A
// pending write buffer (bit 1) is flushed first: with no data bit (0x10) it is a write into a read-only
// stream (error); otherwise _ptr resets to _base. The stream then becomes write-only (`_flag =
// (_flag & ~0x10) | 2`) and a fresh buffer is allocated via __getbuf unless this is stdout/stderr and
// the fd is a terminal. With no large-buffer bit (0x108) one byte goes out through __write (0x5AFF);
// otherwise the byte is staged at _base (advancing _ptr, setting `_cnt = _bufsiz - 1`) and the bytes
// already buffered before it are written; a console record with the 0x20 bit appends (SEEK_END).
// Returns the byte when the write length matches, else `_flag |= 0x20` and -1.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_4ae8(int32_t ch, void* file, D2cIsattyFn isatty_,
                                            D2cGetbufFn getbuf, D2cWriteFn write_,
                                            D2cLseekFn lseek_) {
    D2cFile* f = (D2cFile*)file;
    uint32_t flag = (uint32_t)f->flag;
    const int32_t handle = f->file;
    if ((flag & 0x82) == 0 || (flag & 0x40) != 0) {
        f->flag = (int32_t)(flag | 0x20);
        return -1;
    }
    if ((flag & 1) != 0) {
        f->cnt = 0;
        if ((flag & 0x10) == 0) {
            f->flag = (int32_t)(flag | 0x20);
            return -1;
        }
        f->ptr = f->base;
        f->flag = (int32_t)(flag & ~1u);
    }
    flag = (uint32_t)f->flag;
    f->cnt = 0;
    f->flag = (int32_t)((flag & ~0x10u) | 2u);
    if ((flag & 0x10C) == 0) {
        const bool std_stream = (uintptr_t)file == g_std_files[0] ||
                                (uintptr_t)file == g_std_files[1];
        if (!std_stream || isatty_(handle) == 0) getbuf(file);
    }
    int32_t written;
    int32_t needed;
    if ((f->flag & 0x108) != 0) {
        char* base = f->base;
        char* p = f->ptr;
        f->ptr = base + 1;
        needed = (int32_t)(p - base);
        f->cnt = f->bufsiz - 1;
        if (needed > 0) {
            written = write_(handle, base, (uint32_t)needed);
        } else {
            const uint8_t* rec = handle == -1 ? g_console_dev : d2c_fd_record(handle);
            if ((rec[4] & 0x20) != 0) lseek_(handle, 0, 2);
            written = 0;
        }
        *f->base = (char)ch;
    } else {
        needed = 1;
        written = write_(handle, &ch, 1);
    }
    if (written == needed) return ch & 0xFF;
    f->flag |= 0x20;
    return -1;
}

// 1.13c D2Client+0x04c01 (51 B, callers 0x4C34, 0x4C58, 0x4C8F): _write_char. Unless this is a string
// stream (flag 0x40) without a base, cnt is decremented first: a negative cnt flushes through 0x4AE8
// __flsbuf, otherwise the byte lands at _ptr and _ptr advances. A -1 result stores -1 in the byte
// counter (ESI on retail) and returns; otherwise the counter advances by one.
extern "C"
void d2_client_4c01(D2cFile* f, uint8_t ch, int32_t* written, D2cIsattyFn isatty_,
                    D2cGetbufFn getbuf, D2cWriteFn write_, D2cLseekFn lseek_) {
    if ((f->flag & 0x40) == 0 || f->base != nullptr) {
        f->cnt -= 1;
        int32_t rv;
        if (f->cnt < 0) {
            rv = d2_client_4ae8((int32_t)ch, f, isatty_, getbuf, write_, lseek_);
        } else {
            *f->ptr = (char)ch;
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

// --- formatted-output string emitter -------------------------------------------------------------
typedef void (*D2cWriteCharFn)(D2cFile* file, uint8_t ch, int32_t* written);

// 1.13c D2Client+0x04c58 (55 B, caller 0x4C8F): MSVC 2003 Release CRT _write_string (output.c), the
// helper the vsnprintf-style formatter at 0x4C8F emits its runs of literal/formatted characters with.
// Register contract: EAX = &written, ECX = text, EDI = FILE, the length on the stack. A string stream
// (flag 0x40) with no buffer only adds the length to the byte count; every other stream emits the
// characters one at a time through write_char (0x4C01) and stops after the character that leaves the
// count at -1 (checked after each write, never before the first). A length below 1 writes nothing and
// leaves the count untouched. write_char (0x4C01) is a seam AFTER the retail arguments.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_4c58(int32_t* written, const char* text, D2cFile* file, int32_t len,
                                         D2cWriteCharFn write_char_) {
    if ((file->flag & 0x40) == 0 || file->base != nullptr) {
        for (; len > 0; --len) {
            write_char_(file, (uint8_t)*text++, written);
            if (*written == -1) break;
        }
    } else {
        *written += len;
    }
}
