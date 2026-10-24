// 1.13c Storm+0x1B960 SMemFree / Storm+0x1C580 SMemAlloc -- linkable allocator entry points.
// Prior art used: the decompile (build-wasm/ghidra-storm/decomp/0x1B960.c, 0x1C580.c); env-seam forms in
// storm_batch_34110.cpp (Storm_Sub1B960Env) and storm_batch_19040.cpp (Storm_Sub1C580Env).
// ponytail: the SCode heap segments (0x1AD80/0x1BDD0/0x1C390/0x1B170 lists, header magic 0x6F6D) are not
// modelled; the pool is the host heap. Contract kept: zeroed block for the flag-8 callers (all do), null
// free is a no-op returning 0 (retail sets 0x85100081), free of a live block returns 1.
#include <cstdint>
#include <cstdlib>

// 1.13c Storm+0x1C580
extern "C" void* Storm_Sub1C580(uint32_t size, const char*, int32_t, uint32_t) { return calloc(1, size ? size : 1); }

// 1.13c Storm+0x1B960
extern "C" int32_t Storm_Sub1B960(void* p, const char*, int32_t) {
    if (!p) return 0;
    free(p);
    return 1;
}
