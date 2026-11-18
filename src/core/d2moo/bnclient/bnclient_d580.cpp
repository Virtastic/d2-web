// 1.13c BNClient+0x0D580 CopyRecordClear. Under the DLL critical section (0x138E8):
// Storm #491 copies n bytes from src to f670 (the record), f508 := 0, return 1. Storm #491 is the copy seam here.
#include <cstdint>

namespace d2::bnclient::d580 {
struct CopyEnv {
    void (*enter)(void* lock);
    void (*leave)(void* lock);
    void (*copy)(void* dst, const void* src, uint32_t n);   // Storm #491 (dst, src, size)
};

uint32_t copy_record_clear(const CopyEnv& env, void* lock, void* record, uint32_t* progress,
                           const void* src, uint32_t n) {
    env.enter(lock);
    env.copy(record, src, n);
    *progress = 0;
    env.leave(lock);
    return 1;
}
}
