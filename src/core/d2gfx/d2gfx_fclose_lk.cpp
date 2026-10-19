// 1.13c D2Gfx+0x06FA6 (__fclose_lk, 76 bytes): caller owns the FILE lock. A stream whose
// _flag intersects 0x83 is flushed, its buffer freed and descriptor closed. A close error forces -1;
// after a successful close, a non-null temporary name is freed and cleared. The flag is cleared on
// every path, and the result is the flush status unless close failed.
// Prior art: no matching CRT implementation in D2MOO's D2Gfx module; cross-checked the same VS2003
// routine already recovered for D2Client+0x03857, and kept this DLL's callees as explicit seams.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

// MSVC 2003's 32-bit FILE layout. Pointer fields stay retail uint32 values under wasm64.
struct D2GfxCrtFile {
    uint32_t _ptr;       // +0x00
    int32_t  _cnt;       // +0x04
    uint32_t _base;      // +0x08
    int32_t  _flag;      // +0x0C
    int32_t  _file;      // +0x10
    int32_t  _charbuf;   // +0x14
    int32_t  _bufsiz;    // +0x18
    uint32_t _tmpfname;  // +0x1C
};
static_assert(sizeof(D2GfxCrtFile) == 0x20);
static_assert(offsetof(D2GfxCrtFile, _flag) == 0x0C);
static_assert(offsetof(D2GfxCrtFile, _file) == 0x10);
static_assert(offsetof(D2GfxCrtFile, _tmpfname) == 0x1C);

// Unrecovered D2Gfx callees remain explicit host seams, in retail call order.
struct D2GfxFcloseLkEnv {
    int32_t (*flush)(D2GfxCrtFile*);   // 0x05EB4 __flush
    void (*freebuf)(D2GfxCrtFile*);    // 0x075F8 __freebuf
    int32_t (*close_)(int32_t);        // 0x0755D __close
    void (*free_)(void*);              // 0x02BD4 _free
};

extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2gfx_6fa6(
    D2GfxCrtFile* file, const D2GfxFcloseLkEnv* env) {
    int32_t result = -1;
    if ((file->_flag & 0x83) != 0) {
        result = env->flush(file);
        env->freebuf(file);
        const int32_t closed = env->close_(file->_file);
        if (closed < 0) {
            result = -1;
        } else if (file->_tmpfname != 0) {
            env->free_(reinterpret_cast<void*>(static_cast<uintptr_t>(file->_tmpfname)));
            file->_tmpfname = 0;
        }
    }
    file->_flag = 0;
    return result;
}
