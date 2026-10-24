// Batch i04: Storm CRT/PKWARE seam-style ports (callees and retail globals supplied by the caller).
#include <cstdint>
#include <cstring>
#include <cstdlib>

// 1.13c STORM+0x60BE: MSVC eh vector constructor iterator; runs ctor count times (SEH unwind cleanup omitted).
extern "C" void storm_eh_vector_ctor_iter(void *p, uint32_t size, int count, void (*ctor)(void *)) {
    (void)size;
    for (int i = 0; i < count; i++) ctor(p);
}

// 1.13c STORM+0xC911: _isatty; ioinfo is the table of 0x24-byte entries per 32 handles, flags at +4.
extern "C" int storm_isatty(uint32_t fh, uint32_t nhandles, uint8_t *const *ioinfo) {
    if (fh >= nhandles) return 0;
    return (int8_t)ioinfo[fh >> 5][(fh & 0x1f) * 0x24 + 4] & 0x40;
}

struct StormOutFile { char *ptr; int cnt; char *base; int flag; };

// 1.13c STORM+0x7AE9: _write_char; putc to the stream, *count = -1 on EOF else ++.
extern "C" void storm_write_char(uint8_t ch, StormOutFile *f, int *count, int (*flsbuf)(int, StormOutFile *)) {
    if (!((f->flag & 0x40) == 0 || f->base != nullptr)) { (*count)++; return; }
    int r;
    if (--f->cnt < 0) r = flsbuf((int8_t)ch, f);
    else { *f->ptr++ = (char)ch; r = ch; }
    if (r == -1) *count = -1; else (*count)++;
}

// 1.13c STORM+0x7B40: _write_string; write n bytes, stop on error. String-only streams just count.
extern "C" void storm_write_string(const uint8_t *s, int n, StormOutFile *f, int *count, int (*flsbuf)(int, StormOutFile *)) {
    if ((f->flag & 0x40) && f->base == nullptr) { *count += n; return; }
    while (n > 0) {
        n--;
        storm_write_char(*s++, f, count, flsbuf);
        if (*count == -1) return;
    }
}

// 1.13c STORM+0x925E: forwards 4 to the lock routine (_lock(4)).
extern "C" void storm_lock4(void (*lock)(int)) { lock(4); }

// 1.13c STORM+0x90D6: forwards 10 to the lock routine (_lock(10)).
extern "C" void storm_lock10(void (*lock)(int)) { lock(10); }

// 1.13c STORM+0x83C0: strcat; returns dst.
extern "C" char *storm_strcat(char *dst, const char *src) { strcat(dst, src); return dst; }

// 1.13c STORM+0x9F15: _getdcwd; drive 0 = current. getcwd/fullpath are seams for the two Win32 calls,
// each returns the length (0 = failure). validdrive failure -> errno 0xd (EACCES-ish), alloc fail 0xc, too small 0x22.
extern "C" char *storm_getdcwd(uint32_t drive, char *buf, size_t len, int (*validdrive)(uint32_t),
                               uint32_t (*get_path)(uint32_t drive, char *out260), int *err) {
    char tmp[0x104];
    if (drive && !validdrive(drive)) { *err = 0xd; return nullptr; }
    uint32_t n = get_path(drive, tmp);
    if (n == 0 || n + 1 >= 0x105) return nullptr;
    uint32_t sz = n + 1;
    if (!buf) {
        if ((int)sz <= (int)len) sz = (uint32_t)len;
        buf = (char *)malloc(sz);
        if (!buf) { *err = 0xc; return nullptr; }
    } else if ((int)sz > (int)len) { *err = 0x22; return nullptr; }
    strcpy(buf, tmp);
    return buf;
}

// 1.13c STORM+0xEDA0: PKWARE explode main loop. ctx+8 out pos, +0x24 user, +0x2c write cb(buf,&size,user),
// +0x30 window, +0x1030 flush area. decode_sym/copy_dist/memmove are the three callees (EEA0, F050, F2A0).
extern "C" uint32_t pk_explode_loop(uint8_t *c, uint32_t (*decode_sym)(uint8_t *), int (*copy_dist)(uint8_t *, uint32_t),
                                    void (*mv)(void *, void *, int)) {
    typedef void (*Cb)(void *, uint32_t *, void *);
    auto cb = *(Cb *)(c + 0x2c);
    auto user = *(void **)(c + 0x24);
    auto &pos = *(int32_t *)(c + 8);
    pos = 0x1000;
    uint32_t sym;
    for (;;) {
        sym = decode_sym(c);
        if (sym > 0x304) break;
        if (sym < 0x100) c[pos++ + 0x30] = (uint8_t)sym;
        else {
            sym -= 0xfe;
            int d = copy_dist(c, sym);
            if (d == 0) { sym = 0x306; break; }
            uint8_t *dst = c + pos + 0x30; const uint8_t *src = dst - d;
            pos += sym;
            while (sym--) *dst++ = *src++;
        }
        if ((uint32_t)pos > 0x1fff) {
            uint32_t n = 0x1000;
            cb(c + 0x1030, &n, user);
            mv(c + 0x30, c + 0x1030, pos - 0x1000);
            pos -= 0x1000;
        }
    }
    int32_t n = pos - 0x1000;
    cb(c + 0x1030, (uint32_t *)&n, user);
    return sym;
}

// 1.13c STORM+0xF1A0: PKWARE gen_asc_tabs; tab = retail ushort code table (256 entries, ascending index).
extern "C" void pk_gen_asc_tabs(uint8_t *c, const uint16_t *tab) {
    for (int i = 255; i >= 0; i--) {
        uint8_t *lenp = c + 0x2fb4 + i;
        uint32_t b = *lenp, code = tab[i], x;
        if (b < 9) {
            for (x = code; x < 0x100; x += 1u << (b & 31)) c[0x2c34 + x] = (uint8_t)i;
        } else if ((code & 0xff) == 0) {
            *lenp = b - 8;
            for (x = code >> 8; x < 0x100; x += 1u << ((b - 8) & 31)) c[0x2eb4 + x] = (uint8_t)i;
        } else {
            c[0x2c34 + (code & 0xff)] = 0xff;
            if ((code & 0x3f) == 0) {
                b = *lenp - 6; *lenp = (uint8_t)b;
                for (x = code >> 6; x < 0x80; x += 1u << (b & 31)) c[0x2e34 + x] = (uint8_t)i;
            } else {
                b = *lenp - 4; *lenp = (uint8_t)b;
                for (x = code >> 4; x < 0x100; x += 1u << (b & 31)) c[0x2d34 + x] = (uint8_t)i;
            }
        }
    }
}
