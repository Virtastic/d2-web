// src/core/storm/storm_batch_078e1.cpp -- 1.13c Storm.dll MSVC CRT rows 0x78E1 (__mtinit), 0x79D0 (__flsbuf),
// 0x8333 (_free), 0x84A8 (__heap_alloc), 0x8523 (__nh_malloc), from build-wasm/ghidra-storm/decomp/0x<RVA>.c
// (alpha lane: marker only, no spec, no vector). Retail globals and callees are seam members of
// StormBatch78e1Env, passed after the retail argument list.
#include <cstdint>
#include <cstring>

struct StormCrtFile {  // retail FILE layout (VS2003)
    char* _ptr; int32_t _cnt; char* _base; uint32_t _flag; int32_t _file; int32_t _charbuf; int32_t _bufsiz; char* _tmpfname;
};

struct StormBatch78e1Env {
    // ---- 0x78E1 ----
    int32_t (*mtinitlocks)();                       // 0x9198-area __mtinitlocks
    void    (*mtterm)();                            // __mtterm
    void*   (*getKernel32)();                       // GetModuleHandleA("kernel32.dll")
    void*   (*getProc)(void* mod, const char* nm);  // GetProcAddress
    void*   fls[4];                                 // DAT_6fc42484..90 (Alloc, Get, Set, Free)
    void*   fallback[4];                            // TlsGetValue, TlsSetValue, FUN_76C1, TlsFree
    uint32_t flsIndex;                              // DAT_6fc402b0
    void*   freefls;                                // __freefls@4
    void*   (*calloc_)(uint32_t n, uint32_t sz);    // FUN_6fbf9198
    uint32_t (*flsAlloc)(void* cb);                 // *DAT_6fc42484
    int32_t (*flsSet)(uint32_t idx, void* v);       // *DAT_6fc4248c
    uint32_t (*curThreadId)();
    void*   initialPtd;                             // &DAT_6fc40408
    // ---- 0x79D0 ----
    int32_t (*isatty)(int32_t fh);
    void    (*getbuf)(StormCrtFile* f);
    int32_t (*write)(int32_t fh, const void* buf, uint32_t n);
    int32_t (*lseek)(int32_t fh, int32_t off, int32_t whence);
    StormCrtFile* stdoutF; StormCrtFile* stderrF;   // DAT_6fc408f8 / DAT_6fc40918
    uint8_t** ioinfo;                                // DAT_6fc47ba0
    uint8_t*  dummyInfo;                             // DAT_6fc403e0
    // ---- 0x8333 / 0x84A8 / 0x8523 ----
    int32_t heapMode;                                // DAT_6fc47b98
    void*   heap;                                    // DAT_6fc47b94
    uint32_t sbhThreshold;                           // DAT_6fc46910
    void    (*lock)(int32_t n);
    void    (*unlockHeap)();                         // FUN_8386 / FUN_851A
    void*   (*sbhFindBlock)(void* p);
    void    (*sbhFreeBlock)(void* hdr, void* p);
    void*   (*sbhAllocBlock)(uint32_t size);
    void    (*heapFree)(void* heap, uint32_t flags, void* p);
    void*   (*heapAlloc)(void* heap, uint32_t flags, uint32_t size);
    int32_t (*callnewh)(uint32_t size);
};

// 1.13c STORM+0x78E1
extern "C" int32_t d2_storm_mtinit_78e1(StormBatch78e1Env* e) {
    if (e->mtinitlocks() == 0) { e->mtterm(); return 0; }
    void* k = e->getKernel32();
    if (k) {
        e->fls[0] = e->getProc(k, "FlsAlloc");
        e->fls[1] = e->getProc(k, "FlsGetValue");
        e->fls[2] = e->getProc(k, "FlsSetValue");
        e->fls[3] = e->getProc(k, "FlsFree");
        if (!e->fls[1]) { e->fls[1] = e->fallback[0]; e->fls[2] = e->fallback[1]; e->fls[0] = e->fallback[2]; e->fls[3] = e->fallback[3]; }
    }
    e->flsIndex = e->flsAlloc(e->freefls);
    if (e->flsIndex != 0xFFFFFFFFu) {
        uint32_t* p = (uint32_t*)e->calloc_(1, 0x8c);
        if (p && e->flsSet(e->flsIndex, p)) {
            p[0x15] = (uint32_t)(uintptr_t)e->initialPtd;
            p[5] = 1;
            p[1] = 0xFFFFFFFFu;
            p[0] = e->curThreadId();
            return 1;
        }
    }
    e->mtterm();
    return 0;
}

// 1.13c STORM+0x79D0
extern "C" int32_t d2_storm_flsbuf_79d0(int32_t ch, StormCrtFile* f, StormBatch78e1Env* e) {
    uint32_t fl = f->_flag;
    int32_t fh = f->_file;
    if ((fl & 0x82) == 0 || (fl & 0x40) != 0) { f->_flag = fl | 0x20; return -1; }
    if (fl & 1) {
        f->_cnt = 0;
        if ((fl & 0x10) == 0) { f->_flag = fl | 0x20; return -1; }
        f->_ptr = f->_base;
        f->_flag = fl & 0xfffffffeu;
    }
    fl = f->_flag;
    f->_cnt = 0;
    f->_flag = (fl & 0xffffffefu) | 2;
    int32_t written = 0, want;
    if ((fl & 0x10c) == 0 && ((f != e->stdoutF && f != e->stderrF) || e->isatty(fh) == 0)) e->getbuf(f);
    if ((f->_flag & 0x108) == 0) {
        want = 1;
        uint8_t c = (uint8_t)ch;
        written = e->write(fh, &c, 1);
    } else {
        char* base = f->_base; char* ptr = f->_ptr;
        f->_ptr = base + 1;
        want = (int32_t)(ptr - base);
        f->_cnt = f->_bufsiz - 1;
        if (want < 1) {
            uint8_t* info = fh == -1 ? e->dummyInfo : e->ioinfo[fh >> 5] + (fh & 0x1f) * 0x24;
            if (info[4] & 0x20) e->lseek(fh, 0, 2);
        } else written = e->write(fh, base, (uint32_t)want);
        *f->_base = (char)ch;
    }
    if (written == want) return ch & 0xff;
    f->_flag |= 0x20;
    return -1;
}

// 1.13c STORM+0x8333
extern "C" void d2_storm_free_8333(void* p, StormBatch78e1Env* e) {
    if (!p) return;
    if (e->heapMode == 3) {
        e->lock(4);
        void* hdr = e->sbhFindBlock(p);
        if (hdr) e->sbhFreeBlock(hdr, p);
        e->unlockHeap();
        if (hdr) return;
    }
    e->heapFree(e->heap, 0, p);
}

// 1.13c STORM+0x84A8
extern "C" void* d2_storm_heap_alloc_84a8(uint32_t size, StormBatch78e1Env* e) {
    if (e->heapMode == 3 && size <= e->sbhThreshold) {
        e->lock(4);
        void* b = e->sbhAllocBlock(size);
        e->unlockHeap();
        if (b) return b;
    }
    if (size == 0) size = 1;
    if (e->heapMode != 1) size = (size + 0xf) & 0xfffffff0u;
    return e->heapAlloc(e->heap, 0, size);
}

// 1.13c STORM+0x8523
extern "C" void* d2_storm_nh_malloc_8523(uint32_t size, int32_t nhFlag, StormBatch78e1Env* e) {
    if (size < 0xffffffe1u) {
        do {
            void* p = d2_storm_heap_alloc_84a8(size, e);
            if (p) return p;
            if (nhFlag == 0) return nullptr;
        } while (e->callnewh(size) != 0);
    }
    return nullptr;
}
