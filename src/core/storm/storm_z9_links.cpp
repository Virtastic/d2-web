// Link definitions for the storm_z9 seams of storm_batch_z9.cpp that no other object defines (Storm_Free, Storm_Alloc, the
// z9 copies of the Storm globals). Every definition is weak: a real port or a probe file that defines the same name replaces
// it at link time. Without these, any object that pulls storm_batch_z9.o into a link (the 12f40 snet probe, the 36CD0 probe's
// callers) fails with "undefined symbol: storm_z9::..." and the probe is dropped as BUILD-FAIL.
#include <cstdint>

void Storm_Sub4D60(uint8_t* node);   // C++ linkage, storm_batch_b47.cpp (list unlink)
extern "C" {
void* Storm_Sub1C580(uint32_t size, const char* tag, int32_t line, uint32_t flags);   // storm_alloc_1b960.cpp
int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line);                       // storm_alloc_1b960.cpp (SMemFree)
}

namespace storm_z9 {
__attribute__((weak)) void Storm_Free(void* p, const char* file, int32_t line) { (void)Storm_Sub1B960(p, file, line); }
__attribute__((weak)) void* Storm_Alloc(uint32_t n, const char* file, int32_t line, uint32_t flags) { return Storm_Sub1C580(n, file, line, flags); }
__attribute__((weak)) void Storm_Sub4D60(void* p) { ::Storm_Sub4D60((uint8_t*)p); }
__attribute__((weak)) uint32_t* Storm_Sub1560(void*, int32_t, int32_t) { return nullptr; }   // not yet ported (list head walk)
__attribute__((weak)) uint8_t* g_list_458BC;
__attribute__((weak)) int32_t g_lastError, g_shX, g_shY, g_e0, g_e4, g_ec, g_f4;
extern __attribute__((weak)) const char g_autrans[] = "AUTRANS";   // the 0x6FC415D0 tag storm_batch_36d40.cpp passes as "AUTRANS"
}  // namespace storm_z9
