// Fog CRT leaves (VC7.1 Release): fclose_lk, floor, __CRT_INIT, amsg_exit, locale/ptd/mt init-term.
// Same CRT as d2net_crt_thread.cpp / bnclient_freefls.cpp; retail globals and callees are seams
// passed by the caller, argument order after retail.
#include <cstdint>

// 1.13c FOG+0x2BBF __fclose_lk
struct FogFile { char* ptr; int32_t cnt; char* base; int32_t flag, file, charbuf, bufsiz; char* tmpfname; };
int32_t fog_fclose_lk(FogFile* f, int32_t (*flush)(FogFile*), void (*freebuf)(FogFile*),
                      int32_t (*close_)(int32_t), void (*free_)(void*)) {
    int32_t r = -1;
    if (f->flag & 0x83) {
        r = flush(f);
        freebuf(f);
        if (close_(f->file) < 0) r = -1;
        else if (f->tmpfname) { free_(f->tmpfname); f->tmpfname = nullptr; }
    }
    f->flag = 0;
    return r;
}

// 1.13c FOG+0x2EC0 floor (SSE2 fast path, bit-exact on the IEEE pattern; NaN/Inf/huge pass through)
uint64_t fog_floor_bits(uint64_t b) {
    uint32_t e = (b >> 52) & 0x7ff; bool neg = b >> 63;
    if (e < 0x3ff) return neg ? ((b << 1) ? 0xBFF0000000000000ull : b) : 0;
    if (e >= 0x433) return b;
    uint64_t frac = (1ull << (52 - (e - 0x3ff))) - 1, t = b & ~frac;
    return (neg && (b & frac)) ? t + frac + 1 : t;
}

struct FogMtSeams {
    int32_t* fls_index;                         // DAT_6ff7e20c
    int32_t (*fls_free)(int32_t);               // DAT_6ff8027c
    int32_t (*fls_set)(int32_t, void*);         // DAT_6ff80278
    void* (*fls_get)(int32_t);                  // DAT_6ff80274
    void (*freefls)(void*);                     // 0x393C
    void (*mtdeletelocks)();                    // 0x3BEA
};

// 1.13c FOG+0x389B __mtterm
void fog_mtterm(const FogMtSeams* s) {
    if (*s->fls_index != -1) { s->fls_free(*s->fls_index); *s->fls_index = -1; }
    s->mtdeletelocks();
}

// 1.13c FOG+0x38B8 __initptd
struct FogPtd { uint32_t tid, thandle; uint8_t pad1[0x0c]; uint32_t holdrand; uint8_t pad2[0x3c]; uint32_t initaddr; };
void fog_initptd(FogPtd* p, uint32_t retail_init_addr) { p->initaddr = retail_init_addr; p->holdrand = 1; }

// 1.13c FOG+0x3A83 __freeptd
void fog_freeptd(const FogMtSeams* s, void* ptd) {
    if (*s->fls_index != -1) {
        if (!ptd) ptd = s->fls_get(*s->fls_index);
        s->freefls(ptd);
        s->fls_set(*s->fls_index, nullptr);
    }
}

// 1.13c FOG+0x3246 __amsg_exit
void fog_amsg_exit(int32_t msg, int32_t mode_a, int32_t mode_b, void (*msgbanner)(), void (*write_msg)(int32_t),
                   void (*exit_)(int32_t)) {
    if (mode_a == 1 || (mode_a == 0 && mode_b == 1)) msgbanner();
    write_msg(msg);
    exit_(0xff);
}

// 1.13c FOG+0x36C6 ___freetlocinfo (fields by retail offset; counters are the shared refcounts)
void fog_freetlocinfo(uint8_t* l, const uint32_t* defs /*{0x40,0x50,0x34,0x30,0x3c defaults} 5 entries*/, const uint32_t* c_locale_mon_ptr,
                      void (*free_)(void*), void (*free_lconv_mon)(void*), void (*free_lconv_num)(void*),
                      void (*free_lc_time)(void*)) {
    auto P = [&](int o) { return *(uint8_t**)(l + o); };
    auto I = [&](uint8_t* p, int o) { return *(int32_t*)(p + o); };
    (void)c_locale_mon_ptr;
    if (*(uint8_t**)(l + 0x3c) != (uint8_t*)(uintptr_t)defs[4] && P(0x3c) && *(int32_t*)P(0x2c) == 0) {
        uint8_t* p = P(0x34);
        if (p && *(int32_t*)p == 0 && p != (uint8_t*)(uintptr_t)defs[2]) { free_(p); free_lconv_mon(P(0x3c)); }
        p = P(0x30);
        if (p && *(int32_t*)p == 0 && p != (uint8_t*)(uintptr_t)defs[3]) { free_(p); free_lconv_num(P(0x3c)); }
        free_(P(0x2c)); free_(P(0x3c));
    }
    uint8_t* p = P(0x40);
    if (p != (uint8_t*)(uintptr_t)defs[0] && p && *(int32_t*)p == 0) { free_(p); free_(P(0x44) - 0xfe); }
    uint8_t* t = P(0x50);
    if (t != (uint8_t*)(uintptr_t)defs[1] && t && I(t, 0xb4) == 0) { free_lc_time(t); free_(P(0x50)); }
    free_(l);
}

// 1.13c FOG+0x3796 ___updatetlocinfo_lk: swap the thread's locinfo for the global one, moving refcounts.
void fog_updatetlocinfo_lk(int32_t** slot /*ptd->_tfpecode*/, int32_t* global, int32_t* c_locinfo,
                           void (*freetlocinfo)(void*)) {
    int32_t* old = *slot;
    if (old == global) return;
    auto dec = [](int32_t* p) { if (p) --*p; };
    auto inc = [](int32_t* p) { if (p) ++*p; };
    auto sub = [](int32_t* l, int o) { return *(int32_t**)((uint8_t*)l + o); };
    if (old) {
        --*old; dec(sub(old, 0x2c)); dec(sub(old, 0x34)); dec(sub(old, 0x30)); dec(sub(old, 0x40));
        --*(int32_t*)((uint8_t*)sub(old, 0x4c) + 0xb4);
    }
    *slot = global; ++*global;
    inc(sub(global, 0x2c)); inc(sub(global, 0x34)); inc(sub(global, 0x30)); inc(sub(global, 0x40));
    ++*(int32_t*)((uint8_t*)sub(global, 0x4c) + 0xb4);
    if (old && *old == 0 && old != c_locinfo) freetlocinfo(old);
}

// 1.13c FOG+0x3BA1 __mtinitlocks: 0x24 {cs, flag} slots; flag==1 slots get a static 0x18-byte CS.
int32_t fog_mtinitlocks(uint32_t* tbl, uint8_t* cs_pool, uintptr_t (*pool_ptr_to_u32)(uint8_t*),
                        int32_t (*init_cs)(uint8_t*, uint32_t)) {
    for (int i = 0; i < 0x24; ++i)
        if (tbl[i * 2 + 1] == 1) {
            uint8_t* cs = cs_pool; cs_pool += 0x18;
            tbl[i * 2] = (uint32_t)pool_ptr_to_u32(cs);
            if (!init_cs(cs, 4000)) { tbl[i * 2] = 0; return 0; }
        }
    return 1;
}

// 1.13c FOG+0x3BEA __mtdeletelocks: delete+free dynamic locks, then delete the static (flag==1) ones.
void fog_mtdeletelocks(uint32_t* tbl, void (*delete_cs)(void*), void (*free_)(void*)) {
    for (int i = 0; i < 0x24; ++i) {
        uint32_t* s = tbl + i * 2;
        if (s[0] && s[1] != 1) { delete_cs((void*)(uintptr_t)s[0]); free_((void*)(uintptr_t)s[0]); s[0] = 0; }
    }
    for (int i = 0; i < 0x24; ++i) {
        uint32_t* s = tbl + i * 2;
        if (s[0] && s[1] == 1) delete_cs((void*)(uintptr_t)s[0]);
    }
}

// 1.13c FOG+0x3AB2 __mtinit: init locks, resolve Fls*/Tls* fallbacks, allocate the first ptd (0x8c bytes).
struct FogMtInit {
    int32_t (*initlocks)();  void (*mtterm)();
    int32_t (*fls_alloc)(void*); int32_t (*fls_set)(int32_t, void*);
    void* (*calloc_)(uint32_t, uint32_t); int32_t* fls_index; void* freefls;
    uint32_t (*thread_id)(); uint32_t init_addr;
};
int32_t fog_mtinit(const FogMtInit* m) {
    if (!m->initlocks()) { m->mtterm(); return 0; }
    *m->fls_index = m->fls_alloc(m->freefls);
    if (*m->fls_index != -1) {
        uint32_t* p = (uint32_t*)m->calloc_(1, 0x8c);
        if (p && m->fls_set(*m->fls_index, p)) {
            p[0x15] = m->init_addr; p[5] = 1; p[1] = 0xffffffff; p[0] = m->thread_id();
            return 1;
        }
    }
    m->mtterm();
    return 0;
}

// 1.13c FOG+0x2FE1 __CRT_INIT@12: reason 1 attach, 0 detach, 2 thread attach, 3 thread detach.
struct FogCrtInit {
    int32_t (*attach)();      // GetVersionEx, __heap_init, __mtinit, ..., __cinit(0); 1 on success, unwinds on failure
    int32_t* init_count;      // DAT_6ff8024c
    int32_t no_cexit;         // DAT_6ff80240
    void (*cexit)(); void (*ioterm)(); void (*mtterm)(); void (*heap_term)();
    void (*freeptd)(void*);
    void* (*calloc_)(uint32_t, uint32_t); int32_t (*fls_set)(void*); void (*initptd)(void*);
    uint32_t (*thread_id)(); void (*free_)(void*);
};
int32_t fog_crt_init(const FogCrtInit* c, int32_t reason) {
    if (reason == 1) return c->attach();
    if (reason == 0) {
        if (*c->init_count <= 0) return 0;
        --*c->init_count;
        if (!c->no_cexit) c->cexit();
        c->ioterm(); c->mtterm(); c->heap_term();
        return 1;
    }
    if (reason == 3) { c->freeptd(nullptr); return 1; }
    if (reason != 2) return 1;
    uint32_t* p = (uint32_t*)c->calloc_(1, 0x8c);
    if (!p) return 0;
    if (c->fls_set(p)) { c->initptd(p); p[1] = 0xffffffff; p[0] = c->thread_id(); return 1; }
    c->free_(p);
    return 0;
}
