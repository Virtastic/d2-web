// src/core/d2sound/d2sound_crt_n23.cpp -- D2Sound.dll MSVC7.1 CRT startup/teardown. Win32 seams (heap, FLS,
// critical sections, handles, message box) are passed as hooks; the data is plain memory.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <initializer_list>

typedef int (*d2s_initfn)();
typedef void (*d2s_voidfn)();

// 1.13c D2sound+0x5623  __security_error: user handler if set, else banner text built and shown, then exit(3)
void d2s_security_error(int kind, void (*handler)(), const char* progname, void (*msgbox)(const char*), void (*exit_)(int)) {
    if (!handler) {
        const char* msg = kind == 1 ? "Buffer overrun detected!" : "Unknown security failure detected!";
        char prog[0x104 + 1]; // GetModuleFileNameA; empty name -> "<program name unknown>"
        strncpy(prog, progname && *progname ? progname : "<program name unknown>", 0x104); prog[0x104] = 0;
        const char* p = prog;
        size_t n = strlen(p);
        char tail[0x40];
        if (n + 11 > 0x3c) { snprintf(tail, sizeof tail, "...%s", p + n - (0x3c - 11 - 3)); p = tail; }
        char buf[0x300];
        snprintf(buf, sizeof buf, "%s\n\nProgram: %s\n\n%s", msg, p, kind == 1 ? "" : "");
        if (msgbox) msgbox(buf);
    } else handler();
    exit_(3);
    for (;;) {}
}

// 1.13c D2sound+0x105A  __cinit: fp init, C initializers (stop on first nonzero), atexit hook, C++ initializers
int d2s_cinit(int a, void (*fpmath)(int), d2s_initfn* ci_b, d2s_initfn* ci_e, d2s_voidfn* cx_b, d2s_voidfn* cx_e, void (*reg_atexit)()) {
    if (fpmath) fpmath(a);
    int r = 0;
    for (d2s_initfn* p = ci_b; p < ci_e; p++) {
        if (r) return r;
        if (*p) r = (*p)();
    }
    if (r == 0) {
        reg_atexit();
        for (d2s_voidfn* p = cx_b; p < cx_e; p++) if (*p) (*p)();
    }
    return r;
}

// 1.13c D2sound+0x11DA  __CRT_INIT@12(hinst, reason): 1 attach, 0 detach, 2 thread attach, 3 thread detach
struct d2s_crt_hooks {
    int (*heap_init)(); int (*mtinit)(); int (*ioinit)(); int (*setargv)(); int (*setenvp)(); int (*cinit)(int);
    void (*ioterm)(); void (*mtterm)(); void (*heap_term)(); void (*cexit)(); void (*freeptd)();
    int (*ptd_attach)(); // calloc ptd, FlsSetValue, __initptd; 0 on failure
    int (*getversion)(); // 0 on failure
};
static int g_crt_refs, g_crt_dont_cexit;
int d2s_crt_init(const d2s_crt_hooks* h, int reason) {
    switch (reason) {
    case 1:
        if (h->getversion() && h->heap_init()) {
            if (h->mtinit()) {
                if (h->ioinit() >= 0) {
                    if (h->setargv() >= 0 && h->setenvp() >= 0 && h->cinit(0) == 0) { g_crt_refs++; return 1; }
                    h->ioterm();
                }
                h->mtterm();
            }
            h->heap_term();
        }
        return 0;
    case 0:
        if (g_crt_refs > 0) {
            g_crt_refs--;
            if (!g_crt_dont_cexit) h->cexit();
            h->ioterm(); h->mtterm(); h->heap_term();
            return 1;
        }
        return 0;
    case 2: return h->ptd_attach() ? 1 : 0;
    case 3: h->freeptd(); return 1;
    }
    return 1;
}

// 1.13c D2sound+0x14BB  __mtdeletelocks: delete non-static locks (and free), then static ones (kind 1)
struct d2s_lock { void* cs; int kind; };
void d2s_mtdeletelocks(d2s_lock* t, d2s_lock* e, void (*del)(void*), void (*free_)(void*)) {
    for (d2s_lock* p = t; p < e; p++) if (p->cs && p->kind != 1) { del(p->cs); free_(p->cs); p->cs = nullptr; }
    for (d2s_lock* p = t; p < e; p++) if (p->cs && p->kind == 1) del(p->cs);
}

// 1.13c D2sound+0x15F6  __onexit_lk(fn): append to the atexit table; cap is _msize (bytes), grows by min(cap,0x800), else +0x10
struct d2s_exit_tab { void** begin; void** end; size_t cap; };
void d2s_onexit_lk(d2s_exit_tab* t, void* fn) {
    if (t->cap < (size_t)((char*)t->end - (char*)t->begin) + 4) {
        size_t add = t->cap < 0x800 ? t->cap : 0x800, off = t->end - t->begin;
        void** n = (void**)realloc(t->begin, t->cap + add);
        if (n) t->cap += add;
        else if ((n = (void**)realloc(t->begin, t->cap + 0x10))) t->cap += 0x10;
        else return;
        t->begin = n; t->end = n + off;
    }
    *t->end++ = fn;
}

// 1.13c D2sound+0x196F  __freefls(ptd): free per-thread buffers (+0x24,2c,34,3c,44,48,54 unless static), drop locale refs
struct d2s_locinfo { int refcnt; };
void d2s_freefls(uint8_t* ptd, const void* static54, d2s_locinfo* static_mb, d2s_locinfo* def_mb,
                 void (*lock)(int), void (*unlock)(int), void (*freetlocinfo)(void*)) {
    if (!ptd) return;
    static const int off[] = {0x24, 0x2c, 0x34, 0x3c, 0x44, 0x48};
    for (int o : off) { void* p = *(void**)(ptd + o); if (p) free(p); }
    void* p54 = *(void**)(ptd + 0x54);
    if (p54 != static54) free(p54);
    lock(0xd);
    int* li = *(int**)(ptd + 0x60);
    if (li && --*li == 0 && li != (int*)static_mb) free(li);
    unlock(0xd);
    lock(0xc);
    int* l = *(int**)(ptd + 100);
    if (l) {
        --l[0];
        for (int k : {0xb, 0xd, 0xc, 0x10}) if (l[k]) --*(int*)(intptr_t)l[k];
        --*(int*)((intptr_t)l[0x13] + 0xb4);
        if (l != (int*)def_mb && l[0] == 0) freetlocinfo(l);
    }
    unlock(0xc);
    free(ptd);
}

// 1.13c D2sound+0x1AE5  __mtinit: init locks, allocate the FLS/TLS slot, create the main thread's ptd; 1 ok, 0 fail
struct d2s_fls { uint32_t (*alloc)(void (*)(void*)); int (*set)(uint32_t, void*); };
static uint32_t g_flsindex;
int d2s_mtinit(int (*mtinitlocks)(), void (*mtterm)(), const d2s_fls& f, void (*freefls)(void*), const void* static54, uint32_t tid) {
    if (!mtinitlocks()) { mtterm(); return 0; }
    g_flsindex = f.alloc(freefls);
    if (g_flsindex != 0xffffffffu) {
        uint32_t* p = (uint32_t*)calloc(1, 0x8c);
        if (p && f.set(g_flsindex, p)) {
            p[0x15] = (uint32_t)(uintptr_t)static54; p[5] = 1; p[1] = 0xffffffffu; p[0] = tid;
            return 1;
        }
    }
    mtterm();
    return 0;
}

// 1.13c D2sound+0x1D1C  __ioinit: ioinfo arrays of 32 x 0x24 bytes; inherit handles from lpReserved2; stdin/out/err
struct d2s_ioinfo { intptr_t h; uint8_t flag; uint8_t pad[3]; int lockinit; uint8_t cs[24]; }; // 0x24 bytes
struct d2s_io_hooks {
    uint32_t (*filetype)(intptr_t); intptr_t (*stdhandle)(uint32_t); int (*initcs)(void*);
    const uint8_t* lpReserved2; uint16_t cbReserved2;
};
static d2s_ioinfo* g_pio[64]; static int g_nhandle;
static void io_fill(d2s_ioinfo* a) { for (int i = 0; i < 32; i++) { a[i].h = -1; a[i].lockinit = 0; a[i].flag = 0; a[i].pad[0] = 10; } }
int d2s_ioinit(const d2s_io_hooks& h) {
    d2s_ioinfo* a = (d2s_ioinfo*)malloc(0x480);
    if (!a) return -1;
    g_nhandle = 0x20; g_pio[0] = a; io_fill(a);
    if (h.cbReserved2 && h.lpReserved2) {
        uint32_t n; memcpy(&n, h.lpReserved2, 4);
        const uint8_t* fl = h.lpReserved2 + 4; const uint8_t* hp = fl + n;
        if ((int)n > 0x7ff) n = 0x800;
        int cnt = n;
        for (int i = 1; g_nhandle < (int)n; i++) {
            d2s_ioinfo* b = (d2s_ioinfo*)malloc(0x480);
            cnt = g_nhandle; if (!b) break;
            g_nhandle += 0x20; g_pio[i] = b; io_fill(b); cnt = n;
        }
        for (int i = 0; i < cnt; i++, hp += 4, fl++) {
            intptr_t hv; int32_t t; memcpy(&t, hp, 4); hv = t;
            if (hv != -1 && (*fl & 1) && ((*fl & 8) || h.filetype(hv) != 0)) {
                d2s_ioinfo* e = &g_pio[i >> 5][i & 0x1f];
                e->h = hv; e->flag = *fl;
                if (!h.initcs(e->cs)) return -1;
                e->lockinit++;
            }
        }
    }
    for (int i = 0; i < 3; i++) {
        d2s_ioinfo* e = &g_pio[0][i];
        if (e->h == -1) {
            e->flag = 0x81;
            intptr_t hf = h.stdhandle(i == 0 ? 0xfffffff6u : 0xfffffff5u - (i != 1));
            uint32_t ft;
            if (hf == -1 || (ft = h.filetype(hf)) == 0) e->flag |= 0x40;
            else {
                e->h = hf;
                if ((ft & 0xff) == 2) e->flag |= 0x40; else if ((ft & 0xff) == 3) e->flag |= 8;
                if (!h.initcs(e->cs)) return -1;
                e->lockinit++;
            }
        } else e->flag |= 0x80;
    }
    return 0;
}

// 1.13c D2sound+0x1F1A  __ioterm: delete each initialised entry's lock, free the arrays
void d2s_ioterm(void (*delcs)(void*)) {
    for (auto& a : g_pio) {
        if (!a) continue;
        for (int i = 0; i < 32; i++) if (a[i].lockinit) delcs(a[i].cs);
        free(a); a = nullptr;
    }
}

// 1.13c D2sound+0x1F66  __setenvp: split the double-NUL environment block into a NULL-terminated char*[] (skips "=C:=..." entries)
int d2s_setenvp(char*& envblock, char**& environ_) {
    if (!envblock) return -1;
    int n = 0;
    for (char* p = envblock; *p; p += strlen(p) + 1) if (*p != '=') n++;
    char** v = (char**)malloc(n * sizeof(char*) + sizeof(char*));
    environ_ = v;
    if (!v) return -1;
    for (char* p = envblock; *p; p += strlen(p) + 1) {
        if (*p == '=') continue;
        size_t l = strlen(p);
        char* c = (char*)malloc(l + 1);
        if (!c) { free(environ_); environ_ = nullptr; return -1; }
        memcpy(c, p, l + 1); *v++ = c;
    }
    free(envblock); envblock = nullptr; *v = nullptr;
    return 0;
}

// 1.13c D2sound+0x202D  parse_cmdline(cmd, argv, args, &numargs, &numchars): MSVC quoting rules; argv/args may be null (count pass)
// isleadbyte: the DBCS table (_mbctype & 4); retail ANSI-codepage seam
void d2s_parse_cmdline(const uint8_t* p, char** argv, char* args, int* numargs, int* numchars, bool (*lead)(uint8_t)) {
    bool inq = false; *numchars = 0; *numargs = 1;
    if (argv) *argv++ = args;
    uint8_t c;
    do { // program name: quotes toggle and are dropped, no backslash rules
        if (*p == '"') { inq = !inq; c = '"'; p++; }
        else {
            ++*numchars; if (args) *args++ = *p;
            c = *p++;
            if (lead(c)) { ++*numchars; if (args) *args++ = *p; p++; }
            if (c == 0) { p--; goto rest; }
        }
    } while (inq || (c != ' ' && c != '\t'));
    if (args) args[-1] = 0;
rest:
    inq = false;
    while (*p) {
        while (*p == ' ' || *p == '\t') p++;
        if (!*p) break;
        if (argv) *argv++ = args;
        ++*numargs;
        for (;;) {
            bool copy = true; unsigned bs = 0;
            while (*p == '\\') { p++; bs++; }
            if (*p == '"') {
                if ((bs & 1) == 0) {
                    if (inq && p[1] == '"') p++; else copy = false;
                    inq = !inq;
                }
                bs >>= 1;
            }
            while (bs--) { if (args) *args++ = '\\'; ++*numchars; }
            c = *p;
            if (!c || (!inq && (c == ' ' || c == '\t'))) break;
            if (copy) {
                if (args) { if (lead(c)) { *args++ = c; p++; ++*numchars; } *args++ = *p; }
                else if (lead(c)) { p++; ++*numchars; }
                ++*numchars;
            }
            p++;
        }
        if (args) *args++ = 0;
        ++*numchars;
    }
    if (argv) *argv = nullptr;
    ++*numargs;
}

// 1.13c D2sound+0x2199  __setargv: parse GetCommandLineA() into one malloc (argv[] then chars); modname becomes _pgmptr (unused here); 0 ok, -1 oom
int d2s_setargv(const char* cmdline, const char* modname [[maybe_unused]], int& argc, char**& argv, bool (*lead)(uint8_t)) {
    int na, nc;
    d2s_parse_cmdline((const uint8_t*)cmdline, nullptr, nullptr, &na, &nc, lead);
    char** v = (char**)malloc(nc + na * sizeof(char*));
    if (!v) return -1;
    d2s_parse_cmdline((const uint8_t*)cmdline, v, (char*)(v + na), &na, &nc, lead);
    argc = na - 1; argv = v;
    return 0;
}
