// D2Multi batch b67: statically linked MSVC CRT pieces (stdio write path, lseek, write, abort msg).
// Skipped: 1EF5 (__CRT_INIT, Win32), 2076 (entry, SEH), 2334 (vfprintf core, x87 + giant), 463D/475C (direct Win32).
// Retail FILE/ioinfo are 32-bit layouts; modelled here with host pointers (ponytail: layout not byte-exact).
#include <cstdint>
#include <initializer_list>

struct D2MultiFile { char* _ptr; int32_t _cnt; char* _base; uint32_t _flag; int32_t _file; int32_t _charbuf; int32_t _bufsiz; char* _tmpfname; };
struct D2MultiIoInfo { uintptr_t osfhnd; uint8_t osfile; uint8_t pad[0x1f]; };

extern "C" {
extern int32_t DAT_6f9ebad4, DAT_6f9ebad8;
extern void (*DAT_6f9ebd5c)(void);
extern void (*PTR___exit_6f9e9040)(int32_t);
extern uint32_t DAT_6fa0b38c;                 // _nhandle
extern D2MultiIoInfo* DAT_6fa0b3a0[];         // __pioinfo
extern D2MultiFile DAT_6f9e9600, DAT_6f9e9620; // stdout, stderr
extern D2MultiIoInfo DAT_6f9e9490;            // __badioinfo
void FUN_6f9d448c(int32_t rtcode);            // _NMSG_WRITE
int32_t* FUN_6f9d39d4(void);                  // _errno
uint32_t* FUN_6f9d39dd(void);                 // __doserrno
void FUN_6f9d4738(void);                      // _unlock_fhandle (lseek)
void FUN_6f9d499f(void);                      // _unlock_fhandle (write)
void __lock_fhandle(int32_t);
int32_t __isatty(int32_t);
void __getbuf(D2MultiFile*);
int32_t __lseek_lk(uint32_t, int32_t, uint32_t);
int32_t FUN_6f9d475c(uint32_t, const char*, uint32_t); // _write_lk
int32_t __write(int32_t, const void*, uint32_t);
int32_t d2multi_lseek(int32_t, int32_t, int32_t);
int32_t __flsbuf(int32_t, D2MultiFile*);
void __FF_MSGBANNER(void);
void write_char(uint8_t, D2MultiFile*, int32_t*);
uint32_t ___crtGetStringTypeA(uint32_t, uint32_t, const char*, int32_t, uint16_t*, int32_t, int32_t);
}

// 1.13c D2Multi+0x4604
extern "C" void __FF_MSGBANNER(void) {
    if (DAT_6f9ebad4 == 1 || (DAT_6f9ebad4 == 0 && DAT_6f9ebad8 == 1)) {
        FUN_6f9d448c(0xfc);
        if (DAT_6f9ebd5c) DAT_6f9ebd5c();
        FUN_6f9d448c(0xff);
    }
}

// 1.13c D2Multi+0x215A
extern "C" void __amsg_exit(int32_t code) {
    if (DAT_6f9ebad4 == 1 || (DAT_6f9ebad4 == 0 && DAT_6f9ebad8 == 1)) __FF_MSGBANNER();
    FUN_6f9d448c(code);
    PTR___exit_6f9e9040(0xff);
}

// 1.13c D2Multi+0x218D
extern "C" int32_t __flsbuf(int32_t ch, D2MultiFile* f) {
    uint32_t fl = f->_flag;
    int32_t fh = f->_file;
    int32_t written = 0, want = 0;
    if ((fl & 0x82) == 0 || (fl & 0x40) != 0) { f->_flag = fl | 0x20; return -1; }
    if (fl & 1) {
        f->_cnt = 0;
        if ((fl & 0x10) == 0) { f->_flag = fl | 0x20; return -1; }
        f->_ptr = f->_base;
        f->_flag = fl & ~1u;
    }
    fl = f->_flag;
    f->_cnt = 0;
    f->_flag = (fl & ~0x10u) | 2;
    if ((fl & 0x10c) == 0 && ((f != &DAT_6f9e9600 && f != &DAT_6f9e9620) || __isatty(fh) == 0)) __getbuf(f);
    if ((f->_flag & 0x108) == 0) {
        want = 1;
        char c = (char)ch;
        written = __write(fh, &c, 1);
    } else {
        char* b = f->_base;
        char* p = f->_ptr;
        f->_ptr = b + 1;
        want = (int32_t)(p - b);
        f->_cnt = f->_bufsiz - 1;
        if (want < 1) {
            D2MultiIoInfo* io = fh == -1 ? &DAT_6f9e9490 : DAT_6fa0b3a0[fh >> 5] + (fh & 0x1f);
            if (io->osfile & 0x20) d2multi_lseek(fh, 0, 2);
        } else {
            written = __write(fh, b, (uint32_t)want);
        }
        *f->_base = (char)ch;
    }
    if (written == want) return ch & 0xff;
    f->_flag |= 0x20;
    return -1;
}

// 1.13c D2Multi+0x22A6
// Register inputs: AL -> ch, ECX -> f, ESI -> pcount.
extern "C" void write_char(uint8_t ch, D2MultiFile* f, int32_t* pcount) {
    if ((f->_flag & 0x40) == 0 || f->_base != nullptr) {
        uint32_t r;
        if (--f->_cnt < 0) r = (uint32_t)__flsbuf((int8_t)ch, f);
        else { *f->_ptr++ = (char)ch; r = ch; }
        if (r == 0xffffffffu) { *pcount = -1; return; }
    }
    *pcount += 1;
}

// 1.13c D2Multi+0x22D9
// Register inputs: EAX -> pcount, ECX -> f; ch is the stack char (param_1), n the count.
extern "C" void write_multi_char(uint8_t ch, int32_t n, int32_t* pcount, D2MultiFile* f) {
    while (n > 0) { --n; write_char(ch, f, pcount); if (*pcount == -1) return; }
}

// 1.13c D2Multi+0x22FD
// Register inputs: EAX -> pcount, EDI -> f; text (ESI/hidden) -> s. n is the stack length.
extern "C" void write_string(int32_t n, const char* s, int32_t* pcount, D2MultiFile* f) {
    if ((f->_flag & 0x40) == 0 || f->_base != nullptr) {
        while (n > 0) { --n; write_char((uint8_t)*s++, f, pcount); if (*pcount == -1) return; }
    } else {
        *pcount += n;
    }
}

// 1.13c D2Multi+0x2B85
// __thiscall: this -> unused-high bits; param_1 = locale info ptr (fields +0x48 ctype table, +4 cp, +0x14 lcid).
extern "C" uint32_t ___isctype_mt(const uint8_t* loc, int32_t c, uint32_t mask) {
    uint16_t t;
    const uint8_t* tbl = *(const uint8_t* const*)(loc + 0x48);
    if ((uint32_t)(c + 1) < 0x101) {
        t = *(const uint16_t*)(tbl + c * 2);
    } else {
        char buf[2]; int32_t len;
        if ((tbl[1 + ((c >> 8) & 0xff) * 2] & 0x80) == 0) { buf[0] = (char)c; len = 1; }
        else { buf[0] = (char)(c >> 8); buf[1] = (char)c; len = 2; }
        if (!___crtGetStringTypeA(1, 0, buf, len, &t, *(const int32_t*)(loc + 0x14), 1)) return 0;
    }
    return t & mask;
}

// 1.13c D2Multi+0x46B1
int32_t d2multi_lseek(int32_t fh, int32_t off, int32_t origin) {
    if ((uint32_t)fh < DAT_6fa0b38c) {
        D2MultiIoInfo* io = DAT_6fa0b3a0[fh >> 5] + (fh & 0x1f);
        if (io->osfile & 1) {
            int32_t r;
            __lock_fhandle(fh);
            if ((io->osfile & 1) == 0) { *FUN_6f9d39d4() = 9; *FUN_6f9d39dd() = 0; r = -1; }
            else r = __lseek_lk((uint32_t)fh, off, (uint32_t)origin);
            FUN_6f9d4738();
            return r;
        }
    }
    *FUN_6f9d39d4() = 9; *FUN_6f9d39dd() = 0;
    return -1;
}

// 1.13c D2Multi+0x4918
extern "C" int32_t __write(int32_t fh, const void* buf, uint32_t n) {
    if ((uint32_t)fh < DAT_6fa0b38c) {
        D2MultiIoInfo* io = DAT_6fa0b3a0[fh >> 5] + (fh & 0x1f);
        if (io->osfile & 1) {
            int32_t r;
            __lock_fhandle(fh);
            if ((io->osfile & 1) == 0) { *FUN_6f9d39d4() = 9; *FUN_6f9d39dd() = 0; r = -1; }
            else r = FUN_6f9d475c((uint32_t)fh, (const char*)buf, n);
            FUN_6f9d499f();
            return r;
        }
    }
    *FUN_6f9d39d4() = 9; *FUN_6f9d39dd() = 0;
    return -1;
}
