// D2Multi batch z36: CRT onexit table, SEH prolog/epilog, RTC init, _atexit, ___ascii_stricmp, errno accessors, _dosmaperr, _free.
// 32-bit links are uint32_t; retail's callees and globals are weak seams.
#include <cstdint>
#define W __attribute__((weak))
extern "C++" {
extern uint32_t z36_msize(uint32_t blk) W;                    // __msize
extern uint32_t z36_realloc60DF(uint32_t blk, uint32_t n) W;  // FUN_6f9d60df
extern uint32_t z36_malloc(uint32_t n) W;                     // _malloc (4CD2)
extern void z36_lock1363() W; extern void z36_unlock33B1() W; // _lockexit / _unlockexit
extern void z36_lock(int) W; extern void z36_unlock3AAC() W;  // __lock(4) / FUN_3AAC
extern uint32_t z36_sbhFindBlock(uint32_t p) W;               // ___sbh_find_block
extern void z36_sbhFreeBlock(uint32_t hdr, uint32_t p) W;     // ___sbh_free_block
extern void z36_heapFree(uint32_t heap, uint32_t p) W;        // HeapFree(h,0,p)
extern int32_t* z36_getptd() W;                               // __getptd(): _terrno at +8, _tdoserrno at +0xc
extern uint32_t DAT_6fa0b4ac W, DAT_6fa0b4b0 W, DAT_6fa0b388 W, DAT_6fa0b384 W;
extern uint32_t DAT_6f9e81bc[] W;                             // RTC init table (begin == end in retail)
extern uint32_t DAT_6f9e9320[] W;                             // {oserr, errno} x 0x2d
extern uint32_t z36_ExceptionList W;                          // fs:[0]
extern uint32_t* z36_ebp W; extern uint32_t z36_retaddr W;    // register inputs of the inlined helpers
extern uint32_t z36_stackSave[4] W;
extern uint32_t* z36_mem32(uint32_t a) W;                     // linear-memory view of a 32-bit link
extern void z36_call(uint32_t fn) W;

static uint32_t* P(uint32_t a) { return z36_mem32(a); }

// 1.13c D2Multi+0x32D7  (__onexit_lk; edi = function pointer)
void d2multi_32D7(uint32_t edi) {
    uint32_t sz = z36_msize(DAT_6fa0b4b0);
    if (sz < DAT_6fa0b4ac + (4 - DAT_6fa0b4b0)) {
        uint32_t grow = sz < 0x800 ? sz : 0x800;
        uint32_t n = z36_realloc60DF(DAT_6fa0b4b0, grow + sz);
        if (!n) {
            n = z36_realloc60DF(DAT_6fa0b4b0, sz + 0x10);
            if (!n) return;
        }
        DAT_6fa0b4ac = n + (((int32_t)(DAT_6fa0b4ac - DAT_6fa0b4b0)) >> 2) * 4;
        DAT_6fa0b4b0 = n;
    }
    *P(DAT_6fa0b4ac) = edi;
    DAT_6fa0b4ac += 4;
}

// 1.13c D2Multi+0x3357  (___onexitinit)
uint32_t d2multi_3357(void) {
    DAT_6fa0b4b0 = z36_malloc(0x80);
    if (!DAT_6fa0b4b0) return 0x18;
    *P(DAT_6fa0b4b0) = 0;
    DAT_6fa0b4ac = DAT_6fa0b4b0;
    return 0;
}

// 1.13c D2Multi+0x337F  (__onexit)
uint32_t d2multi_337F(uint32_t func) {
    z36_lock1363();
    d2multi_32D7(func);
    z36_unlock33B1();
    return func;   // __onexit_lk leaves the function in edi/eax path; retail returns its eax
}

// 1.13c D2Multi+0x33B7  (_atexit)
int32_t d2multi_33B7(uint32_t func) { return (d2multi_337F(func) != 0) - 1; }

// 1.13c D2Multi+0x33C9  (__RTC_Initialize; table is empty in retail so the loop never runs)
void d2multi_33C9(void) {
    for (uint32_t* p = DAT_6f9e81bc; p < DAT_6f9e81bc; ++p) if (*p) z36_call(*p);
}

// 1.13c D2Multi+0x3454  (__SEH_prolog, inlined helper: saves regs below the frame, links ExceptionList)
void d2multi_3454(uint32_t param_1, int32_t param_2) {
    (void)param_1; (void)param_2;   // frame-building is register/stack level; seam z36_stackSave holds ebx,esi,edi,retaddr
    z36_ExceptionList = 0;          // retail: ExceptionList = &local_8 (guessed: frame address has no linear-memory meaning)
}

// 1.13c D2Multi+0x348F  (__SEH_epilog)
void d2multi_348F(void) {
    z36_ExceptionList = P((uint32_t)(uintptr_t)z36_ebp)[-4];
    *z36_ebp = z36_retaddr;
}

// 1.13c D2Multi+0x35B0  (___ascii_stricmp)
int32_t d2multi_35B0(const uint8_t* a, const uint8_t* b) {
    uint8_t c3 = 0xff;
    for (;;) {
        do {
            if (c3 == 0) return 0;
            c3 = *b++;
            uint8_t c2 = *a++;
            if (c2 == c3) continue;
            uint8_t x = c3 + 0xbf; uint8_t l1 = x + ((x < 0x1a) ? 0x20 : 0) + 0x41;
            uint8_t y = c2 + 0xbf; uint8_t l2 = y + ((y < 0x1a) ? 0x20 : 0) + 0x41;
            if (l2 != l1) return (l2 < l1) ? -1 : 1;
        } while (true);
    }
}

// 1.13c D2Multi+0x39D4  (&_errno)
int32_t* d2multi_39D4(void) { return &z36_getptd()[2]; }

// 1.13c D2Multi+0x39DD  (&__doserrno)
uint32_t* d2multi_39DD(void) { return (uint32_t*)&z36_getptd()[3]; }

// 1.13c D2Multi+0x39E6  (__dosmaperr)
void d2multi_39E6(uint32_t e) {
    z36_getptd()[3] = (int32_t)e;
    for (uint32_t i = 0; i < 0x2d; ++i)
        if (e == DAT_6f9e9320[i * 2]) { z36_getptd()[2] = (int32_t)DAT_6f9e9320[i * 2 + 1]; return; }
    int32_t r = 0x16;
    if (e > 0x12 && e < 0x25) r = 0xd;
    else if (e > 0xbb && e < 0xcb) r = 8;
    z36_getptd()[2] = r;
}

// 1.13c D2Multi+0x3A59  (_free)
void d2multi_3A59(uint32_t mem) {
    if (!mem) return;
    if (DAT_6fa0b388 == 3) {
        z36_lock(4);
        uint32_t h = z36_sbhFindBlock(mem);
        if (h) z36_sbhFreeBlock(h, mem);
        z36_unlock3AAC();
        if (h) return;
    }
    z36_heapFree(DAT_6fa0b384, mem);
}
}
