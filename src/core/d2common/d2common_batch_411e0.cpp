// D2Common implementation batch: 1.13c RVA 0x411E0 and 0x84190, transcribed from cached decompiles.
// ESI is an implicit register argument in both; it is an explicit parameter here. Callees and
// globals are seams (retail-layout 32-bit tokens resolved by the caller).
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace {
inline uint32_t R32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, static_cast<const uint8_t*>(p) + o, 4); return v; }
inline int32_t RI32(const void* p, uint32_t o) { return int32_t(R32(p, o)); }
}

// 1.13c D2Common+0x411e0
// pCtx+0x14 -> tables; flags[(rowIdx[esi]+x)] & 0x200 and code in (0x392,0x39b) -> 0x42A70(code+0x10).
struct D2Common411E0Env {
    void* (*resolve)(uint32_t address);                                   // retail token -> wasm address
};
extern "C" void D2Common_42A70(void*, int32_t, int32_t, int32_t, int32_t, int32_t);   // d2common_42a70.cpp
extern "C" uint32_t d2common_sub_411e0(const D2Common411E0Env* e, void* pCtx, int32_t x, int32_t esi) {
    void* t = e->resolve(R32(pCtx, 0x14));
    const uint32_t* flags = static_cast<const uint32_t*>(e->resolve(R32(t, 0x2c)));
    const int32_t* flagIdx = static_cast<const int32_t*>(e->resolve(R32(t, 0x30)));
    if (!(flags[flagIdx[esi] + x] & 0x200)) return 0;
    const int32_t* codes = static_cast<const int32_t*>(e->resolve(R32(t, 4)));
    const int32_t* codeIdx = static_cast<const int32_t*>(e->resolve(R32(t, 8)));
    const int32_t code = codes[codeIdx[esi] + x];
    if (code > 0x392 && code < 0x39b) {
        D2Common_42A70(pCtx, x, esi, code + 0x10, -1, 0);
        return 1;
    }
    return 0;
}

// 1.13c D2Common+0x84190
// esi (unit-like) +0x18 = key; table (0x15c-byte rows, first dword = key) located via index array.
// Loops over consecutive rows with matching key. Missing key/table/index: fatal (assert path, exit -1).
struct D2Common84190Env {
    uint8_t* table;      // DAT_6fdf143c
    const int32_t* index; // DAT_6fdf1444
    void* (*resolve)(uint32_t address);
    void (*sub83CA0)(void* owner, void* row);  // 0x83CA0
    void (*sub84110)(void* esi);               // 0x84110
    void (*fatal)(int32_t code);               // Ordinal_10265/10024 then _exit(-1); must not return
};
extern "C" void d2common_sub_84190(const D2Common84190Env* e, void* esi) {
    const int32_t key = RI32(esi, 0x18);
    if (key < 0) { e->fatal(0x402); std::abort(); }
    if (!e->table) { e->fatal(0x403); std::abort(); }
    if (!e->index) { e->fatal(0x404); std::abort(); }
    uint8_t* row = e->table + size_t(uint32_t(e->index[key])) * 0x15c;
    void* owner = e->resolve(R32(e->resolve(R32(e->resolve(R32(esi, 0)), 0x1b4)), 0x464));
    if (RI32(row, 0) != key) return;
    do {
        e->sub83CA0(owner, row);
        e->sub84110(esi);
        row += 0x15c;
    } while (RI32(row, 0) == RI32(esi, 0x18));
}
