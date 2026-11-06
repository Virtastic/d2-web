// D2Lang batch s-d2lang-next-30: CRT _realloc (0x4FD3); _setmbcp (0x4BB1) is in d2lang_4bb1.cpp.
#include <cstdint>
#include <cstdlib>
#include <cstring>

// 1.13c D2Lang+0x4FD3: _realloc(p, n). NULL p -> malloc; n==0 -> free, NULL. Retail's small-block-heap / HeapReAlloc /
// new-handler retry paths collapse to host realloc (single heap on wasm); size > 0xFFFFFFE0 fails.
void* d2lang_crt_realloc_4fd3(void* p, uint32_t n) {
    if (!p) return malloc(n);
    if (!n) { free(p); return nullptr; }
    if (n > 0xffffffe0u) return nullptr;
    return realloc(p, n);
}
