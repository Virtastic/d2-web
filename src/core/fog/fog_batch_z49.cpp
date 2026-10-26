// src/core/fog/fog_batch_z49.cpp -- twelve 1.13c Fog.dll alpha-lane rows (statically linked MSVC CRT:
// __cexit, memmove, free/malloc/realloc, heap alloc, isalpha/isdigit) from the cached decompiles
// (build-wasm/ghidra-fog/decomp/0x<RVA>.c). Base 0x6FF50000. Retail globals and callees are seam
// parameters; 32-bit links are uint32_t.
#include <cstdint>
#include <cstring>

// Seams for the retail CRT internals these rows call.
struct FogCrtSeams {
    void     (*lock)(int n);                          // __lock (+0x3CF4 family)
    void     (*unlock)(int n);                        // +0x3C3F
    uint32_t (*sbhFindBlock)(uint32_t p);             // ___sbh_find_block
    void     (*sbhFreeBlock)(uint32_t hdr, uint32_t p);
    uint32_t (*sbhAllocBlock)(uint32_t size);
    int      (*sbhResizeBlock)(uint32_t hdr, uint32_t p, uint32_t size);
    uint32_t (*heapAlloc)(uint32_t size);             // HeapAlloc(DAT_6ffa8280,0,size)
    uint32_t (*heapReAlloc)(uint32_t p, uint32_t size);
    void     (*heapFree)(uint32_t p);
    int      (*callNewH)(uint32_t size);              // __callnewh (+0x54E8)
    void*    (*mem)(uint32_t p);                      // 32-bit link -> host pointer
    uint32_t heapMode;      // DAT_6ffa8284 (3 = small-block heap)
    uint32_t sbhThreshold;  // DAT_6ffa8270
    int      newHandlerMode; // DAT_6ff803d4
};

// 1.13c FOG+0x01BCB  __cexit
void Fog_cexit_01BCB(void (*doexit)(int, int, int)) { doexit(0, 0, 1); }

// 1.13c FOG+0x01C70  _memmove (overlap-safe; the retail unrolled copy has memmove semantics)
void* Fog_memmove_01C70(void* dst, const void* src, uint32_t size) { return memmove(dst, src, size); }

// 1.13c FOG+0x02000, +0x02090, +0x0223F  unlock heap lock 4
void Fog_heapUnlock_02000(const FogCrtSeams& s) { s.unlock(4); }
void Fog_heapUnlock_02090(const FogCrtSeams& s) { s.unlock(4); }
void Fog_heapUnlock_0223F(const FogCrtSeams& s) { s.unlock(4); }

// 1.13c FOG+0x01FAD  _free
void Fog_free_01FAD(const FogCrtSeams& s, uint32_t p) {
    if (!p) return;
    if (s.heapMode == 3) {
        s.lock(4);
        uint32_t hdr = s.sbhFindBlock(p);
        if (hdr) s.sbhFreeBlock(hdr, p);
        Fog_heapUnlock_02000(s);
        if (hdr) return;
    }
    s.heapFree(p);
}

// 1.13c FOG+0x0201E  __heap_alloc
uint32_t Fog_heapAlloc_0201E(const FogCrtSeams& s, uint32_t size) {
    if (s.heapMode == 3 && size <= s.sbhThreshold) {
        s.lock(4);
        uint32_t b = s.sbhAllocBlock(size);
        Fog_heapUnlock_02090(s);
        if (b) return b;
    }
    if (size == 0) size = 1;
    if (s.heapMode != 1) size = (size + 0xF) & 0xFFFFFFF0u;
    return s.heapAlloc(size);
}

// 1.13c FOG+0x02099  __nh_malloc
uint32_t Fog_nhMalloc_02099(const FogCrtSeams& s, uint32_t size, int nhFlag) {
    if (size < 0xFFFFFFE1u) {
        do {
            uint32_t p = Fog_heapAlloc_0201E(s, size);
            if (p) return p;
            if (nhFlag == 0) return 0;
        } while (s.callNewH(size) != 0);
    }
    return 0;
}

// 1.13c FOG+0x020C5  _malloc
uint32_t Fog_malloc_020C5(const FogCrtSeams& s, uint32_t size) {
    return Fog_nhMalloc_02099(s, size, s.newHandlerMode);
}

// 1.13c FOG+0x020D7  _realloc
uint32_t Fog_realloc_020D7(const FogCrtSeams& s, uint32_t p, uint32_t size) {
    if (!p) return Fog_malloc_020C5(s, size);
    if (!size) { Fog_free_01FAD(s, p); return 0; }
    auto oldSize = [&]() { return *(uint32_t*)((uint8_t*)s.mem(p) - 4) - 1u; };  // block header dword
    if (s.heapMode == 3) {
        do {
            uint32_t out = 0;
            if (size > 0xFFFFFFE0u) { s.callNewH(size); return 0; }
            s.lock(4);
            uint32_t hdr = s.sbhFindBlock(p);
            if (hdr) {
                if (size <= s.sbhThreshold) {
                    if (s.sbhResizeBlock(hdr, p, size) == 0) {
                        out = s.sbhAllocBlock(size);
                        if (out) {
                            uint32_t n = oldSize();
                            if (size <= n) n = size;
                            memcpy(s.mem(out), s.mem(p), n);
                            hdr = s.sbhFindBlock(p);
                            s.sbhFreeBlock(hdr, p);
                        }
                    } else {
                        out = p;
                    }
                }
                if (!out) {
                    size = ((size ? size : 1) + 0xF) & 0xFFFFFFF0u;
                    out = s.heapAlloc(size);
                    if (out) {
                        uint32_t n = oldSize();
                        if (size <= n) n = size;
                        memcpy(s.mem(out), s.mem(p), n);
                        s.sbhFreeBlock(hdr, p);
                    }
                }
            }
            Fog_heapUnlock_0223F(s);
            if (!hdr) {
                size = ((size ? size : 1) + 0xF) & 0xFFFFFFF0u;
                out = s.heapReAlloc(p, size);
            }
            if (out) return out;
            if (!s.newHandlerMode) return 0;
        } while (s.callNewH(size) != 0);
    } else {
        do {
            if (size > 0xFFFFFFE0u) { s.callNewH(size); return 0; }
            if (!size) size = 1;
            uint32_t out = s.heapReAlloc(p, size);
            if (out) return out;
            if (!s.newHandlerMode) return 0;
        } while (s.callNewH(size) != 0);
    }
    return 0;
}

// 1.13c FOG+0x02289  _isalpha (mask 0x103); ctype table / MB-aware path are seams
int Fog_isalpha_02289(int c, int mbCurMax, const uint16_t* ctype, int (*isctypeMt)(int c, int mask)) {
    return mbCurMax > 1 ? isctypeMt(c, 0x103) : (ctype[c] & 0x103);
}

// 1.13c FOG+0x022C8  _isdigit (mask 4)
int Fog_isdigit_022C8(int c, int mbCurMax, const uint16_t* ctype, int (*isctypeMt)(int c, int mask)) {
    return mbCurMax > 1 ? isctypeMt(c, 4) : (ctype[c] & 4);
}
