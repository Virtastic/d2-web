// src/core/d2multi/d2multi_crt_2bfc.cpp -- D2Multi.dll CRT locale-info / per-thread-data lifetime (0x2BFC-0x2FB9).
// Struct fields are named by their 1.13c (x86) offsets; pointers are native width. Win32/CRT calls go through weak seams.
#include <cstdint>
#include <cstdlib>

#define SEAM extern "C" __attribute__((weak))
SEAM void d2multi_seam_lock(int n);              // __lock
SEAM void d2multi_seam_unlock(int n);            // FUN_6f9d31f1
SEAM uint32_t d2multi_seam_tlsalloc(void);       // TlsAlloc
SEAM void* d2multi_seam_tlsget(uint32_t idx);    // DAT_6f9ebaec
SEAM int d2multi_seam_tlsset(uint32_t idx, void* v);  // DAT_6f9ebaf0
SEAM void d2multi_seam_tlsfree(uint32_t idx);    // DAT_6f9ebaf4
SEAM void d2multi_seam_mtdeletelocks(void);
SEAM void d2multi_seam_amsg_exit(int n);
SEAM void d2multi_seam_free_lconv_mon(void* lconv);
SEAM void d2multi_seam_free_lconv_num(void* lconv);
SEAM void d2multi_seam_free_lc_time(void* t);
SEAM uint32_t d2multi_seam_getlasterror(void);
SEAM void d2multi_seam_setlasterror(uint32_t e);
SEAM uint32_t d2multi_seam_getcurrentthreadid(void);

struct d2multi_lc4c { char pad[0xb4]; int cnt; };  // +0xb4 counter
struct d2multi_locinfo {  // threadlocinfo, fields by 1.13c offset
    int refcount;          // +0x00
    int* r2c; int* r30; int* r34;  // +0x2c/+0x30/+0x34 refcount pointers
    void* lconv;           // +0x3c
    int* r40;              // +0x40
    char* ctype;           // +0x44
    d2multi_lc4c* p4c;     // +0x4c
    int* time50;           // +0x50 (lc_time; refcount at int index 0x2d)
};
struct d2multi_ptd {  // _tiddata, fields by 1.13c offset
    uint32_t tid, thandle;           // +0x00/+0x04
    void* initaddr;                  // +0x14
    uint32_t holdrand;               // +0x14 (collapsed)
    void *p24, *p2c, *p34, *p3c, *p44, *p48, *p54;
    int* p60;                        // +0x60 mbcinfo refcount
    d2multi_locinfo* locinfo;        // +0x64 (_tfpecode)
};

// globals: PTR_DAT_6f9e90ac current locinfo, DAT_6f9e9058 the static C locinfo, others are static defaults
d2multi_locinfo g_d2multi_locinfo_c;
d2multi_locinfo* g_d2multi_locinfo = &g_d2multi_locinfo_c;
void* g_d2multi_c_lconv; int* g_d2multi_c_mon; int* g_d2multi_c_num; int* g_d2multi_c_time_ref; int* g_d2multi_c_ctype_ref;
int* g_d2multi_c_mbc;  // DAT_6fa0a12c
char g_d2multi_c_ctype[1];  // DAT_6f9e94c0 default
uint32_t g_d2multi_tlsidx = (uint32_t)-1;  // DAT_6f9e91e4
int* g_d2multi_c_time;  // DAT_6f9ebd6c

// 1.13c D2Multi+0x2BFC  ___freetlocinfo
void d2multi_freetlocinfo(d2multi_locinfo* p) {
    if (p->lconv != g_d2multi_c_lconv && p->lconv && *p->r2c == 0) {
        int* m = p->r34;
        if (m && *m == 0 && m != g_d2multi_c_mon) { free(m); d2multi_seam_free_lconv_mon(p->lconv); }
        int* n = p->r30;
        if (n && *n == 0 && n != g_d2multi_c_num) { free(n); d2multi_seam_free_lconv_num(p->lconv); }
        free(p->r2c);
        free(p->lconv);
    }
    int* c = p->r40;
    if (c != g_d2multi_c_ctype_ref && c && *c == 0) { free(c); free(p->ctype - 0xfe); }
    int* t = p->time50;
    if (t != g_d2multi_c_time && t && t[0x2d] == 0) { d2multi_seam_free_lc_time(t); free(p->time50); }
    free(p);
}

static void dec_refs(d2multi_locinfo* p) {
    p->refcount--;
    if (p->r2c) (*p->r2c)--;
    if (p->r34) (*p->r34)--;
    if (p->r30) (*p->r30)--;
    if (p->r40) (*p->r40)--;
    p->p4c->cnt--;
}

// 1.13c D2Multi+0x2E01  __getptd
d2multi_ptd* d2multi_getptd(void) {
    uint32_t err = d2multi_seam_getlasterror();
    d2multi_ptd* p = (d2multi_ptd*)d2multi_seam_tlsget(g_d2multi_tlsidx);
    if (!p) {
        p = (d2multi_ptd*)calloc(1, sizeof(d2multi_ptd));
        if (p && d2multi_seam_tlsset(g_d2multi_tlsidx, p)) {
            p->initaddr = g_d2multi_c_ctype; p->holdrand = 1;
            p->thandle = 0xffffffff; p->tid = d2multi_seam_getcurrentthreadid();
        } else d2multi_seam_amsg_exit(0x10);
    }
    d2multi_seam_setlasterror(err);
    return p;
}

// 1.13c D2Multi+0x2CCC  ___updatetlocinfo_lk
d2multi_locinfo* d2multi_updatetlocinfo_lk(void) {
    d2multi_ptd* t = d2multi_getptd();
    d2multi_locinfo* old = t->locinfo;
    if (old != g_d2multi_locinfo) {
        if (old) dec_refs(old);
        d2multi_locinfo* n = g_d2multi_locinfo;
        t->locinfo = n;
        n->refcount++;
        if (n->r2c) (*n->r2c)++;
        if (n->r34) (*n->r34)++;
        if (n->r30) (*n->r30)++;
        if (n->r40) (*n->r40)++;
        n->p4c->cnt++;
        if (old && old->refcount == 0 && old != &g_d2multi_locinfo_c) d2multi_freetlocinfo(old);
    }
    return t->locinfo;
}

// 1.13c D2Multi+0x2DBF  sub_2DBF
void d2multi_unlock_c(void) { d2multi_seam_unlock(0xc); }
// 1.13c D2Multi+0x2FA4  sub_2FA4
void d2multi_unlock_d(void) { d2multi_seam_unlock(0xd); }
// 1.13c D2Multi+0x2FB0  sub_2FB0
void d2multi_unlock_c2(void) { d2multi_seam_unlock(0xc); }

// 1.13c D2Multi+0x2D8D  ___updatetlocinfo
d2multi_locinfo* d2multi_updatetlocinfo(void) {
    d2multi_seam_lock(0xc);
    d2multi_locinfo* r = d2multi_updatetlocinfo_lk();
    d2multi_unlock_c();
    return r;
}

// 1.13c D2Multi+0x2DC8  sub_2DC8
void d2multi_tlsalloc(void) { d2multi_seam_tlsalloc(); }

// 1.13c D2Multi+0x2DD1  __mtterm
void d2multi_mtterm(void) {
    if (g_d2multi_tlsidx != (uint32_t)-1) { d2multi_seam_tlsfree(g_d2multi_tlsidx); g_d2multi_tlsidx = (uint32_t)-1; }
    d2multi_seam_mtdeletelocks();
}

// 1.13c D2Multi+0x2DEE  __initptd
void d2multi_initptd(d2multi_ptd* p, void* /*locale*/) { p->initaddr = g_d2multi_c_ctype; p->holdrand = 1; }

// 1.13c D2Multi+0x2E72  __freefls@4
void d2multi_freefls(d2multi_ptd* p) {
    if (!p) return;
    free(p->p24); free(p->p2c); free(p->p34); free(p->p3c); free(p->p44); free(p->p48);
    if (p->p54 != (void*)g_d2multi_c_ctype) free(p->p54);
    d2multi_seam_lock(0xd);
    int* m = p->p60;
    if (m && --*m == 0 && m != g_d2multi_c_mbc) free(m);
    d2multi_unlock_d();
    d2multi_seam_lock(0xc);
    d2multi_locinfo* l = p->locinfo;
    if (l) {
        dec_refs(l);
        if (l != g_d2multi_locinfo && l != &g_d2multi_locinfo_c && l->refcount == 0) d2multi_freetlocinfo(l);
    }
    d2multi_unlock_c2();
    free(p);
}

// 1.13c D2Multi+0x2FB9  __freeptd
void d2multi_freeptd(d2multi_ptd* p) {
    if (g_d2multi_tlsidx == (uint32_t)-1) return;
    if (!p) p = (d2multi_ptd*)d2multi_seam_tlsget(g_d2multi_tlsidx);
    d2multi_freefls(p);
    d2multi_seam_tlsset(g_d2multi_tlsidx, nullptr);
}
