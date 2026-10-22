// Fog batch b54b. Decompile-derived from build-wasm/ghidra-fog/decomp.
#include <cstdint>

// Seam: 256-entry int32 table at Fog+0x77F50 (retail .data).
extern const int32_t fog_table_77F50[256] __attribute__((weak));

// 1.13c Fog+0xFCE0 (ordinal 10236): empty body.
void fog_sub_FCE0() {}

// 1.13c Fog+0x13C10 (ordinal 10229): count loop with no effect.
void fog_sub_13C10(void*, int32_t) {}

// 1.13c Fog+0x13BE0 (ordinal 10230): sum of table[byte] over a buffer.
int32_t fog_sub_13BE0(const uint8_t* p, int32_t n) {
    if (!fog_table_77F50) return 0;
    int32_t s = 0;
    for (const uint8_t* e = p + n; p < e; ++p) s += fog_table_77F50[*p];
    return s;
}
