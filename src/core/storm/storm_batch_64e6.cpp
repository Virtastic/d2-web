// Batch 64e6: Storm CRT seam-style ports (callees supplied by the caller).
#include <cstdint>
#include <cstddef>

// 1.13c STORM+0x64E6: __fullpath; empty path -> getcwd. Null buffer -> malloc(0x104). errno 12 (nomem) / 0x22 (range).
extern "C" char *storm_fullpath(char *full, const char *path, size_t size,
                                char *(*getcwd_)(char *, size_t), void *(*malloc_)(size_t), void (*free_)(void *),
                                uint32_t (*GetFullPathNameA_)(const char *, uint32_t, char *, char **),
                                int *(*errno_)(), void (*dosmaperr)(uint32_t), uint32_t (*GetLastError_)()) {
    if (!path || !*path) return getcwd_(full, size);
    char *buf = full;
    if (!full) {
        buf = (char *)malloc_(0x104);
        if (!buf) { *errno_() = 0xc; return nullptr; }
        size = 0x104;
    }
    char *fp;
    uint32_t n = GetFullPathNameA_(path, (uint32_t)size, buf, &fp);
    if (size <= n) { if (!full) free_(buf); *errno_() = 0x22; return nullptr; }
    if (n == 0) { if (!full) free_(buf); dosmaperr(GetLastError_()); return nullptr; }
    return buf;
}

// 1.13c STORM+0x6590: ___toupper_mt; loc = threadlocinfo (+4 codepage, +0x14 lcid, +0x24 C-locale flag, +0x28 mb_cur_max, +0x48 ctype).
extern "C" uint32_t storm_toupper_mt(const uint8_t *loc, uint32_t c,
                                     uint32_t (*isctype_mt)(const uint8_t *, uint32_t, int),
                                     uint32_t (*lcmap)(uint32_t lcid, uint32_t flags, const char *src, uint32_t n, char *dst, uint32_t dn, uint32_t cp, int)) {
    auto I = [&](int o) { return *(const int32_t *)(loc + o); };
    if (I(0x14) == 0 || (I(0x24) != 0 && c < 0x80)) {
        if (c > 0x60 && (int)c < 0x7b) return c - 0x20;
        return c;
    }
    const uint8_t *ct = (const uint8_t *)(intptr_t)(uint32_t)I(0x48);
    if (c < 0x100) {
        uint32_t u = I(0x28) < 2 ? (ct[c * 2] & 2) : isctype_mt(loc, c, 2);
        if (!u) return c;
    }
    char in[4] = {0}, out[4] = {0};
    uint32_t n;
    if (!(ct[1 + ((c >> 8) & 0xff) * 2] & 0x80)) { in[0] = (char)c; n = 1; }
    else { in[0] = (char)(c >> 8); in[1] = (char)c; n = 2; }
    uint32_t r = lcmap((uint32_t)I(0x14), 0x200, in, n, out, 3, (uint32_t)I(4), 1);
    if (!r) return c;
    if (r != 1) return ((uint32_t)(uint8_t)out[0] << 8) | (uint8_t)out[1];
    return (uint8_t)out[0];
}

// 1.13c STORM+0x683A: _strtol -> strtoxl(str, end, radix, 0).
extern "C" long storm_strtol(const char *s, char **end, int radix, uint32_t (*strtoxl)(const uint8_t *, char **, int, int)) {
    return (long)strtoxl((const uint8_t *)s, end, radix, 0);
}

// 1.13c STORM+0x6D32: ___updatetlocinfo; _lock(0xc), update_lk, unlock.
extern "C" void *storm_updatetlocinfo(void (*lock)(int), void *(*update_lk)(), void (*unlock)()) {
    lock(0xc);
    void *r = update_lk();
    unlock();
    return r;
}
