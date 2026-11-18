// src/core/d2client/d2client_batch_0312a.cpp -- 13 UNMAPPED 1.13c D2Client.dll rows reimplemented from
// the cached decompiles (build-wasm/ghidra-d2client/decomp/0x<RVA>.c), in RVA order. Alpha batch lane
// (OBJECTIVE v8.3): implementation + marker only, no spec, no vector. Base 0x6FAB0000; every RVA below
// is retail's, and every global this file owns is a static at its retail RVA comment.
//
// All thirteen are MSVC 2003 Release CRT glue the linker pulled into the DLL: the
// type_info deleting destructor, _sprintf/__fsopen/__openfile/_fwrite/__fclose_lk and their lock/SEH
// funclets. Unrecovered callees are host seams on D2ClientBatch312AEnv, after the retail arguments,
// which keep their order (the src/core/d2client/d2client_batch_1e70.cpp pattern). The callees already
// recovered elsewhere are called directly: 0x47AB _unlock (batch_3664), 0x579C _errno (batch_4857),
// 0x35A1 doexit (crt_doexit_35a1.cpp), and this file's own 0x36E7/0x3704/0x384D/0x5634.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>
#include "client_host.h"

// The 32-bit (MSVC 2003) FILE object, VS2003 layout, from the disassembly of _sprintf (0x3146): the
// local at ebp-0x20 has _ptr at +0x00, _cnt at +0x04, _base at +0x08 and _flag at +0x0C.
struct CrtFile {
    uint32_t _ptr;      // 0x00
    int32_t  _cnt;      // 0x04
    uint32_t _base;     // 0x08
    int32_t  _flag;     // 0x0C
    int32_t  _file;     // 0x10
    int32_t  _charbuf;  // 0x14
    int32_t  _bufsiz;   // 0x18
    uint32_t _tmpfname; // 0x1C
};

// Host seams for the callees this batch does not implement (address = retail RVA).
struct D2ClientBatch312AEnv {
    void*    (*typeinfo_dtor)(void*);                                   // 0x30E4 ~type_info
    void     (*oper_delete)(void*);                                     // 0x1000 sub_1000 operator delete
    int32_t  (*output)(CrtFile*, const uint8_t*, const uint32_t*);      // 0x4C8F _output
    int32_t  (*flsbuf)(int32_t, CrtFile*);                              // 0x4AE8 __flsbuf
    void     (*lock)(int32_t);                                          // 0x4860 __lock
    void     (*fpmath)(int32_t);                                        // 0xCACA4 __fpmath
    int32_t  (*invoke)(uint32_t);                                       // one C initialiser table entry
    int32_t  (*atexit_)(uint32_t);                                      // 0x3F82 _atexit
    void*    (*getstream)(void);                                        // 0x5821 __getstream
    int32_t  (*sopen)(const char*, uint32_t, int32_t, int32_t);         // 0xA467 __sopen (variadic pmode)
    void     (*lock_file)(CrtFile*);                                    // 0x5590 __lock_file
    void     (*unlock_file)(CrtFile*);                                  // 0x55E2 __unlock_file
    int32_t  (*flush)(CrtFile*);                                        // 0x5BAA __flush
    int32_t  (*write)(int32_t, const void*, uint32_t);                  // 0x5AFF __write
    void*    (*memcpy_)(void*, const void*, uint32_t);                  // 0x5D20 _memcpy
    void     (*freebuf)(CrtFile*);                                      // 0x617B __freebuf
    int32_t  (*close_)(int32_t);                                        // 0x60E0 __close
    void     (*free_)(void*);                                           // 0x4891 _free
};

// Callees recovered elsewhere in the tree.
extern "C" void    d2_client_47ab(uint32_t idx);                                     // 0x47AB _unlock
extern "C" int32_t* d2_client_579c(void);                                            // 0x579C _errno
extern "C" void    d2_client_35a1(uint32_t code, uint32_t quick, uint32_t returning,
                                  D2cHost h);                                        // 0x35A1 doexit

// Retail module state, all zero-initialised. The two initialiser tables are filled by the loader in the
// shipped image (retail +0x0DB064, 5 entries, and +0x0DB000, 24 entries); the walk is the rule here and
// each non-NULL entry goes through the invoke seam.
static uint32_t g_xcInit[5];    // +0x0DB064  __xc_a .. __xc_z
static uint32_t g_xiInit[24];   // +0x0DB000  __xi_a .. __xi_z
uint32_t g_streamCount;         // +0x0FAA50  _DAT_6fbaaa50: bumped once in __openfile and again in
                                //            __getbuf 0x9F56 (d2client_batch_03f82.cpp); never read
static uint32_t g_fileFlagBase; // +0x0FABDC  DAT_6fbaabdc: ORed into every new stream's _flag at entry.
                                //            Shipped 0 and never stored anywhere in the image, so it is
                                //            constant 0; only reader in the DLL is this function.

// 1.13c D2Client+0x0312A: `type_info::scalar deleting destructor'. __thiscall: this in ECX, the delete
// flag on the stack. Destroy (0x30E4), and if bit 0 of the flag is set release the object (0x1000);
// the object pointer is returned either way.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_312a(void* self, uint32_t param_1,
                                          const D2ClientBatch312AEnv* env) {
    env->typeinfo_dtor(self);
    if ((param_1 & 1) != 0) env->oper_delete(self);
    return self;
}

// 1.13c D2Client+0x03146 (_sprintf): format into a string-backed FILE. The frame object carries
// _cnt = 0x7FFFFFFF, _flag = 0x42 (_IOSTRG|_IOWRT), and _base = _ptr = dest; the vararg list is the
// address of the first variadic word. When dest is non-NULL the terminator is appended (through
// __flsbuf only in the unreachable count-underflow case). Returns __output's count.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_3146(char* dest, const uint8_t* format,
                                            const uint32_t* args,
                                            const D2ClientBatch312AEnv* env) {
    CrtFile local = {};
    local._cnt = 0x7FFFFFFF;
    local._flag = 0x42;
    local._base = (uint32_t)(uintptr_t)dest;
    local._ptr = local._base;
    int32_t n = env->output(&local, format, args);
    if (dest != nullptr) {
        local._cnt -= 1;
        if (local._cnt < 0) env->flsbuf(0, &local);
        else *(uint8_t*)(uintptr_t)local._ptr = 0;
    }
    return n;
}

// 1.13c D2Client+0x0350D: `push 8; call 0x4860` -- acquire CRT lock 8.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_350d(const D2ClientBatch312AEnv* env) {
    env->lock(8);
}

// 1.13c D2Client+0x03516: `push 8; call 0x47AB` -- release CRT lock 8, via the recovered _unlock.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_3516(void) {
    d2_client_47ab(8);
}

// 1.13c D2Client+0x03537 (__cinit): if the __fpmath pointer (retail +0x0FA588, non-NULL in the image)
// is present, call it with the floating-point precision argument. Then walk __xc_a..__xc_z (5 entries):
// the first non-NULL entry returning non-zero stops the walk and is the result. On success register the
// onexit funclet 0x548F, walk __xi_a..__xi_z (24 entries) calling every non-NULL entry and ignoring its
// result, and return 0.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_3537(int32_t param_1, const D2ClientBatch312AEnv* env) {
    if (env->fpmath != nullptr) env->fpmath(param_1);
    int32_t ret = 0;
    for (uint32_t i = 0; i < 5; ++i) {
        if (ret != 0) return ret;
        if (g_xcInit[i] != 0) ret = env->invoke(g_xcInit[i]);
    }
    if (ret == 0) {
        env->atexit_(0x548F);                       // _atexit(&LAB_6fab548f)
        for (uint32_t i = 0; i < 24; ++i)
            if (g_xiInit[i] != 0) env->invoke(g_xiInit[i]);
        ret = 0;
    }
    return ret;
}

// 1.13c D2Client+0x3686 (__cexit): doexit(0, 0, returning=1).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_3686(D2cHost h) {
    d2_client_35a1(0, 0, 1, h);
}

// 0x36E7 and 0x384D are SEH funclets inside __fsopen and _fwrite: their operand is a slot of the
// caller's frame (EBP-0x1C and EBP+0x14), not an argument list. Per the batch_3664 0x3650 precedent the
// slot's value becomes the argument; both slots hold the stream being unlocked.
extern "C" void d2_client_36e7(CrtFile* file, const D2ClientBatch312AEnv* env);
extern "C" void* d2_client_5634(const char* filename, const char* mode, int32_t shflag,
                                CrtFile* file, const D2ClientBatch312AEnv* env);

// 1.13c D2Client+0x03695 (__fsopen): get a stream (__getstream); on failure store errno = 0x18 (ENFILE)
// and return NULL. Otherwise bind it to the file with __openfile(name, mode, shflag, stream), release
// the stream lock through the 0x36E7 funclet, and return __openfile's result.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_3695(const char* filename, const char* mode, int32_t shflag,
                                          const D2ClientBatch312AEnv* env) {
    CrtFile* stream = (CrtFile*)env->getstream();
    if (stream == nullptr) {
        *d2_client_579c() = 0x18;
        return nullptr;
    }
    CrtFile* f = (CrtFile*)d2_client_5634(filename, mode, shflag, stream, env);
    d2_client_36e7(stream, env);
    return f;
}

// 1.13c D2Client+0x036E7: __unlock_file(*(FILE**)(caller EBP - 0x1C)) -- release the stream lock.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_36e7(CrtFile* file, const D2ClientBatch312AEnv* env) {
    env->unlock_file(file);
}

// 1.13c D2Client+0x03704 (__fwrite_lk): buffered write of size*count bytes, caller holds the stream
// lock. The decompile's rule: total = size*count (32-bit); zero writes nothing and returns count as 0.
// Chunk size is 0x1000 for an unbuffered stream and _bufsiz otherwise. Each pass either copies into the
// stream buffer (when _flag & 0x108 and _cnt > 0), or -- with no room -- writes straight through: flush
// first on a buffered stream, then __write in whole chunk multiples when the remaining bytes reach a
// chunk, else one byte through __flsbuf. A short/failed write sets the error flag and stops. Returns the
// number of whole items (size bytes each) written.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_3704(const uint8_t* p, uint32_t size, uint32_t count,
                                             CrtFile* f, const D2ClientBatch312AEnv* env) {
    uint32_t total = size * count;
    if (total == 0) return 0;
    uint32_t remaining = total;
    uint32_t chunk = ((f->_flag & 0x10C) == 0) ? 0x1000u : (uint32_t)f->_bufsiz;
    for (;;) {
        uint32_t buffered = (uint32_t)f->_flag & 0x108;
        uint32_t cnt = (uint32_t)f->_cnt;
        if (buffered == 0 || cnt == 0) {
            if (chunk <= remaining) {
                if (buffered != 0 && env->flush(f) != 0) break;
                uint32_t n = remaining;
                if (chunk != 0) n = remaining - remaining % chunk;
                int32_t w = env->write(f->_file, p, n);
                if (w == -1 || (remaining -= (uint32_t)w, (uint32_t)w < n)) {
                    f->_flag |= 0x20;
                    break;
                }
                p += (uint32_t)w;
            } else {
                if (env->flsbuf((int32_t)(int8_t)*p, f) == -1) break;
                ++p;
                chunk = (uint32_t)f->_bufsiz;
                --remaining;
                if ((int32_t)chunk < 1) chunk = 1;
            }
        } else {
            uint32_t n = (cnt <= remaining) ? cnt : remaining;
            env->memcpy_((void*)(uintptr_t)f->_ptr, p, n);
            f->_cnt -= (int32_t)n;
            f->_ptr += n;
            remaining -= n;
            p += n;
        }
        if (remaining == 0) return count;
    }
    return (total - remaining) / size;
}

// 1.13c D2Client+0x0380b (_fwrite): lock the stream (0x5590), run __fwrite_lk, release the stream
// through the 0x384D funclet ([EBP+0x14] = the FILE argument), return __fwrite_lk's count.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_380b(const uint8_t* str, uint32_t size, uint32_t count,
                                             CrtFile* file, const D2ClientBatch312AEnv* env);

extern "C" void d2_client_384d(CrtFile* file, const D2ClientBatch312AEnv* env);

extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_380b(const uint8_t* str, uint32_t size, uint32_t count,
                                             CrtFile* file, const D2ClientBatch312AEnv* env) {
    env->lock_file(file);
    uint32_t n = d2_client_3704(str, size, count, file, env);
    d2_client_384d(file, env);
    return n;
}

// 1.13c D2Client+0x0384d: __unlock_file(*(FILE**)(caller EBP + 0x14)) -- the _fwrite funclet.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_384d(CrtFile* file, const D2ClientBatch312AEnv* env) {
    env->unlock_file(file);
}

// 1.13c D2Client+0x03857 (__fclose_lk): caller holds the stream lock. Only a stream with any of the
// 0x83 flags closes: flush, free the buffer, close the fd -- a negative close forces -1 -- and, when the
// close succeeded, free a temporary name and clear it. _flag is cleared on every path; returns the
// flush result or -1.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_3857(CrtFile* f, const D2ClientBatch312AEnv* env) {
    int32_t rc = -1;
    if ((f->_flag & 0x83) != 0) {
        rc = env->flush(f);
        env->freebuf(f);
        int32_t closed = env->close_(f->_file);
        if (closed < 0) {
            rc = -1;
        } else if (f->_tmpfname != 0) {
            env->free_((void*)(uintptr_t)f->_tmpfname);
            f->_tmpfname = 0;
        }
    }
    f->_flag = 0;
    return rc;
}

// 1.13c D2Client+0x05634 (__openfile): bind an already-allocated stream to a file. The first mode
// character selects the open flags and the base stream flag (_IOREAD 1 / _IOWRT 2) ORed with the module
// word at +0x0FABDC; anything else fails with NULL. Each following character may set one flag, and a
// duplicate or a conflicting one (one of R/S, one of c/n, one of b/t) stops the scan silently -- the
// remaining mode text is ignored, not an error:
//   T _O_SHORT_LIVED 0x1000 | D _O_TEMPORARY 0x40 | + _O_RDWR (clears _O_RDONLY, sets _IORW 0x80)
//   R _O_RANDOM 0x10 | S _O_SEQUENTIAL 0x20    (mutually exclusive)
//   b _O_BINARY 0x8000 | t _O_TEXT 0x4000      (mutually exclusive)
//   c _IOCOMMIT 0x4000 in _flag | n clears it  (mutually exclusive)
// Then __sopen(name, openflag, shflag, _S_IREAD|_S_IWRITE = 0x1A4). A negative descriptor returns NULL;
// otherwise bump +0x0FAA50 and fill the stream: _flag, _cnt = _ptr = _base = _tmpfname = 0, _file = fd.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_5634(const char* filename, const char* mode, int32_t shflag,
                                          CrtFile* file, const D2ClientBatch312AEnv* env) {
    uint32_t openflag;
    uint32_t flag;
    char c = *mode;
    if (c == 'a') {
        openflag = 0x109;                           // _O_WRONLY|_O_CREAT|_O_APPEND
        flag = g_fileFlagBase | 2;                  // _IOWRT
    } else if (c == 'r') {
        openflag = 0;                               // _O_RDONLY
        flag = g_fileFlagBase | 1;                  // _IOREAD
    } else if (c == 'w') {
        openflag = 0x301;                           // _O_WRONLY|_O_CREAT|_O_TRUNC
        flag = g_fileFlagBase | 2;                  // _IOWRT
    } else {
        return nullptr;
    }
    bool seen_rs = false;                           // retail's shared R/S latch
    bool seen_cn = false;                           // retail's shared c/n latch
    for (++mode; *mode != '\0'; ++mode) {
        c = *mode;
        bool valid = true;
        if (c == 'T') {
            if ((openflag & 0x1000) != 0) valid = false;
            else openflag |= 0x1000;
        } else if (c == '+') {
            if ((openflag & 2) != 0) valid = false;
            else {
                openflag = (openflag & 0xFFFFFFFEu) | 2u;
                flag = (flag & 0xFFFFFFFCu) | 0x80u;
            }
        } else if (c == 'D') {
            if ((openflag & 0x40) != 0) valid = false;
            else openflag |= 0x40;
        } else if (c == 'R') {
            if (seen_rs) valid = false;
            else { seen_rs = true; openflag |= 0x10; }
        } else if (c == 'S') {
            if (seen_rs) valid = false;
            else { seen_rs = true; openflag |= 0x20; }
        } else if (c == 'b') {
            if ((openflag & 0xC000) != 0) valid = false;
            else openflag |= 0x8000;
        } else if (c == 'c') {
            if (seen_cn) valid = false;
            else { seen_cn = true; flag |= 0x4000; }
        } else if (c == 'n') {
            if (seen_cn) valid = false;
            else { seen_cn = true; flag &= 0xFFFFBFFFu; }
        } else if (c == 't') {
            if ((openflag & 0xC000) != 0) valid = false;
            else openflag |= 0x4000;
        } else {
            valid = false;                          // unknown character: stop the scan
        }
        if (!valid) break;
    }
    int32_t fd = env->sopen(filename, openflag, shflag, 0x1A4);
    if (fd < 0) return nullptr;
    ++g_streamCount;
    file->_flag = (int32_t)flag;
    file->_cnt = 0;
    file->_ptr = 0;
    file->_base = 0;
    file->_tmpfname = 0;
    file->_file = fd;
    return file;
}
