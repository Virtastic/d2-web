// src/core/d2common/d2common_sub_6082.cpp -- 1.13c D2Common.dll _realloc (VC7.1 CRT). Base 0x6FD50000.
// CRT pieces (small-block heap, heap, new-handler) are passed in; this is the control flow only.
#include <cstdint>

struct D2CommonReallocEnv {
    void*    (*malloc_)(uint32_t n);                       // _malloc
    void     (*free_)(void* p);                            // _free
    int      heap_mode;                                    // DAT_6fdf1710 (3 = small-block heap)
    uint32_t sbh_threshold;                                // DAT_6fdf14b4
    void*    (*sbh_find)(void* p);                         // ___sbh_find_block
    int      (*sbh_resize)(void* hdr, void* p, uint32_t n);
    void*    (*sbh_alloc)(uint32_t n);
    void     (*sbh_free)(void* hdr, void* p);
    void     (*lock4)(); void (*unlock4)();                // __lock(4) / 0x61EA
    void*    (*heap_alloc)(uint32_t n);                    // HeapAlloc(DAT_6fdf170c,0,n)
    void*    (*heap_realloc)(void* p, uint32_t n);         // HeapReAlloc(DAT_6fdf170c,0,p,n)
    void     (*memcpy_)(void* d, const void* s, uint32_t n);
    int      new_mode;                                     // DAT_6fdef968
    int      (*callnewh)(uint32_t n);                      // __callnewh
};

// 1.13c D2Common+0x06082 (417 B)
extern "C" void* D2Common_6082_Realloc(void* p, uint32_t n, const D2CommonReallocEnv* e)
{
    if (!p) return e->malloc_(n);
    if (!n) { e->free_(p); return nullptr; }
    for (;;) {
        if (n > 0xffffffe0u) { e->callnewh(n); return nullptr; }
        void* r = nullptr;
        if (e->heap_mode == 3) {
            e->lock4();
            void* hdr = e->sbh_find(p);
            if (hdr) {
                uint32_t old = ((uint32_t*)p)[-1] - 1;
                if (n <= e->sbh_threshold) {
                    if (e->sbh_resize(hdr, p, n)) r = p;
                    else if ((r = e->sbh_alloc(n))) {
                        e->memcpy_(r, p, old < n ? old : n);
                        e->sbh_free(e->sbh_find(p), p);
                    }
                }
                if (!r) {
                    n = (n ? n + 0xf : 1 + 0xf) & ~0xfu;
                    if ((r = e->heap_alloc(n))) {
                        e->memcpy_(r, p, old < n ? old : n);
                        e->sbh_free(hdr, p);
                    }
                }
            }
            e->unlock4();
            if (!hdr) { n = (n ? n + 0xf : 1 + 0xf) & ~0xfu; r = e->heap_realloc(p, n); }
        } else {
            if (!n) n = 1;
            r = e->heap_realloc(p, n);
        }
        if (r) return r;
        if (!e->new_mode || !e->callnewh(n)) return nullptr;
    }
}
