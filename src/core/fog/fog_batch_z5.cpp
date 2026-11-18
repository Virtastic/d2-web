// src/core/fog/fog_batch_z5.cpp -- twelve 1.13c Fog.dll CRT rows (alpha lane, stage A) from
// build-wasm/ghidra-fog/decomp/0x<RVA>.c. Base 0x6FF50000. Retail globals and callees are seam
// parameters; host calls (CreateFile, RaiseException, SetStdHandle) are seam callbacks.
// _pioinfo blocks are 32 entries of 0x24 bytes: +0 handle, +4 osfile flags.
#include <cstdint>

struct FogCrtSeam {
    void (*setErrno)(int);      // FUN_6ff56a5a: *_errno() = v
    void (*setDosErrno)(uint32_t); // FUN_6ff56a63: *__doserrno() = v
};

// 1.13c Fog+0xA15E
extern "C"
int fog_set_osfhnd(const FogCrtSeam* s, uint8_t* const* pio, uint32_t nhandle, int fd, uint32_t h,
                   int consoleApp, void (*setStdHandle)(uint32_t, uint32_t)) {
    if ((uint32_t)fd < nhandle) {
        uint8_t* e = pio[fd >> 5] + (fd & 0x1f) * 0x24;
        if (*(int32_t*)e == -1) {
            if (consoleApp == 1) {
                uint32_t n;
                if (fd == 0) n = 0xfffffff6; else if (fd == 1) n = 0xfffffff5;
                else if (fd == 2) n = 0xfffffff4; else goto store;
                setStdHandle(n, h);
            }
        store:
            *(uint32_t*)e = h;
            return 0;
        }
    }
    s->setErrno(9); s->setDosErrno(0);
    return -1;
}

// 1.13c Fog+0xA1DA
extern "C"
int fog_free_osfhnd(const FogCrtSeam* s, uint8_t* const* pio, uint32_t nhandle, int fd,
                    int consoleApp, void (*setStdHandle)(uint32_t, uint32_t)) {
    if ((uint32_t)fd < nhandle) {
        uint8_t* e = pio[fd >> 5] + (fd & 0x1f) * 0x24;
        if ((e[4] & 1) && *(int32_t*)e != -1) {
            if (consoleApp == 1) {
                uint32_t n;
                if (fd == 0) n = 0xfffffff6; else if (fd == 1) n = 0xfffffff5;
                else if (fd == 2) n = 0xfffffff4; else goto store;
                setStdHandle(n, 0);
            }
        store:
            *(uint32_t*)e = 0xffffffff;
            return 0;
        }
    }
    s->setErrno(9); s->setDosErrno(0);
    return -1;
}

// 1.13c Fog+0x9D9F
// __tsopen_lk(shflag=this, ...). Host file ops are seam callbacks; returns fd or -1.
struct FogTsopenSeam {
    FogCrtSeam crt;
    int  (*allocOsfhnd)(void);                                   // __alloc_osfhnd
    uint32_t (*createFile)(const char*, uint32_t access, uint32_t share, int inherit,
                           uint32_t disp, uint32_t attrs);       // CreateFileA, -1 on failure
    uint32_t (*getFileType)(uint32_t h);
    void (*closeHandle)(uint32_t h);
    void (*dosmaperr)(uint32_t);                                 // GetLastError -> __dosmaperr
    uint32_t (*getLastError)(void);
    int  (*setOsfhnd)(int fd, uint32_t h);
    int32_t (*lseek_lk)(int fd, int32_t off, int whence);
    int  (*read_lk)(int fd, char* buf, uint32_t n);
    int  (*chsize_lk)(int fd, int32_t size);
    void (*close_lk)(int fd);
    uint8_t* (*osfile)(int fd);                                  // &_pioinfo(fd)->osfile
    uint32_t fmode;      // DAT_6ff806ac
    uint32_t umaskVal;   // DAT_6ff80200
    uint32_t osver2;     // DAT_6ff80204
};
extern "C"
uint32_t fog_tsopen_lk(const FogTsopenSeam* s, uint32_t shflag, uint32_t* pUnlockFlag, uint32_t* pFd,
                       const char* path, uint32_t oflag, uint8_t pmode) {
    uint32_t access, share, disp, attrs = 0x80;
    uint8_t fl = (int8_t)oflag < 0 ? 0 : 0x10;
    int inherit = (int8_t)oflag >= 0;
    if (!(oflag & 0x8000) && ((oflag & 0x4000) || s->fmode != 0x8000)) fl |= 0x80;
    switch (oflag & 3) { case 0: access = 0x80000000; break; case 1: access = 0x40000000; break;
                         case 2: access = 0xc0000000; break; default: goto bad; }
    switch (shflag) { case 0x10: share = 0; break; case 0x20: share = 1; break;
                      case 0x30: share = 2; break; case 0x40: share = 3; break; default: goto bad; }
    switch (oflag & 0x700) {
        case 0: case 0x400: disp = 3; break;
        case 0x100: disp = 4; break;
        case 0x200: case 0x600: disp = 5; break;
        case 0x300: disp = 2; break;
        case 0x500: case 0x700: disp = 1; break;
        default: goto bad;
    }
    if ((oflag & 0x100) && !((int8_t)(~(uint8_t)s->umaskVal & pmode) < 0)) attrs = 1;
    if (oflag & 0x40) { access = 0x10000; attrs |= 0x4000000; if (s->osver2 == 2) share |= 4; }
    if (oflag & 0x1000) attrs |= 0x100;
    if (oflag & 0x20) attrs |= 0x8000000; else if (oflag & 0x10) attrs |= 0x10000000;
    {
        int fd = s->allocOsfhnd();
        if (fd == -1) { s->crt.setErrno(0x18); s->crt.setDosErrno(0); return 0xffffffff; }
        *pUnlockFlag = 1; *pFd = (uint32_t)fd;
        uint32_t h = s->createFile(path, access, share, inherit, disp, attrs);
        if (h != 0xffffffff) {
            uint32_t t = s->getFileType(h);
            if (t != 0) {
                if (t == 2) fl |= 0x40; else if (t == 3) fl |= 8;
                s->setOsfhnd(fd, h);
                uint8_t fb = fl | 1;
                *s->osfile(fd) = fb;
                fl &= 0x48;
                if (fl == 0 && (int8_t)fb < 0 && (oflag & 2)) {
                    int32_t p = s->lseek_lk(fd, -1, 2);
                    if (p == -1) {
                        // retail: errno ERROR_NEGATIVE_SEEK (0x83) is tolerated, via the seam
                        if (s->getLastError() == 0x83) goto ok;
                    } else {
                        char c = 0;
                        if (s->read_lk(fd, &c, 1) == 0 && c == 0x1a && s->chsize_lk(fd, p) == -1) {}
                        else if (s->lseek_lk(fd, 0, 0) != -1) goto ok;
                    }
                    s->close_lk(fd);
                    return 0xffffffff;
                }
            ok:
                if (fl == 0 && (oflag & 8)) *s->osfile(fd) |= 0x20;
                return (uint32_t)fd;
            }
            s->closeHandle(h);
        }
        s->dosmaperr(s->getLastError());
        return 0xffffffff;
    }
bad:
    s->crt.setErrno(0x16); s->crt.setDosErrno(0);
    return 0xffffffff;
}

// 1.13c Fog+0xA594
// _abort: FUN_6ff584cc(10) (print abort message), raise(SIGABRT=0x16), _exit(3).
extern "C"
void fog_abort(void (*printMsg)(int), void (*raise_)(int), void (*exit_)(int)) {
    printMsg(10); raise_(0x16); exit_(3);
}

// 1.13c Fog+0xA5E8
// __fpmath: init FP state, record the P5 FDIV-bug test, optionally reset default precision.
extern "C"
void fog_fpmath(int initPrec, void (*fpinit)(void), int (*p5Test)(void), int* pP5Flag,
                void (*setDefaultPrecision)(void)) {
    fpinit();
    *pP5Flag = p5Test();
    if (initPrec) setDefaultPrecision();
}

// 1.13c Fog+0xA606
// __raise_exc: build the exception-record image p[], raise, fold the handler's edits back into the
// x87 control word *cw. p is uint32_t[0x1a]; stat = __statfp(); clrfp = __clrfp().
extern "C"
void fog_raise_exc(uint32_t* p, uint32_t* cw, uint32_t code, int op, const uint64_t* a, uint64_t* res,
                   uint32_t (*statfp)(void), void (*clrfp)(void),
                   void (*raiseException)(uint32_t, uint32_t, uint32_t, uint32_t**)) {
    uint32_t c = code;
    p[1] = p[2] = p[3] = 0;
    if (c & 0x10) { p[1] |= 1;  code = 0xc000008f; }
    if (c & 2)    { p[1] |= 2;  code = 0xc0000093; }
    if (c & 1)    { p[1] |= 4;  code = 0xc0000091; }
    if (c & 4)    { p[1] |= 8;  code = 0xc000008e; }
    if (c & 8)    { p[1] |= 0x10; code = 0xc0000090; }
    p[2] ^= (~(*cw << 4) ^ p[2]) & 0x10;
    p[2] ^= (~(*cw << 1) ^ p[2]) & 8;
    p[2] ^= (~(*cw >> 1) ^ p[2]) & 4;
    p[2] ^= (~(*cw >> 3) ^ p[2]) & 2;
    p[2] ^= (~(*cw >> 5) ^ p[2]) & 1;
    uint32_t st = statfp();
    if (st & 1)    p[3] |= 0x10;
    if (st & 4)    p[3] |= 8;
    if (st & 8)    p[3] |= 4;
    if (st & 0x10) p[3] |= 2;
    if (st & 0x20) p[3] |= 1;
    switch (*cw & 0xc00) {
        case 0:     p[0] &= ~3u; break;
        case 0x400: p[0] = (p[0] & ~2u) | 1; break;
        case 0x800: p[0] = (p[0] & ~1u) | 2; break;
        case 0xc00: p[0] |= 3; break;
    }
    switch (*cw & 0x300) {
        case 0:     p[0] = (p[0] & 0xffffffebu) | 8; break;
        case 0x200: p[0] = (p[0] & 0xffffffe7u) | 4; break;
        case 0x300: p[0] &= 0xffffffe3u; break;
    }
    p[0] ^= ((uint32_t)op << 5 ^ p[0]) & 0x1ffe0;
    p[8] |= 1;  p[8] = (p[8] & 0xffffffe3u) | 2;
    *(uint64_t*)(p + 4) = *a;
    p[0x18] |= 1; p[0x18] = (p[0x18] & 0xffffffe3u) | 2;
    *(uint64_t*)(p + 0x14) = *res;
    clrfp();
    uint32_t* pp = p;
    raiseException(code, 0, 1, &pp);
    if (p[2] & 0x10) *cw &= ~1u;
    if (p[2] & 8)    *cw &= ~4u;
    if (p[2] & 4)    *cw &= ~8u;
    if (p[2] & 2)    *cw &= ~0x10u;
    if (p[2] & 1)    *cw &= ~0x20u;
    switch (p[0] & 3) {
        case 0: *cw &= 0xfffff3ffu; break;
        case 1: *cw = (*cw & 0xfffff7ffu) | 0x400; break;
        case 2: *cw = (*cw & 0xfffffbffu) | 0x800; break;
        case 3: *cw |= 0xc00; break;
    }
    switch ((p[0] >> 2) & 7) {
        case 0: *cw = (*cw & 0xfffff3ffu) | 0x300; break;
        case 1: *cw = (*cw & 0xfffff3ffu) | 0x200; break;
        case 2: *cw &= 0xfffff3ffu; break;
    }
    *res = *(uint64_t*)(p + 0x14);
}

// 1.13c Fog+0xAACF
// __set_errno_from_matherr: 1 (DOMAIN) -> EDOM 0x21; 2,3 (SING, OVERFLOW) -> ERANGE 0x22.
extern "C"
int fog_set_errno_from_matherr(const FogCrtSeam* s, int v) {
    if (v == 1) s->setErrno(0x21);
    else if (v > 1 && v < 4) s->setErrno(0x22);
    return v;
}

// 1.13c Fog+0xACD1
// __frnd: frndint, round-to-nearest-even under the default control word (seam: current mode).
extern "C"
double fog_frnd(double x, double (*frndint)(double)) { return frndint(x); }

// 1.13c Fog+0xACE2
// __set_exp: replace the exponent field of the double's bits with (e + 0x3fe); sign/mantissa kept.
extern "C"
uint64_t fog_set_exp(uint64_t bits, int16_t e) {
    uint16_t top = (uint16_t)(bits >> 48);
    top = (uint16_t)(((e + 0x3fe) * 0x10) | (top & 0x800f));
    return (bits & 0x0000ffffffffffffull) | ((uint64_t)top << 48);
}

// 1.13c Fog+0xAD0C
// __sptype: 0 finite, 1 +inf, 2 -inf, 3 quiet NaN, 4 signalling NaN. (lo, hi) = double halves.
extern "C"
int fog_sptype(uint32_t lo, uint32_t hi) {
    if (hi == 0x7ff00000) { if (lo == 0) return 1; }
    else if (hi == 0xfff00000 && lo == 0) return 2;
    if (((hi >> 16) & 0x7ff8) == 0x7ff8) return 3;
    if (((hi >> 16) & 0x7ff8) != 0x7ff0 || ((hi & 0x7ffff) == 0 && lo == 0)) return 0;
    return 4;
}

// 1.13c Fog+0xAD67
// __decomp (frexp core): returns the fraction bits in [0.5,1) and stores the binary exponent.
// Zero -> 0, exp 0. Denormals are normalised by shifting; the exponent is -1021 - shifts
// (retail keeps the shift count in EDX, which the decompile loses).
extern "C"
uint64_t fog_decomp(uint32_t lo, uint32_t hi, int* pExp) {
    uint64_t v = ((uint64_t)hi << 32) | lo;
    if ((v << 1) == 0) { *pExp = 0; return 0; }
    if ((hi & 0x7ff00000) == 0) {
        bool neg = (hi & 0x80000000) != 0;
        int sh = 0;
        while (!((v >> 52) & 1)) { v <<= 1; ++sh; }
        if (neg) v |= 1ull << 63;
        *pExp = -1021 - sh;
        return fog_set_exp(v & 0xffefffffffffffffull, 0);
    }
    *pExp = (int)((hi >> 16 & 0x7ff0) >> 4) - 0x3fe;
    return fog_set_exp(v, 0);
}

// 1.13c Fog+0xAE3A
// __ctrlfp: fnstcw, returned sign-extended (the control word is a seam read).
extern "C"
int fog_ctrlfp(int16_t (*fnstcw)(void)) { return fnstcw(); }
