// D2Multi batch z42: MSVCRT statics (lock stubs, heap select, banner, lseek/write, getbuf, isatty, strlen, malloc).
// 32-bit pointers/links are uint32_t; retail callees and globals are weak seams.
#include <cstdint>
#include <cstddef>
#define W __attribute__((weak))
extern void z42_31F1(int) W;                       // FUN_6f9d31f1
extern void z42_448C(int) W;                       // FUN_6f9d448c (_NMSG_WRITE)
extern void z42_lock_fhandle(int) W; extern void z42_4738() W; extern void z42_499F() W;
extern int32_t z42_lseek_lk(int, int32_t, int) W; extern int z42_475C(int, const void*, uint32_t) W;
extern int32_t* z42_errno() W; extern uint32_t* z42_doserrno() W;
extern void* z42_heap_alloc(uint32_t) W; extern int z42_callnewh(uint32_t) W;
extern uint32_t z42_malloc_ptr(uint32_t) W;        // _malloc result as 32-bit link
extern uint32_t DAT_6f9eba84 W, DAT_6f9eba90 W, DAT_6f9ebad4 W, DAT_6f9ebad8 W, DAT_6f9ebd5c W, DAT_6f9ebd60 W,
    DAT_6fa0b38c W, DAT_6f9ebe38 W;
extern uint32_t DAT_6fa0b3a0[] W;                  // ioinfo block table (0x24-byte entries)
extern void z42_call32(uint32_t fn) W;

// 1.13c D2Multi+0x3AAC
void z42_3AAC(void) { z42_31F1(4); }
// 1.13c D2Multi+0x3B90
void z42_3B90(void) { z42_31F1(4); }
// 1.13c D2Multi+0x4C9D
void z42_4C9D(void) { z42_31F1(4); }

// 1.13c D2Multi+0x41E2
uint32_t z42_heap_select(void) { return (DAT_6f9eba84 == 2 && 4 < DAT_6f9eba90) ? 1 : 3; }

// 1.13c D2Multi+0x4604
void z42_FF_MSGBANNER(void) {
    if (DAT_6f9ebad4 == 1 || (DAT_6f9ebad4 == 0 && DAT_6f9ebad8 == 1)) {
        z42_448C(0xfc);
        if (DAT_6f9ebd5c) z42_call32(DAT_6f9ebd5c);
        z42_448C(0xff);
    }
}

static uint8_t* z42_fh(int h) { return (uint8_t*)(uintptr_t)DAT_6fa0b3a0[h >> 5] + (h & 0x1f) * 0x24; }
static int32_t z42_badf() { *z42_errno() = 9; *z42_doserrno() = 0; return -1; }

// 1.13c D2Multi+0x46B1
int32_t z42_lseek(int h, int32_t off, int origin) {
    if ((uint32_t)h < DAT_6fa0b38c && (z42_fh(h)[4] & 1)) {
        z42_lock_fhandle(h);
        int32_t r;
        if (!(z42_fh(h)[4] & 1)) { z42_badf(); r = -1; } else r = z42_lseek_lk(h, off, origin);
        z42_4738();
        return r;
    }
    return z42_badf();
}

// 1.13c D2Multi+0x4918
int z42_write(int h, const void* buf, uint32_t n) {
    if ((uint32_t)h < DAT_6fa0b38c && (z42_fh(h)[4] & 1)) {
        z42_lock_fhandle(h);
        int r;
        if (!(z42_fh(h)[4] & 1)) { z42_badf(); r = -1; } else r = z42_475C(h, buf, n);
        z42_499F();
        return r;
    }
    return z42_badf();
}

struct z42_FILE { uint32_t ptr; int32_t cnt; uint32_t base; int32_t flag; int32_t file; int32_t charbuf; int32_t bufsiz; };
// 1.13c D2Multi+0x4CD2
uint32_t z42_malloc(uint32_t n);
// 1.13c D2Multi+0x49C3
void z42_getbuf(z42_FILE* f) {
    DAT_6f9ebd60++;
    uint32_t p = z42_malloc(0x1000);
    f->base = p;
    if (!p) { f->flag |= 4; f->base = (uint32_t)(uintptr_t)&f->charbuf; f->bufsiz = 2; }
    else { f->flag |= 8; f->bufsiz = 0x1000; }
    f->cnt = 0;
    f->ptr = f->base;
}

// 1.13c D2Multi+0x4A07
int z42_isatty(int h) {
    if (DAT_6fa0b38c <= (uint32_t)h) return 0;
    return (int)(int8_t)z42_fh(h)[4] & 0x40;
}

// 1.13c D2Multi+0x4BA0
size_t z42_strlen(const char* s) { size_t n = 0; while (s[n]) n++; return n; }

// 1.13c D2Multi+0x4CA6
void* z42_nh_malloc(uint32_t n, int nh) {
    if (n < 0xffffffe1u)
        do {
            void* p = z42_heap_alloc(n);
            if (p) return p;
            if (!nh) return nullptr;
        } while (z42_callnewh(n));
    return nullptr;
}
// 1.13c D2Multi+0x4CD2
uint32_t z42_malloc(uint32_t n) { return (uint32_t)(uintptr_t)z42_nh_malloc(n, (int)DAT_6f9ebe38); }
