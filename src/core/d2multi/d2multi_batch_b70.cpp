// D2Multi MSVC CRT batch b70. Pointer-sized fields are void*/char* (wasm64), not the retail 4-byte slots.
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#define W __attribute__((weak))

struct d2multi_FILE { char* ptr; int cnt; char* base; int flag; int file; int charbuf; int bufsiz; char* tmpfname; };

extern int d2multi_DAT_6f9ebd60 W;
extern unsigned d2multi_DAT_6fa0b38c W;
extern char** d2multi_DAT_6fa0b3a0 W;   // table of 32-entry ioinfo blocks, 0x24 bytes per entry
extern int d2multi_DAT_6f9ebe38 W;
extern char** d2multi_PTR_PTR_6f9e996c W;
extern char* d2multi_PTR_DAT_6f9e993c W; extern char* d2multi_PTR_DAT_6f9e9940 W; extern char* d2multi_PTR_DAT_6f9e9944 W;
extern char* d2multi_PTR_DAT_6f9e9948 W; extern char* d2multi_PTR_DAT_6f9e994c W; extern char* d2multi_PTR_DAT_6f9e9950 W;
extern char* d2multi_PTR_DAT_6f9e9954 W; extern char* d2multi_PTR_DAT_6f9e9958 W; extern char* d2multi_PTR_DAT_6f9e995c W;
extern char* d2multi_PTR_DAT_6f9e9960 W;

extern void* d2multi_malloc(size_t) W;
extern void d2multi_free(void*) W;
extern void* d2multi_heap_alloc(size_t) W;
extern int d2multi_callnewh(size_t) W;
extern void d2multi_unlock(int) W;
extern int* d2multi_errno_ptr() W;
extern int d2multi_WideCharToMultiByte(unsigned, unsigned, const uint16_t*, int, char*, int, const char*, int*) W;
extern void* d2multi_getptd() W;
extern char* d2multi_updatetlocinfo() W;

// 1.13c D2Multi+0x49C3
void d2multi_getbuf(d2multi_FILE* f) {
    d2multi_DAT_6f9ebd60++;
    char* p = (char*)d2multi_malloc(0x1000);
    f->base = p;
    if (!p) { f->flag |= 4; f->base = (char*)&f->charbuf; f->bufsiz = 2; }
    else { f->flag |= 8; f->bufsiz = 0x1000; }
    f->cnt = 0;
    f->ptr = f->base;
}

// 1.13c D2Multi+0x4A07
int d2multi_isatty(int h) {
    if (d2multi_DAT_6fa0b38c <= (unsigned)h) return 0;
    return (int)*(char*)(d2multi_DAT_6fa0b3a0[h >> 5] + 4 + (h & 0x1f) * 0x24) & 0x40;
}

// 1.13c D2Multi+0x4BA0
size_t d2multi_strlen(const char* s) { size_t n = 0; while (s[n]) n++; return n; }

// 1.13c D2Multi+0x4C9D
void d2multi_heap_unlock4() { d2multi_unlock(4); }

// 1.13c D2Multi+0x4CA6
void* d2multi_nh_malloc(size_t size, int nhFlag) {
    if (size < 0xffffffe1u) {
        do {
            void* p = d2multi_heap_alloc(size);
            if (p) return p;
            if (nhFlag == 0) return nullptr;
        } while (d2multi_callnewh(size) != 0);
    }
    return nullptr;
}

// 1.13c D2Multi+0x4CD2
void* d2multi_malloc_wrap(size_t size) { return d2multi_nh_malloc(size, d2multi_DAT_6f9ebe38); }

// 1.13c D2Multi+0x4CE4  (param_1 = locinfo: +4 codepage, +0x14 is-C-locale flag, +0x28 mb_cur_max)
int d2multi_wctomb_mt(const char* loc, char* mb, uint16_t wc) {
    if (!mb) return 0;
    if (*(const int*)(loc + 0x14) == 0) {
        if (wc < 0x100) { *mb = (char)wc; return 1; }
    } else {
        int usedDefault = 0;
        int n = d2multi_WideCharToMultiByte(*(const unsigned*)(loc + 4), 0, &wc, 1, mb,
                                            *(const int*)(loc + 0x28), nullptr, &usedDefault);
        if (n != 0 && usedDefault == 0) return n;
    }
    *d2multi_errno_ptr() = 0x2a;
    return -1;
}

// 1.13c D2Multi+0x4D44  (ptd->_tfpecode holds the locinfo; PTR_DAT_6f9e90ac is the current global one)
extern char* d2multi_PTR_DAT_6f9e90ac W;
int d2multi_wctomb(char* mb, uint16_t wc) {
    char* li = *(char**)((char*)d2multi_getptd() + 0x8);  // ponytail: offset of _tfpecode unverified on wasm64
    if (li != d2multi_PTR_DAT_6f9e90ac) li = d2multi_updatetlocinfo();
    return d2multi_wctomb_mt(li, mb, wc);
}

// 1.13c D2Multi+0x4F25  (order of frees is retail's: [1..6],[0],[8..0xd],[7],[0xe..0x2a])
void d2multi_free_lc_time(void** p) {
    if (!p) return;
    static const int order[] = {1,2,3,4,5,6,0,8,9,10,11,12,13,7};
    for (int i : order) d2multi_free(p[i]);
    for (int i = 0xe; i <= 0x2a; i++) d2multi_free(p[i]);
}

static void freeUnlessDefault(char* v, char* a, char* b) { if (v != a && v != b) d2multi_free(v); }

// 1.13c D2Multi+0x50B5
void d2multi_free_lconv_num(char** p) {
    if (!p) return;
    freeUnlessDefault(p[0], d2multi_PTR_PTR_6f9e996c[0], d2multi_PTR_DAT_6f9e993c);
    freeUnlessDefault(p[1], d2multi_PTR_PTR_6f9e996c[1], d2multi_PTR_DAT_6f9e9940);
    freeUnlessDefault(p[2], d2multi_PTR_PTR_6f9e996c[2], d2multi_PTR_DAT_6f9e9944);
}

// 1.13c D2Multi+0x5114  (retail byte offsets 0xc..0x24 step 4 -> pointer slots 3..9)
void d2multi_free_lconv_mon(char** p) {
    if (!p) return;
    char* defs[] = {d2multi_PTR_DAT_6f9e9948, d2multi_PTR_DAT_6f9e994c, d2multi_PTR_DAT_6f9e9950, d2multi_PTR_DAT_6f9e9954,
                    d2multi_PTR_DAT_6f9e9958, d2multi_PTR_DAT_6f9e995c, d2multi_PTR_DAT_6f9e9960};
    for (int i = 0; i < 7; i++) freeUnlessDefault(p[3 + i], d2multi_PTR_PTR_6f9e996c[3 + i], defs[i]);
}

// 1.13c D2Multi+0x51F0
char* d2multi_strcpy(char* dst, const char* src) { char* d = dst; while ((*d++ = *src++)) {} return dst; }
