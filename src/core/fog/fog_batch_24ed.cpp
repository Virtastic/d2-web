// src/core/fog/fog_batch_24ed.cpp -- twelve 1.13c Fog.dll rows, the statically linked MSVC 7.1 CRT
// stdio/onexit layer, reimplemented from the cached decompiles (alpha lane, NAMED). Retail's FILE is
// the MSVC struct with its own lock table and low-level _read/_write/_lseek; here FILE is the host
// libc FILE and the lock/unlock leaves (Fog+0x251F 0x2589 0x26D1 0x272F 0x28D8 0x2A7B 0x2B53) are
// no-ops, so each wrapper/_lk pair collapses to the libc call with the same result contract.
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>

// 1.13c Fog+0x24ED
// __onexit: register a termination function; returns it, or NULL when the table is full. The table
// is the CRT's own (grown by realloc); here a fixed 64-entry list run in reverse by fog_run_onexit.
// ponytail: fixed 64 slots, grow it if a caller ever registers more.
typedef int (*fog_onexit_t)(void);
static fog_onexit_t g_fog_onexit[64];
static int g_fog_onexitN;
extern "C" fog_onexit_t fog__onexit(fog_onexit_t fn) {
    if (g_fog_onexitN >= 64) return nullptr;
    g_fog_onexit[g_fog_onexitN++] = fn;
    return fn;
}

// 1.13c Fog+0x2537
// __fsopen: open a file with a share flag; share flag has no host meaning. NULL when no stream.
extern "C" FILE* fog__fsopen(const char* name, const char* mode, int /*shflag*/) {
    return fopen(name, mode);
}

// 1.13c Fog+0x25A6
// __fread_lk: unlocked fread; returns whole elements read (bytes / size), 0 if size*count is 0.
extern "C" size_t fog__fread_lk(void* dst, size_t size, size_t count, FILE* f) {
    return fread(dst, size, count, f);
}

// 1.13c Fog+0x268F
// _fread: lock, __fread_lk, unlock.
extern "C" size_t fog__fread(void* dst, size_t size, size_t count, FILE* f) {
    return fog__fread_lk(dst, size, count, f);
}

// 1.13c Fog+0x26DB
// _fprintf: lock, set up a temp buffer for unbuffered streams, format, release. Returns chars written.
extern "C" int fog__fprintf(FILE* f, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int n = vfprintf(f, fmt, ap);
    va_end(ap);
    return n;
}

// 1.13c Fog+0x2739
// __vsnprintf: formats into a count-limited string stream. Returns chars written, or -1 when the
// output did not fit; a NUL is stored only when at least one byte of room remains (exact fit: no NUL,
// return == count). A NULL dest only measures.
extern "C" int fog__vsnprintf(char* dest, size_t count, const char* fmt, va_list ap) {
    if (!dest) return vsnprintf(nullptr, 0, fmt, ap);
    int n = vsnprintf(dest, count, fmt, ap);
    if (n < 0 || (size_t)n > count) {
        if (count) dest[count - 1] = '\0';   // ponytail: host already terminated; retail leaves no NUL on overflow
        return -1;
    }
    return n;
}

// 1.13c Fog+0x278F
// __fwrite_lk: unlocked fwrite; returns whole elements written, 0 if size*count is 0.
extern "C" size_t fog__fwrite_lk(const void* src, size_t size, size_t count, FILE* f) {
    return fwrite(src, size, count, f);
}

// 1.13c Fog+0x2896
// _fwrite: lock, __fwrite_lk, unlock.
extern "C" size_t fog__fwrite(const void* src, size_t size, size_t count, FILE* f) {
    return fog__fwrite_lk(src, size, count, f);
}

// 1.13c Fog+0x28E2
// __ftell_lk: logical file position (OS position minus unread buffer, text-mode newlines counted);
// -1 on error. The host ftell is that contract.
extern "C" long fog__ftell_lk(FILE* f) {
    return ftell(f);
}

// 1.13c Fog+0x2A44
// _ftell: lock, __ftell_lk, unlock.
extern "C" long fog__ftell(FILE* f) {
    return fog__ftell_lk(f);
}

// 1.13c Fog+0x2A85
// __fseek_lk: origin must be 0/1/2 and the stream open, else errno EINVAL and -1; clears EOF, flushes,
// seeks. 0 on success, -1 on failure.
extern "C" int fog__fseek_lk(FILE* f, long off, int origin) {
    if (origin < 0 || origin > 2) return -1;
    return fseek(f, off, origin) == 0 ? 0 : -1;
}

// 1.13c Fog+0x2B14
// _fseek: lock, __fseek_lk, unlock.
extern "C" int fog__fseek(FILE* f, long off, int origin) {
    return fog__fseek_lk(f, off, origin);
}
