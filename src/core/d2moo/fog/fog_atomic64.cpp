#include <atomic>
#include <cstdint>
// 1.13c Fog+0x1C9A0 -- atomic 64-bit read: *out = *src (cmpxchg8b trick on x86; a plain atomic load here).
extern "C" void fog_read64_1c9a0(uint64_t* out, const uint64_t* src) {
    *out = __atomic_load_n(src, __ATOMIC_SEQ_CST);
}
// 1.13c Fog+0x1C9C0 -- atomic 64-bit store: *dst = *src (cmpxchg8b retry loop on x86).
extern "C" void fog_write64_1c9c0(uint64_t* dst, const uint64_t* src) {
    __atomic_store_n(dst, *src, __ATOMIC_SEQ_CST);
}
