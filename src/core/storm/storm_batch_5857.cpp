// Batch 5857: Storm VC7.1 CRT leaves (onexit / DllMainCRTStartup / tlocinfo / ptd). Retail globals and callees are seams.
#include <cstdint>
#include <initializer_list>
// ponytail: locinfo/ptd link fields are read as native pointers; retail guest pointers are 32-bit, the caller must supply a native-pointer image.

// 1.13c STORM+0x5857 __onexit_lk: append fn to the atexit table (tab..end), growing by min(msize,0x800) then +0x10 dwords.
extern "C" void storm_onexit_lk(uint32_t **tab, uint32_t **end, uint32_t fn, uint32_t (*msize)(void *),
                                uint32_t *(*realloc_)(uint32_t *, uint32_t)) {
    uint32_t sz = msize(*tab);
    if (sz < (uint32_t)((uint8_t *)*end - (uint8_t *)*tab) + 4) {
        uint32_t *n = realloc_(*tab, sz + (sz < 0x800 ? sz : 0x800));
        if (!n) { n = realloc_(*tab, sz + 0x10); if (!n) return; }
        *end = n + (*end - *tab);
        *tab = n;
    }
    *(*end)++ = fn;
}

// 1.13c STORM+0x58FF __onexit: lock, __onexit_lk, unlock.
extern "C" void storm_onexit(void (*lock)(), void (*onexit_lk)(), void (*unlock)()) { lock(); onexit_lk(); unlock(); }

// 1.13c STORM+0x59BF __CRT_INIT@12: reason 1 attach, 0 detach, 2 thread attach, 3 thread detach. Steps are seams; true = all ok.
struct StormCrtInit {
    int *initcount; int (*heap_init)(); int (*mtinit)(); int (*ioinit)(); int (*setargv)(); int (*setenvp)(); int (*cinit)(int);
    void (*ioterm)(); void (*mtterm)(); void (*heap_term)(); void (*cexit)(); int *no_cexit;
    void *(*calloc_)(uint32_t, uint32_t); int (*fls_set)(void *); void (*initptd)(void *); uint32_t (*tid)(); void (*free_)(void *); void (*freeptd)(void *);
};
extern "C" int storm_crt_init(const StormCrtInit *c, int reason) {
    if (reason == 1) {
        if (!c->heap_init()) return 0;
        if (c->mtinit()) {
            if (c->ioinit() >= 0) {
                if (c->setargv() >= 0 && c->setenvp() >= 0 && c->cinit(0) == 0) { ++*c->initcount; return 1; }
                c->ioterm();
            }
            c->mtterm();
        }
        c->heap_term();
        return 0;
    }
    if (reason == 0) {
        if (*c->initcount > 0) {
            --*c->initcount;
            if (*c->no_cexit == 0) c->cexit();
            c->ioterm(); c->mtterm(); c->heap_term();
            return 1;
        }
        return 0;
    }
    if (reason == 3) { c->freeptd(nullptr); return 1; }
    if (reason != 2) return 1;
    uint32_t *p = (uint32_t *)c->calloc_(1, 0x8c);
    if (!p) return 0;
    if (c->fls_set(p)) { c->initptd(p); p[0] = c->tid(); p[1] = 0xffffffff; return 1; }
    c->free_(p);
    return 0;
}

// 1.13c STORM+0x5B40 __DllMainCRTStartup@12: pre-hook, CRT_INIT, DllMain (entry), post-hook per reason.
extern "C" int storm_dllmain_crt_startup(int reason, int initcount, int (*hook)(), int (*crt_init)(int), int (*dllmain)()) {
    int r = 1;
    if (reason == 0 && initcount == 0) return 0;
    if (reason == 1 || reason == 2) {
        if (hook) r = hook();
        if (r == 0 || crt_init(reason) == 0) return 0;
    }
    r = dllmain();
    if (reason == 1 && r == 0) crt_init(0);
    if (reason == 0 || reason == 3) {
        if (crt_init(reason) == 0) r = 0;
        if (r != 0 && hook) r = hook();
    }
    return r;
}

// 1.13c STORM+0x5C24 __amsg_exit: banner when the app type needs it, write the message, exit(255).
extern "C" void storm_amsg_exit(int msg, int apptype, int fallback, void (*banner)(), void (*write)(int), void (*exit_)(int)) {
    if (apptype == 1 || (apptype == 0 && fallback == 1)) banner();
    write(msg);
    exit_(0xff);
}

// 1.13c STORM+0x6A53 ___freetlocinfo: free a refcount-zero locale block (offsets 0x2c..0x50 as retail).
struct StormLocFree {
    void *def_cur, *def_mon, *def_num, *def_ctype, *def_time; // DAT_6fc42748, 47b88, 47b90, 47b84, 42744
    void (*free_)(void *); void (*free_mon)(void *); void (*free_num)(void *); void (*free_time)(void *);
};
extern "C" void storm_freetlocinfo(uint8_t *l, const StormLocFree *c) {
    auto f = [&](int o) { return *(uint8_t **)(l + o); };
    if (f(0x3c) != c->def_cur && f(0x3c) && **(int **)(l + 0x2c) == 0) {
        int *p = (int *)f(0x34);
        if (p && *p == 0 && p != c->def_mon) { c->free_(p); c->free_mon(f(0x3c)); }
        p = (int *)f(0x30);
        if (p && *p == 0 && p != c->def_num) { c->free_(p); c->free_num(f(0x3c)); }
        c->free_(f(0x2c)); c->free_(f(0x3c));
    }
    int *p = (int *)f(0x40);
    if (p != c->def_ctype && p && *p == 0) { c->free_(p); c->free_(f(0x44) - 0xfe); }
    uint8_t *t = f(0x50);
    if (t != c->def_time && t && ((int *)t)[0x2d] == 0) { c->free_time(t); c->free_(f(0x50)); }
    c->free_(l);
}

// 1.13c STORM+0x6B23 ___updatetlocinfo_lk: swap the thread's locinfo (ptd+0x4c... _tfpecode) to the global one, refcounting.
extern "C" int *storm_updatetlocinfo_lk(int **slot, int *global, int *dflt, void (*freeloc)(int *)) {
    int *old = *slot;
    if (old == global) return *slot;
    auto adj = [](int *b, int d) { if (b) *b += d; };
    if (old) {
        --*old;
        adj(*(int **)(old + 0xb), -1); adj(*(int **)(old + 0xd), -1); adj(*(int **)(old + 0xc), -1); adj(*(int **)(old + 0x10), -1);
        --*(int *)(*(intptr_t *)(old + 0x13) + 0xb4);
    }
    *slot = global; ++*global;
    adj(*(int **)(global + 0xb), 1); adj(*(int **)(global + 0xd), 1); adj(*(int **)(global + 0xc), 1); adj(*(int **)(global + 0x10), 1);
    ++*(int *)(*(intptr_t *)(global + 0x13) + 0xb4);
    if (old && *old == 0 && old != dflt) freeloc(old);
    return *slot;
}

// 1.13c STORM+0x7572 ___tolower_mt: ASCII fast path, else LCMapStringA(LCMAP_LOWERCASE) via seam. loc = threadlocinfo.
struct StormToLower { int lcid, cp, c_locale, mb_cur_max; const uint8_t *ctype; int (*isctype)(int); uint32_t (*lcmap)(int lcid, const uint8_t *in, int n, uint8_t *out, int cp); };
extern "C" uint32_t storm_tolower_mt(const StormToLower *l, uint32_t c) {
    if (l->lcid == 0 || (l->c_locale && c < 0x80)) return (c > 0x40 && c < 0x5b) ? c + 0x20 : c;
    if (c < 0x100) {
        uint32_t up = l->mb_cur_max < 2 ? (l->ctype[c * 2] & 1) : (uint32_t)l->isctype((int)c);
        if (!up) return c;
    }
    uint8_t in[2], out[4] = {0}; int n;
    if ((l->ctype[1 + ((c >> 8) & 0xff) * 2] & 0x80) == 0) { in[0] = (uint8_t)c; n = 1; }
    else { in[0] = (uint8_t)(c >> 8); in[1] = (uint8_t)c; n = 2; }
    uint32_t r = l->lcmap(l->lcid, in, n, out, l->cp);
    if (!r) return c;
    return r == 1 ? out[0] : (uint32_t)(out[0] << 8 | out[1]);
}

// 1.13c STORM+0x76CA __mtterm: free the FLS/TLS index, then delete the locks.
extern "C" void storm_mtterm(uint32_t *idx, void (*fls_free)(uint32_t), void (*deletelocks)()) {
    if (*idx != 0xffffffff) { fls_free(*idx); *idx = 0xffffffff; }
    deletelocks();
}

// 1.13c STORM+0x76E7 __initptd: _initaddr = default locinfo, _holdrand = 1 (ptd+0x54, +0x14).
extern "C" void storm_initptd(uint8_t *ptd, uint32_t dflt) { *(uint32_t *)(ptd + 0x54) = dflt; *(uint32_t *)(ptd + 0x14) = 1; }

// 1.13c STORM+0x776B __freefls@4: free ptd members, drop lc refcounts under locks 0xd / 0xc, free ptd.
struct StormFreeFls {
    void (*free_)(void *); void (*lock)(int); void (*unlock_d)(); void (*unlock_c)(); void (*freeloc)(int *);
    void *dflt_ctype_tab; int *dflt_lc; int *dflt_ref; int *cur_loc;
};
extern "C" void storm_freefls(uint8_t *p, const StormFreeFls *c) {
    if (!p) return;
    for (int o : {0x24, 0x2c, 0x34, 0x3c, 0x44, 0x48}) { void *m = *(void **)(p + o); if (m) c->free_(m); }
    void *t = *(void **)(p + 0x54); if (t != c->dflt_ctype_tab) c->free_(t);
    c->lock(0xd);
    int *r = *(int **)(p + 0x60);
    if (r && (--*r == 0) && r != c->dflt_ref) c->free_(r);
    c->unlock_d();
    c->lock(0xc);
    int *l = *(int **)(p + 100);
    if (l) {
        --*l;
        for (int i : {0xb, 0xd, 0xc, 0x10}) if (l[i]) --*(int *)(intptr_t)l[i];
        --*(int *)(intptr_t)(l[0x13] + 0xb4);
        if (l != c->cur_loc && l != c->dflt_lc && *l == 0) c->freeloc(l);
    }
    c->unlock_c();
    c->free_(p);
}

// 1.13c STORM+0x78B2 __freeptd: free the calling thread's (or given) ptd and clear its FLS slot.
extern "C" void storm_freeptd(uint32_t idx, void *ptd, void *(*fls_get)(uint32_t), void (*fls_set)(uint32_t, void *), void (*freefls)(void *)) {
    if (idx == 0xffffffff) return;
    if (!ptd) ptd = fls_get(idx);
    freefls(ptd);
    fls_set(idx, nullptr);
}
