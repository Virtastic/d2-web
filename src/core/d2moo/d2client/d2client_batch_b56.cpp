// src/core/d2client/d2client_batch_b56.cpp -- seven UNMAPPED 1.13c D2Client.dll CRT rows reimplemented
// from the decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane). Base 0x6FAB0000; every
// RVA below is retail's, and every global this file owns is a static named at its retail RVA.
//
// All seven are MSVC 2003 Release CRT bodies: __dosmaperr, __getstream, _write,
// __flush, _fclose's tail (0x5C07), _memcpy and __close_lk. Their observable is host state (a kernel file
// handle, a kernel critical section, the per-thread _ptiddata), so the unrecovered callees and the PE
// imports are host seams on one env, called in retail's call order after the retail arguments (the
// src/core/d2client/d2client_batch_1e70.cpp pattern).
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <emscripten.h>

// The 32-bit (MSVC 2003) FILE object, VS2003 layout (d2client_batch_0312a.cpp).
struct CrtFile {
    uint32_t ptr;       // 0x00
    int32_t  cnt;       // 0x04
    uint32_t base;      // 0x08
    int32_t  flag;      // 0x0C
    int32_t  file;      // 0x10
    int32_t  charbuf;   // 0x14
    int32_t  bufsiz;    // 0x18
    uint32_t tmpfname;  // 0x1C
};
static_assert(offsetof(CrtFile, cnt) == 0x04, "FILE._cnt");
static_assert(offsetof(CrtFile, base) == 0x08, "FILE._base");
static_assert(offsetof(CrtFile, flag) == 0x0C, "FILE._flag");
static_assert(offsetof(CrtFile, file) == 0x10, "FILE._file");

// __getstream's stream record: a FILE plus the CRITICAL_SECTION the allocator reserves one record past
// it (retail _malloc(0x38), EnterCriticalSection(record + 0x20)).
struct B56Stream {
    CrtFile  f;
    uint8_t  lock[0x18];
};
static_assert(sizeof(B56Stream) == 0x38, "stream record");

// +0x1239A0  DAT_6fbd39a0: the file-descriptor table (owned by d2client_batch_03f82.cpp now).
extern uint32_t g_pioinfo[0x20];
// +0x123AA0  DAT_6fbd3aa0: __piob -- the CRT stream pointer table.
// +0x124AC0  DAT_6fbd4ac0: _nstream -- its entry count.
static uint32_t g_streams[0x40];
static uint32_t g_nstream;

// __getptd's env (d2client_batch_908f.cpp defines d2_client_72f7); the shape is repeated so the TUs
// agree at the C ABI (function pointers only).
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

// Callees and imports this file reaches, address = retail RVA (IAT slots are imports).
struct D2ClientBatchB56Env {
    void     (*lock_1)();                                                   // 0x4860 __lock(1)
    void     (*unlock_1)();                                                 // 0x593A _unlock(1)
    void     (*lock_file2)(int32_t, void*);                                 // 0x55BF
    void     (*unlock_file2)(int32_t, void*);                               // 0x5611
    int32_t  (*mtinitlocknum)(int32_t);                                     // 0x47C0
    void*    (*malloc_)(uint32_t);                                          // 0x7D5F _malloc
    void     (*free_)(void*);                                               // 0x4891 _free
    int32_t  (*init_cs)(void*, uint32_t);                                   // 0x908F ___crtInitCritSecAndSpinCount
    void     (*enter_cs)(void*);                                            // IAT EnterCriticalSection
    void     (*leave_cs)(void*);                                            // IAT LeaveCriticalSection
    int32_t  (*write_)(uint32_t fd, const void* buf, uint32_t n);           // 0x5AFF __write
    uint32_t (*commit_)(uint32_t fd);                                       // 0xA8B9 _commit
    int32_t  (*lseeki64_lk)(uint32_t fd, uint32_t lo, uint32_t hi, uint32_t origin); // 0xA4BC
    uint32_t (*write_file)(uint32_t h, const void* buf, uint32_t n, uint32_t* written, uint32_t ov); // IAT
    uint32_t (*get_last_error)();                                           // IAT GetLastError
    uint32_t (*close_handle)(uint32_t h);                                   // IAT CloseHandle
    uint32_t (*get_osfhandle)(uint32_t fd);                                 // 0xA63A __get_osfhandle
    void     (*free_osfhnd)(uint32_t fd);                                   // 0xA5BB __free_osfhnd
    void     (*lock_fhandle)(uint32_t fd);                                  // 0xA67B __lock_fhandle
    void     (*unlock_fhandle)();                                           // 0x6157 __unlock_fhandle
    void     (*free_lconv_mon)(uint32_t lconv);                             // 0xAEE4 ___free_lconv_mon
    void     (*free_lconv_num)(uint32_t lconv);                             // 0xACF5 ___free_lconv_num
    void     (*free_lc_time)(uint32_t lc_time);                             // 0xAE85 ___free_lc_time
    int32_t  (*crt_get_string_type_a)(uint32_t lcid, const void* src, int32_t len, uint16_t* out,
                                      uint32_t src2, int32_t code_page, uint32_t err); // 0xAB3B
    const D2cGetptdEnv* ptd;                                                // 0x72F7 env, for __dosmaperr
};

static uint8_t* fd_record(uint32_t fd) {
    return (uint8_t*)(uintptr_t)(g_pioinfo[fd >> 5] + (fd & 0x1F) * 0x24);
}
static uint32_t* ptd_errno(const D2cGetptdEnv* env) {
    return (uint32_t*)((uint8_t*)d2_client_72f7(env) + 8);      // _ptiddata._terrno
}
static uint32_t* ptd_doserrno(const D2cGetptdEnv* env) {
    return (uint32_t*)((uint8_t*)d2_client_72f7(env) + 0xC);    // _ptiddata._tdoserrno
}

// +0x123990  DAT_6fbd3990: _nfile -- the number of valid descriptors.
uint32_t g_fdcount; // Shared with __read (0x08383): one retail _nfile global.
// Locale defaults compared by pointer in ___freetlocinfo (+0x123960/+0x123964/+0x123968,
// +0x0FAABE8/+0x0FAABEC) and the initial-locale sentinel its caller checks (+0x0DB5E8).
static uint32_t g_loc[5];
uint8_t g_initial_locale;           // +0x0DB5E8, only its address is compared (also read by batch_7368)

// +0x0DB63C  PTR_DAT_6fb8b63c: the CRT default locale pointer (shared with d2client_batch_03f82.cpp).
extern uint32_t g_default_locale;

// Scenario setup: the CRT's stream table (__piob / _nstream), _nfile and the locale defaults.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_streams_pin(uint32_t count, uint32_t table) {
    g_nstream = count < 0x40 ? count : 0x40;
    for (uint32_t i = 0; i < g_nstream; ++i)
        g_streams[i] = table ? *(const uint32_t*)(uintptr_t)(table + i * 4) : 0;
}
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_fdcount_pin(uint32_t ndesc) { g_fdcount = ndesc; }
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_locale_defaults_pin(uint32_t a3c, uint32_t a34, uint32_t a30,
                                                        uint32_t a40, uint32_t a50) {
    g_loc[0] = a3c; g_loc[1] = a34; g_loc[2] = a30; g_loc[3] = a40; g_loc[4] = a50;
}

// The 0x2D {dos error, errno} pairs at +0x0DB478 DAT_6fb8b478 (MSVC 2003 release CRT).
static const uint32_t kDosErrMap[0x2D][2] = {
    {0x001, 0x16}, {0x002, 0x02}, {0x003, 0x02}, {0x004, 0x18}, {0x005, 0x0D},
    {0x006, 0x09}, {0x007, 0x0C}, {0x008, 0x0C}, {0x009, 0x0C}, {0x00A, 0x07},
    {0x00B, 0x08}, {0x00C, 0x16}, {0x00D, 0x16}, {0x00F, 0x02}, {0x010, 0x0D},
    {0x011, 0x12}, {0x012, 0x02}, {0x021, 0x0D}, {0x035, 0x02}, {0x041, 0x0D},
    {0x043, 0x02}, {0x050, 0x11}, {0x052, 0x0D}, {0x053, 0x0D}, {0x057, 0x16},
    {0x059, 0x0B}, {0x06C, 0x0D}, {0x06D, 0x20}, {0x070, 0x1C}, {0x072, 0x09},
    {0x006, 0x16}, {0x080, 0x0A}, {0x081, 0x0A}, {0x082, 0x09}, {0x083, 0x16},
    {0x084, 0x0D}, {0x091, 0x29}, {0x09E, 0x0D}, {0x0A1, 0x02}, {0x0A4, 0x0B},
    {0x0A7, 0x0D}, {0x0B7, 0x11}, {0x0CE, 0x02}, {0x0D7, 0x0B}, {0x718, 0x0C},
};

// 1.13c D2Client+0x057AE
// __dosmaperr(doserr): store the DOS error in _ptiddata._tdoserrno, then map it to _terrno through the
// table above; otherwise 0xD (EACCES) for 0x12 < e < 0x25, 8 (ENOEXEC) for 0xBB < e < 0xCB, else 0x16
// (EINVAL). Retail re-reads the thread block before each store; the calls keep that order.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_57ae(uint32_t doserr, const D2cGetptdEnv* env) {
    *ptd_doserrno(env) = doserr;
    for (uint32_t i = 0; i < 0x2D; ++i) {
        if (kDosErrMap[i][0] == doserr) {
            *ptd_errno(env) = kDosErrMap[i][1];
            return;
        }
    }
    uint32_t mapped;
    if (doserr > 0x12 && doserr < 0x25) mapped = 0xD;
    else if (doserr > 0xBB && doserr < 0xCB) mapped = 8;
    else mapped = 0x16;
    *ptd_errno(env) = mapped;
}

// 1.13c D2Client+0x05821
// __getstream: walk __piob/_nstream under CRT lock 1. An empty slot gets a 0x38-byte record whose
// critical section is initialised with spin 4000; a free stream is reused when _flag & 0x83 is clear
// after taking its per-stream lock (indexes 3..0x13 first ensure CRT lock index+0x10 exists). The
// chosen stream is zeroed with _file = -1. Lock 1 is released on every exit.
extern "C"
EMSCRIPTEN_KEEPALIVE CrtFile* d2_client_5821(const D2ClientBatchB56Env* env) {
    B56Stream* file = nullptr;
    env->lock_1();
    for (uint32_t i = 0;; ++i) {
        if (i >= g_nstream) break;
        uint32_t entry = g_streams[i];
        if (entry == 0) {
            void* mem = env->malloc_(0x38);
            g_streams[i] = (uint32_t)(uintptr_t)mem;
            if (mem != nullptr) {
                if (env->init_cs((uint8_t*)mem + 0x20, 4000) == 0) {
                    env->free_(mem);
                    g_streams[i] = 0;
                } else {
                    env->enter_cs((uint8_t*)mem + 0x20);
                    file = (B56Stream*)mem;
                }
            }
            break;
        }
        CrtFile* f = (CrtFile*)(uintptr_t)entry;
        if ((f->flag & 0x83) == 0) {
            if (i > 2 && i < 0x14 && env->mtinitlocknum((int32_t)(i + 0x10)) == 0) break;
            env->lock_file2((int32_t)i, f);
            f = (CrtFile*)(uintptr_t)g_streams[i];
            if ((f->flag & 0x83) == 0) { file = (B56Stream*)f; break; }
            env->unlock_file2((int32_t)i, f);
        }
    }
    if (file != nullptr) {
        file->f.cnt = 0;
        file->f.flag = 0;
        file->f.base = 0;
        file->f.ptr = 0;
        file->f.tmpfname = 0;
        file->f.file = -1;
    }
    env->unlock_1();
    return file ? &file->f : nullptr;
}

// 1.13c D2Client+0x05943
// _write(fd, buf, count): in append mode first seek to end. Binary descriptors (device flags & 0x80
// clear) pass the buffer straight to WriteFile; text descriptors translate '\n' to "\r\n" in 0x400-byte
// chunks. A partial write returns written - newlines. zero bytes written is an error: 5 (ERROR_ACCESS_
// DENIED) latches errno 9 / doserrno 5, any other DOS error goes through __dosmaperr; otherwise a
// console stream whose first byte is 0x1A is EOF (return 0) and everything else is errno 0x1C.
// Retail's stack cookie (DAT_6fb8b780) guards a stack buffer that is never read; it is not modelled.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_5943(uint32_t fd, const char* buf, uint32_t count,
                                            const D2ClientBatchB56Env* env) {
    if (count == 0) return 0;
    uint8_t* rec = fd_record(fd);
    if ((*(uint8_t*)(rec + 4) & 0x20) != 0) env->lseeki64_lk(fd, 0, 0, 2);

    uint32_t written = 0;      // local_41c
    uint32_t newlines = 0;     // local_420
    uint32_t doserr = 0;       // local_410
    if ((*(uint8_t*)(rec + 4) & 0x80) == 0) {
        uint32_t n = 0;
        if (env->write_file(*(uint32_t*)rec, buf, count, &n, 0) == 0) {
            doserr = env->get_last_error();
        } else {
            written = n;
        }
    } else {
        const char* cur = buf;             // local_418
        char chunk[0x404];                 // local_40c + local_3a8
        for (;;) {
            uint32_t consumed = (uint32_t)(cur - buf);   // uVar7
            uint32_t len = 0;                            // local_414 / pcVar3 - chunk
            while (consumed < count && len < 0x400) {
                char c = *cur++;
                consumed++;
                if (c == '\n') { newlines++; chunk[len++] = '\r'; }
                chunk[len++] = c;
            }
            uint32_t n = 0;
            if (env->write_file(*(uint32_t*)rec, chunk, len, &n, 0) == 0) {
                doserr = env->get_last_error();
                break;
            }
            written += n;
            if (len > n || (uint32_t)(cur - buf) >= count) break;
        }
    }
    if (written != 0) return (int32_t)(written - newlines);
    if (doserr != 0) {
        if (doserr == 5) {
            *ptd_errno(env->ptd) = 9;
            *ptd_doserrno(env->ptd) = 5;
            return -1;
        }
        d2_client_57ae(doserr, env->ptd);
        return -1;
    }
    if ((*(uint8_t*)(rec + 4) & 0x40) != 0 && *(const uint8_t*)buf == 0x1A) return 0;
    *ptd_errno(env->ptd) = 0x1C;
    *ptd_doserrno(env->ptd) = 0;
    return -1;
}

// 1.13c D2Client+0x05baa
// __flush: a write stream (flag & 3 == 2) with a buffer (flag & 0x108) writes _ptr - _base bytes;
// a short write sets the error bit 0x20 and returns -1, otherwise the 0x80 clear-bit is dropped. _cnt
// is reset and _ptr rewound to _base on every path.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_5baa(CrtFile* file, const D2ClientBatchB56Env* env) {
    int32_t rc = 0;
    if (((uint8_t)file->flag & 3) == 2 && (file->flag & 0x108) != 0) {
        uint32_t n = file->ptr - file->base;
        if ((int32_t)n > 0) {
            uint32_t w = (uint32_t)env->write_((uint32_t)file->file, (const void*)(uintptr_t)file->base, n);
            if (w == n) {
                if ((char)file->flag < 0) file->flag &= ~2;
            } else {
                file->flag |= 0x20;
                rc = -1;
            }
        }
    }
    file->cnt = 0;
    file->ptr = file->base;
    return rc;
}

// 1.13c D2Client+0x05C07
// The tail after __flush in _fclose: a failed flush is -1; a stream flagged 0x4000 then commits its
// descriptor (_commit, 0xA8B9) and maps a failure to -1.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_5c07(CrtFile* file, const D2ClientBatchB56Env* env) {
    if (d2_client_5baa(file, env) != 0) return -1;
    if ((file->flag & 0x4000) != 0)
        return (int32_t)(0u - (uint32_t)(env->commit_((uint32_t)file->file) != 0));
    return 0;
}

// 1.13c D2Client+0x05d20
// _memcpy: the release CRT's overlapping-aware copy -- memmove semantics, byte-identical result.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_5d20(void* dst, const void* src, uint32_t size) {
    return std::memmove(dst, src, size);
}

// 1.13c D2Client+0x0605D
// __close_lk(fd): close the descriptor's OS handle unless fd is 1 or 2 and shares stdout's handle; a
// failed CloseHandle keeps its DOS error. The descriptor record is then freed and zeroed, and a DOS
// error maps through __dosmaperr and returns -1.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_605d(uint32_t fd, const D2ClientBatchB56Env* env) {
    uint32_t doserr = 0;
    if ((int32_t)env->get_osfhandle(fd) != -1) {
        bool skip = false;
        if (fd == 1 || fd == 2) {
            if (env->get_osfhandle(2) == env->get_osfhandle(1)) skip = true;
        }
        if (!skip && env->close_handle(env->get_osfhandle(fd)) == 0)
            doserr = env->get_last_error();
    }
    env->free_osfhnd(fd);
    *(uint8_t*)(fd_record(fd) + 4) = 0;
    if (doserr == 0) return 0;
    d2_client_57ae(doserr, env->ptd);
    return -1;
}

// 1.13c D2Client+0x060E0  (test/d2client-60e0.test.mjs)
// __close(fd): a descriptor outside _nfile or without the in-use bit (record +4 & 1) latches errno 9
// and doserrno 0. Otherwise the fhandle lock is taken, the bit re-tested, __close_lk (0x605D) runs, and
// the lock is released.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_60e0(uint32_t fd, const D2ClientBatchB56Env* env) {
    if (fd < g_fdcount) {
        uint8_t* rec = fd_record(fd);
        if ((*(uint8_t*)(rec + 4) & 1) != 0) {
            env->lock_fhandle(fd);
            int32_t rc;
            if ((*(uint8_t*)(rec + 4) & 1) == 0) {
                *ptd_errno(env->ptd) = 9;
                rc = -1;
            } else {
                rc = d2_client_605d(fd, env);
            }
            env->unlock_fhandle();
            return rc;
        }
    }
    *ptd_errno(env->ptd) = 9;
    *ptd_doserrno(env->ptd) = 0;
    return -1;
}

// 1.13c D2Client+0x0617b
// __freebuf: only a stream with a buffer (flag & 0x83 and flag & 8) frees _base, clears the _IOMYBUF
// (8) and _IOYOURBUF (0x1000 -> 0xF7FF mask) bits -- a 16-bit store, so only the low half of _flag is
// written -- and rewinds _ptr/_base/_cnt.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_617b(CrtFile* file, const D2ClientBatchB56Env* env) {
    if ((file->flag & 0x83) != 0 && (file->flag & 8) != 0) {
        env->free_((void*)(uintptr_t)file->base);
        *(uint16_t*)&file->flag = (uint16_t)((uint16_t)file->flag & 0xFBF7u);
        file->ptr = 0;
        file->base = 0;
        file->cnt = 0;
    }
}

// 1.13c D2Client+0x0707b
// ___isctype_mt(self, locale, c, mask): a byte in [0, 0x100] is classified from the locale's 16-bit
// ctype table at locale+0x48. Anything else is converted through ___crtGetStringTypeA: a two-byte
// sequence only when the high byte's table entry has the MBCS lead-bit 0x80; the API's output word is
// then masked.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_707b(void* self, void* locale, int32_t c, uint32_t mask,
                                             const D2ClientBatchB56Env* env) {
    (void)self;
    const uint8_t* loc = (const uint8_t*)locale;
    const uint8_t* table = (const uint8_t*)(uintptr_t)*(const uint32_t*)(loc + 0x48);
    uint16_t type;
    if ((uint32_t)(c + 1) < 0x101u) {
        type = *(const uint16_t*)(table + (intptr_t)c * 2);
    } else {
        int32_t len;
        uint8_t src[2];
        if ((table[1 + ((uint32_t)c >> 8 & 0xFF) * 2] & 0x80) == 0) {
            src[0] = (uint8_t)c;
            len = 1;
        } else {
            src[0] = (uint8_t)((uint32_t)c >> 8);  // CRT: lead byte first
            src[1] = (uint8_t)c;
            len = 2;
        }
        type = 0;
        if (env->crt_get_string_type_a(1, src, len, &type, *(const uint16_t*)(loc + 4),
                                       *(const int32_t*)(loc + 0x14), 1) == 0)
            return 0;
    }
    return (uint32_t)type & mask;
}

// 1.13c D2Client+0x070f2
// ___freetlocinfo(locale): release a locale block's referenced tables when their refcount is zero and
// they are not the CRT defaults: lconv (three tables at +0x34/+0x30/+0x2c keyed off +0x3c), the
// codepage buffer at +0x40 with its +0x44 base, the lc_time block at +0x50, and the block itself. Each
// table that goes also runs its own deep free (monetary/numeric/time).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_70f2(uint8_t* locale, const D2ClientBatchB56Env* env) {
    uint32_t p2c = *(uint32_t*)(locale + 0x2C);
    uint32_t p3c = *(uint32_t*)(locale + 0x3C);
    if (p3c != g_loc[0] && p3c != 0 && *(uint32_t*)(uintptr_t)p2c == 0) {
        uint32_t p34 = *(uint32_t*)(locale + 0x34);
        if (p34 != 0 && *(uint32_t*)(uintptr_t)p34 == 0 && p34 != g_loc[1]) {
            env->free_((void*)(uintptr_t)p34);
            env->free_lconv_mon(p3c);
        }
        uint32_t p30 = *(uint32_t*)(locale + 0x30);
        if (p30 != 0 && *(uint32_t*)(uintptr_t)p30 == 0 && p30 != g_loc[2]) {
            env->free_((void*)(uintptr_t)p30);
            env->free_lconv_num(p3c);
        }
        env->free_((void*)(uintptr_t)p2c);
        env->free_((void*)(uintptr_t)p3c);
    }
    uint32_t p40 = *(uint32_t*)(locale + 0x40);
    if (p40 != g_loc[3] && p40 != 0 && *(uint32_t*)(uintptr_t)p40 == 0) {
        env->free_((void*)(uintptr_t)p40);
        env->free_((void*)(uintptr_t)(*(uint32_t*)(locale + 0x44) - 0xFE));
    }
    uint32_t p50 = *(uint32_t*)(locale + 0x50);
    if (p50 != g_loc[4] && p50 != 0 && *(uint32_t*)(uintptr_t)(p50 + 0x2D * 4) == 0) {
        env->free_lc_time(p50);
        env->free_((void*)(uintptr_t)p50);
    }
    env->free_(locale);
}

// 1.13c D2Client+0x071C2
// Pinned by test/d2client-71c2.test.mjs (decomp field indices 0xb/0xd/0xc/0x10 -> +0x2C/+0x34/+0x30/+0x40).
// ___updatetlocinfo_lk: release the thread's current locale (refcounts of its lconv tables, the time
// block's +0xB4 count) when it is not the default, install the default and bump its refcounts, then
// deep-free the old block if it is now unreferenced and not the initial locale. Returns the installed
// locale pointer.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_71c2(const D2ClientBatchB56Env* env) {
    uint8_t* ptd = (uint8_t*)d2_client_72f7(env->ptd);
    uint32_t* old = (uint32_t*)(uintptr_t)*(uint32_t*)(ptd + 0x64);
    if (old != (uint32_t*)(uintptr_t)g_default_locale) {
        if (old != nullptr) {
            old[0] -= 1;
            for (uint32_t off : {0x2C, 0x34, 0x30, 0x40}) {
                uint32_t* r = (uint32_t*)(uintptr_t)old[off / 4];
                if (r != nullptr) *r -= 1;
            }
            uint32_t t = old[0x4C / 4];
            *(uint32_t*)(uintptr_t)(t + 0xB4) -= 1;
        }
        uint32_t* def = (uint32_t*)(uintptr_t)g_default_locale;
        *(uint32_t*)(ptd + 0x64) = g_default_locale;
        def[0] += 1;
        for (uint32_t off : {0x2C, 0x34, 0x30, 0x40}) {
            uint32_t r = def[off / 4];
            if (r != 0) *(uint32_t*)(uintptr_t)r += 1;
        }
        *(uint32_t*)(uintptr_t)(def[0x4C / 4] + 0xB4) += 1;
        if (old != nullptr && old[0] == 0 && old != (uint32_t*)(uintptr_t)&g_initial_locale)
            d2_client_70f2((uint8_t*)old, env);
    }
    return *(uint32_t*)(ptd + 0x64);
}
