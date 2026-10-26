// D2Net CRT thread-data / SEH / RTC leaves (0x1A03-0x1E01). Same VC7.1 Release CRT as the BNClient and
// D2MCPClient copies (bnclient_batch_5906.cpp, bnclient_freefls.cpp, bnclient_mtinit_343c.cpp); the cached
// decompile for 0x1A03/0x1BE1-0x1E01 is absent, so these follow those byte-identical siblings.
// Retail globals and kernel32 calls are seams after the retail argument order.
#include <cstdint>
#include <initializer_list>

struct SehFrame { SehFrame* prev; };

// 1.13c D2Net+0x1A8C __SEH_prolog: link a new frame into the thread's exception-list head.
void d2net_seh_prolog(SehFrame** head, SehFrame* frame) { frame->prev = *head; *head = frame; }

// 1.13c D2Net+0x1AC7 __SEH_epilog: restore the saved exception-list head (frame->prev, ebp[-4]).
void d2net_seh_epilog(SehFrame** head, SehFrame* frame) { *head = frame->prev; }

// 1.13c D2NET+0x01A03 __RTC_Initialize: call each non-null entry of the (empty in this image) RTC table.
void d2net_rtc_initialize(void (**begin)(), void (**end)()) {
    for (; begin < end; ++begin)
        if (*begin) (*begin)();
}

// 1.13c D2Net+0x1BE1 sub_1BE1: the TlsAlloc wrapper __mtinit installs as the FlsAlloc fallback.
uint32_t d2net_tls_alloc_stub(uint32_t (*tls_alloc)()) { return tls_alloc(); }

struct D2NetLocks {
    uint32_t* table;                      // DAT_6fbfa038, 8-byte entries: {cs ptr, flag}
    uint32_t count;                       // 0x24
    void (*delete_cs)(void*);
    void (*free_)(void*);                 // 0x1EF0 _free
};

// 1.13c D2Net+0x1BEA __mtterm: free the TLS/FLS index if allocated, then delete the lock table (0x17D6).
void d2net_mtterm(int32_t* tls_index, int32_t (*tls_free)(int32_t), const D2NetLocks* l) {
    if (*tls_index != -1) { tls_free(*tls_index); *tls_index = -1; }
    for (uint32_t i = 0; i < l->count; ++i) {
        uint32_t* s = l->table + i * 2;
        if (s[0] && s[1] != 1) { l->delete_cs((void*)(uintptr_t)s[0]); l->free_((void*)(uintptr_t)s[0]); s[0] = 0; }
    }
    for (uint32_t i = 0; i < l->count; ++i) {
        uint32_t* s = l->table + i * 2;
        if (s[0] && s[1] == 1) l->delete_cs((void*)(uintptr_t)s[0]);
    }
}

// 1.13c D2Net+0x1C07 __initptd: seed _initaddr (+0x54) and _holdrand (+0x14).
extern "C" void d2net_initptd(uint8_t* ptd, uint32_t initaddr) {
    *(uint32_t*)(ptd + 0x54) = initaddr;
    *(uint32_t*)(ptd + 0x14) = 1;
}

struct D2NetPtdEnv {
    int32_t  tls_index;                           // DAT index
    void*    (*get)(uint32_t);                    // FlsGetValue/TlsGetValue
    int32_t  (*set)(uint32_t, void*);             // FlsSetValue/TlsSetValue
    void*    (*calloc_)(uint32_t, uint32_t);      // _calloc
    uint32_t (*tid)();                            // GetCurrentThreadId
    void     (*amsg_exit)(int32_t);               // 0x175A
    uint32_t initaddr;                            // retail RVA stored at +0x54
};

// 1.13c D2Net+0x1C1A __getptd: the thread's _tiddata, created (0x8C bytes) on first use; failure -> __amsg_exit(0x10).
void* d2net_getptd(const D2NetPtdEnv* e) {
    void* p = e->get((uint32_t)e->tls_index);
    if (!p) {
        p = e->calloc_(1, 0x8C);
        if (!p || !e->set((uint32_t)e->tls_index, p)) e->amsg_exit(0x10);
        d2net_initptd((uint8_t*)p, e->initaddr);
        *(uint32_t*)((uint8_t*)p + 4) = 0xFFFFFFFFu;
        *(uint32_t*)p = e->tid();
    }
    return p;
}

struct D2NetFreeFlsEnv {
    uint32_t default_locale_a, default_locale_b, static_locale, initaddr;
    void (*free_)(void*);
    void (*lock)(int32_t);
    void (*unlock)(int32_t);
    void (*free_locale)(uint32_t);
};

// 1.13c D2Net+0x1C8B __freefls@4: free the owned strings, drop both locale refs under locks 0xD/0xC, free the block.
void d2net_freefls(uint32_t td, const D2NetFreeFlsEnv* e) {
    if (!td) return;
    auto f = [td](uint32_t o) { return (uint32_t*)(uintptr_t)(td + o); };
    auto vp = [](uint32_t a) { return (void*)(uintptr_t)a; };
    for (uint32_t o : {0x24u, 0x2Cu, 0x34u, 0x3Cu, 0x44u, 0x48u}) if (*f(o)) e->free_(vp(*f(o)));
    if (*f(0x54) != e->initaddr) e->free_(vp(*f(0x54)));
    e->lock(0xD);
    if (uint32_t l = *f(0x60)) { if (--*(int32_t*)vp(l) == 0 && l != e->default_locale_a) e->free_(vp(l)); }
    e->unlock(0xD);
    e->lock(0xC);
    if (uint32_t l = *f(0x64)) {
        int32_t* r = (int32_t*)vp(l);
        --*r;
        for (uint32_t o : {0x2Cu, 0x34u, 0x30u, 0x40u})
            if (uint32_t q = *(uint32_t*)vp(l + o)) --*(int32_t*)vp(q);
        --*(int32_t*)vp(*(uint32_t*)vp(l + 0x4C) + 0xB4);
        if (l != e->default_locale_b && l != e->static_locale && *r == 0) e->free_locale(l);
    }
    e->unlock(0xC);
    e->free_(vp(td));
}

// 1.13c D2Net+0x1DBD sub_1DBD: __unlock(0xD).
void d2net_unlock_d(void (*unlock)(int32_t)) { unlock(0xD); }

// 1.13c D2Net+0x1DC9 sub_1DC9: __unlock(0xC).
void d2net_unlock_c(void (*unlock)(int32_t)) { unlock(0xC); }

// 1.13c D2Net+0x1DD2 __freeptd: with a live index, fetch the thread's block if none passed, freefls it, clear the slot.
void d2net_freeptd(void* ptd, int32_t idx, void* (*get)(uint32_t), void (*set)(uint32_t, void*), void (*freefls)(void*)) {
    if (idx == -1) return;
    if (!ptd) ptd = get((uint32_t)idx);
    freefls(ptd);
    set((uint32_t)idx, nullptr);
}

struct D2NetMtInitEnv {
    int32_t (*mtinitlocks)();                     // 0x178D
    void    (*mtterm)();                          // 0x1BEA
    void*   (*get_proc)(const char*);             // GetProcAddress(kernel32)
    void*   (**fls_alloc)(void*); void* (**fls_get)(uint32_t); int32_t (**fls_set)(uint32_t, void*); int32_t (**fls_free)(uint32_t);
    void*   (*tls_alloc_stub)(void*); void* (*tls_get)(uint32_t); int32_t (*tls_set)(uint32_t, void*); int32_t (*tls_free)(uint32_t);
    void*   freefls;
    int32_t* idx;
    D2NetPtdEnv ptd;
    uint32_t initaddr;
};

// 1.13c D2Net+0x1E01 __mtinit: lock table up, resolve Fls* (Tls* fallback), allocate the slot and the first _tiddata.
int32_t d2net_mtinit(const D2NetMtInitEnv* e) {
    if (!e->mtinitlocks()) { e->mtterm(); return 0; }
    if (e->get_proc) {
        *e->fls_alloc = (void*(*)(void*))e->get_proc("FlsAlloc");
        *e->fls_get   = (void*(*)(uint32_t))e->get_proc("FlsGetValue");
        *e->fls_set   = (int32_t(*)(uint32_t, void*))e->get_proc("FlsSetValue");
        *e->fls_free  = (int32_t(*)(uint32_t))e->get_proc("FlsFree");
        if (!*e->fls_get) {
            *e->fls_get = e->tls_get; *e->fls_set = e->tls_set;
            *e->fls_alloc = e->tls_alloc_stub; *e->fls_free = e->tls_free;
        }
    }
    *e->idx = (int32_t)(uintptr_t)(*e->fls_alloc)(e->freefls);
    if (*e->idx != -1) {
        uint32_t* p = (uint32_t*)e->ptd.calloc_(1, 0x8C);
        if (p && (*e->fls_set)((uint32_t)*e->idx, p)) {
            d2net_initptd((uint8_t*)p, e->initaddr);
            p[1] = 0xFFFFFFFFu;
            p[0] = e->ptd.tid();
            return 1;
        }
    }
    e->mtterm();
    return 0;
}
