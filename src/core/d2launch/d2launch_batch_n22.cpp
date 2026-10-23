// D2Launch batch 22 -- transcribed from build-wasm/ghidra-d2launch/decomp (base 0x6FA40000).
#include <cstdint>
#include <cstdlib>
#include <unistd.h>

// 1.13c D2Launch+0x12020 -- qsort comparator over pointer-to-pointer-to-string; byte compare, returns -1/0/1.
extern "C" int32_t d2launch_sub_12020(void* const* a, void* const* b) {
    const uint8_t* p = *(const uint8_t* const*)*a;
    const uint8_t* q = *(const uint8_t* const*)*b;
    for (;; ++p, ++q) {
        if (*p != *q) return *p < *q ? -1 : 1;
        if (*p == 0) return 0;
    }
}

static void** g_realm_tab;   // DAT_6fa65e74
static int32_t g_realm_cnt;  // DAT_6fa65e78

// 1.13c D2Launch+0x12140 -- store entry in the first free slot; 0 when full. Null table is fatal (_exit(-1)).
extern "C" int32_t d2launch_sub_12140(void* entry) {
    if (!g_realm_tab) _exit(-1);
    for (int32_t i = 0; i < g_realm_cnt; ++i)
        if (!g_realm_tab[i]) { g_realm_tab[i] = entry; return 1; }
    return 0;
}
