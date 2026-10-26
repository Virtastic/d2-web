// Fog.dll CRT rows 0x36C6 0x3DEB 0x48F5 0x7760 0x7906 0x7940 0x7F18 0x814B 0x81ED 0x8644 0x8680 0x89E4,
// from build-wasm/ghidra-fog/decomp/0x<RVA>.c (alpha lane). Image base 0x6FF50000.
// Retail globals and unrecovered callees are seam params; guest links are uint32_t.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint32_t R32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void W32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }

struct FogB36Env {
    void*    (*malloc)(uint32_t);                  // _malloc (0x1FAD family)
    void     (*free)(void*);                       // _free
    void     (*free_lconv_mon)(uint32_t);          // ___free_lconv_mon
    void     (*free_lconv_num)(uint32_t);          // ___free_lconv_num
    void     (*free_lc_time)(uint32_t);            // ___free_lc_time
    uint32_t (*heap_create)(uint32_t, uint32_t, uint32_t);
    void     (*heap_destroy)(uint32_t);
    int32_t  (*heap_select)(void);                 // ___heap_select
    int32_t  (*sbh_heap_init)(uint32_t);           // 0x49C5
    void     (*initmbctable)(void);                // 0x9705
    void     (*strcpy)(void*, const void*);        // 0x8E70
    uint32_t (*strlen)(const void*);               // 0x6690
    int32_t  (*parse_cmdline)(uint32_t, uint32_t*);// 0x7FDF (argv, &count) -> bytes in ECX
    uint32_t (*get_module_filename)(uint32_t, uint32_t, uint32_t); // GetModuleFileNameA
    void*    (*get_env_strings_w)(void);           // GetEnvironmentStringsW
    void*    (*get_env_strings_a)(void);           // GetEnvironmentStringsA
    uint32_t (*get_last_error)(void);
    int32_t  (*wide_to_multi)(uint32_t, uint32_t, const void*, int32_t, void*, int32_t); // WideCharToMultiByte
    void     (*free_env_w)(void*);                 // FreeEnvironmentStringsW
    void     (*free_env_a)(void*);                 // FreeEnvironmentStringsA
    void     (*memcpy_)(void*, const void*, uint32_t); // 0x5510
    void     (*write_msg)(int32_t);                // 0x84CC
    int32_t  (*get_string_type_w)(uint32_t, const void*, int32_t, void*);
    int32_t  (*get_string_type_a)(uint32_t, uint32_t, const void*, int32_t, void*);
    int32_t  (*multi_to_wide)(uint32_t, uint32_t, const void*, int32_t, void*, int32_t);
    uint32_t (*get_cp_for_lcid)(uint32_t);         // 0x86E0
    void*    (*cp_convert)(uint32_t, uint32_t, const void*, int32_t*); // 0x8727 (from, to, src, &cch) -> malloc'd buffer
    void*    (*calloc)(uint32_t, uint32_t);        // 0x7E41
};

// Retail globals (RVA in comments); owned by the caller, passed as pointers.
struct FogB36Globals {
    uint32_t* lc_c_time;      // DAT_6ff80520
    uint32_t* lc_c_mon;       // DAT_6ff80524
    uint32_t* lc_c_num_dec;   // DAT_6ffa7124
    uint32_t* lc_c_num_thou;  // DAT_6ffa7128
    uint32_t* lc_c_ctype;     // DAT_6ffa7120
    uint32_t* heap;           // DAT_6ffa8280
    uint32_t* heap_kind;      // DAT_6ffa8284
    uint32_t* mbc_inited;     // DAT_6ffa829c
    uint32_t* env_block;      // DAT_6ff80250
    uint32_t* environ_;       // DAT_6ff80224
    uint32_t* env_flag;       // DAT_6ffa8290
    uint32_t* progname_buf;   // &DAT_6ff803e8 (guest addr)
    uint32_t* pgmptr;         // DAT_6ff80234
    uint32_t* argc;           // DAT_6ff80218
    uint32_t* argv;           // DAT_6ff8021c
    uint32_t* env_api;        // DAT_6ff804f0
    uint32_t* mt_flag_a;      // DAT_6ff80258
    uint32_t* mt_flag_b;      // DAT_6ff8025c
    uint32_t* banner_hook;    // DAT_6ff804f4
    uint32_t* stringtype_api; // DAT_6ff80518
    uint32_t* cur_lcid;       // DAT_6ff80500
    uint32_t* cur_cp;         // DAT_6ff80510
    uint32_t* argv_flag;      // DAT_6ff804ec
};

// 1.13c Fog+0x36C6 ___freetlocinfo(p): release the sub-tables of a locale-info block, then the block.
extern "C" EMSCRIPTEN_KEEPALIVE void fog_freetlocinfo_36c6(uint32_t p, const FogB36Globals* g, const FogB36Env* e) {
    uint32_t mon = R32(p + 0x3c);
    if (mon != *g->lc_c_mon && mon != 0 && R32(R32(p + 0x2c)) == 0) {
        uint32_t a = R32(p + 0x34);
        if (a && R32(a) == 0 && a != *g->lc_c_num_dec) { e->free((void*)(uintptr_t)a); e->free_lconv_mon(R32(p + 0x3c)); }
        a = R32(p + 0x30);
        if (a && R32(a) == 0 && a != *g->lc_c_num_thou) { e->free((void*)(uintptr_t)a); e->free_lconv_num(R32(p + 0x3c)); }
        e->free((void*)(uintptr_t)R32(p + 0x2c));
        e->free((void*)(uintptr_t)R32(p + 0x3c));
    }
    uint32_t c = R32(p + 0x40);
    if (c != *g->lc_c_ctype && c != 0 && R32(c) == 0) {
        e->free((void*)(uintptr_t)c);
        e->free((void*)(uintptr_t)(R32(p + 0x44) - 0xfe));
    }
    uint32_t t = R32(p + 0x50);
    if (t != *g->lc_c_time && t != 0 && R32(t + 0xb4) == 0) {
        e->free_lc_time(t);
        e->free((void*)(uintptr_t)R32(p + 0x50));
    }
    e->free((void*)(uintptr_t)p);
}

// 1.13c Fog+0x3DEB __SEH_epilog: compiler-injected x86 ExceptionList restore; no wasm counterpart.
extern "C" EMSCRIPTEN_KEEPALIVE void fog_seh_epilog_3deb_b36(void) {}

// 1.13c Fog+0x48F5 __heap_init(mt): HeapCreate(mt==0, 0x1000, 0); kind 3 -> sbh init(0x3f8) else destroy on failure.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t fog_heap_init_48f5_b36(int32_t mt, const FogB36Globals* g, const FogB36Env* e) {
    *g->heap = e->heap_create(mt == 0, 0x1000, 0);
    if (!*g->heap) return 0;
    *g->heap_kind = (uint32_t)e->heap_select();
    if (*g->heap_kind == 3 && e->sbh_heap_init(0x3f8) == 0) { e->heap_destroy(*g->heap); return 0; }
    return 1;
}

// 1.13c Fog+0x7760 _strcmp: byte compare; result -1/0/1 (retail's word-at-a-time walk is observably identical).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t fog_strcmp_7760(const uint8_t* a, const uint8_t* b) {
    for (;; a++, b++) {
        if (*a != *b) return *a < *b ? -1 : 1;
        if (*a == 0) return 0;
    }
}

// 1.13c Fog+0x7906 __freebuf(FILE*): FILE = {ptr+0,cnt+4,base+8,flag+0xc}. Free an owned (flag 8) buffer.
extern "C" EMSCRIPTEN_KEEPALIVE void fog_freebuf_7906(uint32_t f, const FogB36Env* e) {
    uint32_t flag = R32(f + 0xc);
    if ((flag & 0x83) != 0 && (flag & 8) != 0) {
        e->free((void*)(uintptr_t)R32(f + 8));
        W32(f + 0xc, (flag & 0xffff0000u) | ((flag & 0xfbf7u)));
        W32(f + 0, 0); W32(f + 8, 0); W32(f + 4, 0);
    }
}

// 1.13c Fog+0x7940 ___ascii_stricmp: ASCII case-fold compare; -1/0/1.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t fog_ascii_stricmp_7940(const uint8_t* a, const uint8_t* b) {
    for (;;) {
        uint8_t x = *a++, y = *b++;
        if (x != y) {
            uint8_t fx = (uint8_t)(x - 'A') < 26 ? x + 32 : x, fy = (uint8_t)(y - 'A') < 26 ? y + 32 : y;
            if (fx != fy) return fx < fy ? -1 : 1;
        }
        if (y == 0) return 0;
    }
}

// 1.13c Fog+0x7F18 __setenvp: build environ[] from the env block; 0 ok, -1 on alloc failure.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t fog_setenvp_7f18(const FogB36Globals* g, const FogB36Env* e) {
    if (*g->mbc_inited == 0) e->initmbctable();
    uint32_t blk = *g->env_block;
    if (!blk) return -1;
    int32_t n = 0;
    for (uint32_t p = blk; *(const char*)(uintptr_t)p; p += e->strlen((void*)(uintptr_t)p) + 1)
        if (*(const char*)(uintptr_t)p != '=') n++;
    uint32_t tab = (uint32_t)(uintptr_t)e->malloc(n * 4 + 4);
    *g->environ_ = tab;
    if (!tab) return -1;
    uint32_t out = tab;
    for (uint32_t p = blk;;) {
        if (*(const char*)(uintptr_t)p == 0) {
            e->free((void*)(uintptr_t)*g->env_block);
            *g->env_block = 0;
            W32(out, 0);
            *g->env_flag = 1;
            return 0;
        }
        uint32_t len = e->strlen((void*)(uintptr_t)p);
        if (*(const char*)(uintptr_t)p != '=') {
            uint32_t s = (uint32_t)(uintptr_t)e->malloc(len + 1);
            W32(out, s);
            if (!s) { e->free((void*)(uintptr_t)*g->environ_); *g->environ_ = 0; return -1; }
            e->strcpy((void*)(uintptr_t)s, (void*)(uintptr_t)p);
            out += 4;
        }
        p += len + 1;
    }
}

// 1.13c Fog+0x814B __setargv: module path -> parse_cmdline twice (count pass, fill pass); 0 ok, -1 on alloc failure.
// parse_cmdline(argv_or_0, &count) returns the string-bytes size (retail passes it in ECX).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t fog_setargv_814b(const FogB36Globals* g, const FogB36Env* e) {
    if (*g->mbc_inited == 0) e->initmbctable();
    *g->argv_flag = 0;
    e->get_module_filename(0, *g->progname_buf, 0x104);
    *g->pgmptr = *g->progname_buf;
    uint32_t cnt = 0;
    int32_t bytes = e->parse_cmdline(0, &cnt);
    uint32_t argv = (uint32_t)(uintptr_t)e->malloc((uint32_t)bytes + cnt * 4);
    if (!argv) return -1;
    e->parse_cmdline(argv, &cnt);
    *g->argc = cnt - 1;
    *g->argv = argv;
    return 0;
}

// 1.13c Fog+0x81ED ___crtGetEnvironmentStringsA: wide API (state 1) converted to a malloc'd ANSI block; state 2
// copies the ANSI block; returns guest addr or 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t fog_crtgetenvstrings_81ed(const FogB36Globals* g, const FogB36Env* e) {
    uint16_t* w = nullptr;
    if (*g->env_api == 0) {
        w = (uint16_t*)e->get_env_strings_w();
        if (w) *g->env_api = 1;
        else if (e->get_last_error() == 0x78) *g->env_api = 2;
    }
    if (*g->env_api != 1) {
        if (*g->env_api != 2 && *g->env_api != 0) return 0;
        const char* s = (const char*)e->get_env_strings_a();
        if (!s) return 0;
        const char* p = s;
        while (*p) p += strlen(p) + 1;
        uint32_t sz = (uint32_t)(p + 1 - s);
        void* d = e->malloc(sz);
        if (d) e->memcpy_(d, s, sz);
        e->free_env_a((void*)s);
        return (uint32_t)(uintptr_t)d;
    }
    if (!w && !(w = (uint16_t*)e->get_env_strings_w())) return 0;
    const uint16_t* p = w;
    while (*p) { while (*p) p++; p++; }
    int32_t wn = (int32_t)(p - w) + 1;
    int32_t sz = e->wide_to_multi(0, 0, w, wn, nullptr, 0);
    void* out = nullptr;
    if (sz != 0) {
        void* b = e->malloc((uint32_t)sz);
        if (b) {
            out = b;
            if (e->wide_to_multi(0, 0, w, wn, b, sz) == 0) { e->free(b); out = nullptr; }
        }
    }
    e->free_env_w(w);
    return (uint32_t)(uintptr_t)out;
}

// 1.13c Fog+0x8644 __FF_MSGBANNER: when the runtime-error banner applies, emit msg 0xfc, call the hook, emit 0xff.
extern "C" EMSCRIPTEN_KEEPALIVE void fog_ff_msgbanner_8644(const FogB36Globals* g, const FogB36Env* e, void (*hook)(void)) {
    if (*g->mt_flag_a == 1 || (*g->mt_flag_a == 0 && *g->mt_flag_b == 1)) {
        e->write_msg(0xfc);
        if (*g->banner_hook) hook();
        e->write_msg(0xff);
    }
}

// 1.13c Fog+0x8680 _memset: byte fill, returns dst.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t fog_memset_8680(uint32_t dst, int32_t val, uint32_t n) {
    memset((void*)(uintptr_t)dst, val & 0xff, n);
    return dst;
}

// 1.13c Fog+0x89E4 ___crtGetStringTypeA(info, src, cch, out, code_page, lcid, berror): wide API (state 1) goes
// MultiByteToWideChar -> GetStringTypeW; else the ANSI API with a code-page conversion when needed.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t fog_crtgetstringtypea_89e4(uint32_t info, uint32_t src, int32_t cch, uint32_t out,
        uint32_t code_page, uint32_t lcid, int32_t berror, const FogB36Globals* g, const FogB36Env* e) {
    uint16_t probe[2];
    if (*g->stringtype_api == 0) {
        if (e->get_string_type_w(1, L"", 1, probe) == 0) { if (e->get_last_error() == 0x78) *g->stringtype_api = 2; }
        else *g->stringtype_api = 1;
    }
    if (*g->stringtype_api == 2 || *g->stringtype_api == 0) {
        uint32_t loc = lcid ? lcid : *g->cur_lcid;
        uint32_t cp = code_page ? code_page : *g->cur_cp;
        uint32_t lcp = e->get_cp_for_lcid(loc);
        if (lcp == 0xffffffffu) return 0;
        void* conv = nullptr;
        if (lcp != cp) {
            conv = e->cp_convert(cp, lcp, (const void*)(uintptr_t)src, &cch);
            if (!conv) return 0;
            src = (uint32_t)(uintptr_t)conv;
        }
        int32_t r = e->get_string_type_a(loc, info, (const void*)(uintptr_t)src, cch, (void*)(uintptr_t)out);
        if (conv) e->free(conv);
        return r;
    }
    if (*g->stringtype_api != 1) return 0;
    if (!code_page) code_page = *g->cur_cp;
    int32_t n = e->multi_to_wide(code_page, berror ? 9 : 1, (const void*)(uintptr_t)src, cch, nullptr, 0);
    if (n == 0) return 0;
    uint16_t stackbuf[512];  // ponytail: retail's _alloca fallback to calloc past ~1 KB mirrored by this 1 KB buffer
    uint16_t* w = stackbuf;
    if (n > 512) { w = (uint16_t*)e->calloc((uint32_t)n, 2); if (!w) return 0; }
    else memset(stackbuf, 0, sizeof stackbuf);
    int32_t r = 0;
    if (e->multi_to_wide(code_page, 1, (const void*)(uintptr_t)src, cch, w, n) != 0)
        r = e->get_string_type_w(info, w, n, (void*)(uintptr_t)out);
    if (w != stackbuf) e->free(w);
    return r;
}
