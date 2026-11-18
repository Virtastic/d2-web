// 1.13c Fog+0x1CDA0 ordinal 10047: realloc-style wrapper.
// ecx is masked by the gate global (Fog+0x2F350, nonzero keeps ecx, zero gives 0), then the inner allocator
// Fog+0x1BDD0(a, edx, size, file, line) runs; a null result is logged via Fog+0x10F30(2, file, line, str).
// The callees are supplied by the caller so every path is modelled.
#include <cstdint>
typedef uint32_t (*fog_alloc_fn)(uint32_t a, uint32_t b, uint32_t size, uint32_t file, uint32_t line);
typedef void (*fog_log_fn)(uint32_t ch, uint32_t file, uint32_t line);
extern "C" uint32_t fog_sub_1cda0(uint32_t ecx, uint32_t edx, uint32_t size, uint32_t file, uint32_t line,
                                  uint32_t gate, fog_alloc_fn alloc, fog_log_fn log) {
    uint32_t p = alloc(gate ? ecx : 0, edx, size, file, line);
    if (!p) log(2, file, line);
    return p;
}
