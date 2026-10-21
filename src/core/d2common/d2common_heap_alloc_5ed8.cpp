#include <cstdint>
#include <cstddef>

// 1.13c D2Common+0x05ED8 __heap_alloc(size) (111 B, caller 0x5F53 _malloc). Mode 3 (small-block heap)
// and size <= threshold: __lock(4), ___sbh_alloc_block (+0x8500), unlock (+0x5F4A); non-null wins.
// Else 0 -> 1, round up to 16 unless mode is 1, HeapAlloc. Env seam: CRT hooks are host functions.
struct D2CommonHeapAllocEnv {
    uint32_t heapMode;      // DAT 0x6fdf1710
    uint32_t sbhThreshold;  // DAT 0x6fdf14b4
    void*    heap;          // DAT 0x6fdf170c
    void     (*lock)(int32_t);
    void*    (*sbhAllocBlock)(uint32_t);
    void     (*unlock4)();
    void*    (*heapAlloc)(void*, uint32_t, size_t);
};

extern "C" void* d2common_heap_alloc(size_t size, const D2CommonHeapAllocEnv* e) {
    if (e->heapMode == 3 && size <= e->sbhThreshold) {
        e->lock(4);
        void* b = e->sbhAllocBlock((uint32_t)size);
        e->unlock4();
        if (b) return b;
    }
    if (size == 0) size = 1;
    if (e->heapMode != 1) size = (size + 0xF) & ~(size_t)0xF;
    return e->heapAlloc(e->heap, 0, size);
}
