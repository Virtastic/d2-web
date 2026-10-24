#include <cstdint>
#include <emscripten.h>

extern "C" void d2_mcp_host_lock(uint32_t idx);
extern "C" void d2_mcp_host_unlock(uint32_t idx);

// 1.13c D2MCPClient+0x015A3: __lock(n) = EnterCriticalSection(_locktable[n].cs),
// lazily creating the section via __mtinitlocknum and __amsg_exit(_RT_LOCK) if that fails. The wasm host seam
// (mcp_tiny_rowex.cpp) preallocates all 14 recursive mutexes, so the lazy init cannot fail and the whole body is
// the acquire.
// 1.13c D2MCPClient+0x015A3
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_lock_15a3(int32_t locknum) { d2_mcp_host_lock((uint32_t)locknum); }

// SEH registration chain. Retail keeps it in fs:0; wasm has no TEB, so the chain head is this process global
// and a frame is the saved-previous record (retail's EXCEPTION_REGISTRATION_RECORD.Next, kept at [ebp-0x10]).
struct D2McpSehFrame { D2McpSehFrame* prev; };
static D2McpSehFrame* g_mcp_seh_head;

// 1.13c D2MCPClient+0x01750: save fs:0 in the frame and install it.
// 1.13c D2MCPClient+0x01750
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_seh_prolog_1750(D2McpSehFrame* f) {
    f->prev = g_mcp_seh_head;
    g_mcp_seh_head = f;
}

// 1.13c D2MCPClient+0x0178B: fs:0 = frame's saved previous record.
// 1.13c D2MCPClient+0x0178B
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_seh_epilog_178b(D2McpSehFrame* f) { g_mcp_seh_head = f->prev; }

// _locinfo of retail's per-thread data (the struct __freefls decrements); fields named for the dwords it indexes.
struct D2McpLocInfoN28 {
    int refcount;           // [0]
    int* ref_b;             // [0xb]
    int* ref_c;             // [0xc]
    int* ref_d;             // [0xd]
    int* ref_10;            // [0x10]
    struct { int pad[0x2d]; int refcount_b4; }* lc;  // [0x13]: +0xb4 counter
};

// _tiddata fields __freefls releases (retail offsets in comments).
struct D2McpTiddataN28 {
    void* buf24; void* buf2c; void* buf34; void* buf3c; void* buf44; void* buf48;  // each freed when non-NULL
    void* buf54;                  // +0x54, freed unless it is the static default (env.default_54)
    int* mbcinfo;                 // +0x60, shared multibyte info refcount (locked 0xD)
    D2McpLocInfoN28* locinfo;     // +0x64 (locked 0xC)
};

struct D2McpFreeflsEnv {
    void (*free)(void*);                    // 0x1BB4 _free
    void (*lock)(int32_t);                  // 0x15A3
    void (*unlock)(int32_t);                // 0x1A81 (0xD) / 0x1A8D (0xC) -> 0x14EE
    void (*freetlocinfo)(D2McpLocInfoN28*); // 0x...___freetlocinfo
    const void* default_54;                 // &DAT_6FA2A190
    const int* default_mbcinfo;             // DAT_6FA3226C
    const D2McpLocInfoN28* default_locinfo_a;  // PTR_DAT_6FA2A484
    const D2McpLocInfoN28* default_locinfo_b;  // &DAT_6FA2A430
};

// 1.13c D2MCPClient+0x0194F: per-fiber _tiddata teardown.
// 1.13c D2MCPClient+0x0194F
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_freefls_194f(D2McpTiddataN28* p, const D2McpFreeflsEnv* e) {
    if (!p) return;
    void* b[] = {p->buf24, p->buf2c, p->buf34, p->buf3c, p->buf44, p->buf48};
    for (void* x : b) if (x) e->free(x);
    if (p->buf54 != e->default_54) e->free(p->buf54);
    e->lock(0xd);
    int* m = p->mbcinfo;
    if (m && (--*m == 0) && m != e->default_mbcinfo) e->free(m);
    e->unlock(0xd);
    e->lock(0xc);
    D2McpLocInfoN28* l = p->locinfo;
    if (l) {
        --l->refcount;
        if (l->ref_b) --*l->ref_b;
        if (l->ref_d) --*l->ref_d;
        if (l->ref_c) --*l->ref_c;
        if (l->ref_10) --*l->ref_10;
        --l->lc->refcount_b4;
        if (l != e->default_locinfo_a && l != e->default_locinfo_b && l->refcount == 0) e->freetlocinfo(l);
    }
    e->unlock(0xc);
    e->free(p);
}

struct D2McpFreeEnv {
    int heap_type;                              // DAT_6FA324A4: 3 = small-block heap
    void (*lock)(int32_t);
    void (*unlock)(int32_t);
    uint32_t* (*find_block)(void*);             // 0x4A... ___sbh_find_block
    void (*free_block)(uint32_t*, void*);       // 0x356F
    void (*heap_free)(void*);                   // HeapFree(DAT_6FA324A0, 0, p)
};

// 1.13c D2MCPClient+0x01BB4: free(NULL) is a no-op; under the small-block heap
// try that first (locked 4) and only fall to HeapFree when the block is not an SBH one.
// 1.13c D2MCPClient+0x01BB4
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_free_1bb4(void* p, const D2McpFreeEnv* e) {
    if (!p) return;
    if (e->heap_type == 3) {
        e->lock(4);
        uint32_t* hdr = e->find_block(p);
        if (hdr) e->free_block(hdr, p);
        e->unlock(4);
        if (hdr) return;
    }
    e->heap_free(p);
}
