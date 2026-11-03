// 1.13c Storm+0x1B960 SMemFree / Storm+0x1C580 SMemAlloc -- linkable allocator entry points.
// Prior art used: the decompile (build-wasm/ghidra-storm/decomp/0x1B960.c, 0x1C580.c); env-seam forms in
// storm_batch_34110.cpp (Storm_Sub1B960Env) and storm_batch_19040.cpp (Storm_Sub1C580Env).
// ponytail: the SCode heap segments (0x1AD80/0x1BDD0/0x1C390/0x1B170 lists, header magic 0x6F6D) are not
// modelled; the pool is the host heap. Contract kept: zeroed block for the flag-8 callers (all do), null
// free is a no-op returning 0 (retail sets 0x85100081), free of a live block returns 1.
#include <cstdint>
#include <cstdlib>

// FUNCTION-mode probes set these to log the call instead (src/core/fn/storm_hk.h); null in the game.
extern "C" int32_t (*g_storm_free_hook)(void*, const char*, int32_t) = nullptr;
extern "C" void* (*g_storm_alloc_hook)(uint32_t, const char*, int32_t, uint32_t) = nullptr;

extern "C" void* (*g_storm_realloc_hook)(void*, uint32_t, const char*, int32_t, uint32_t) = nullptr;

// 1.13c Storm+0x1C760 SMemReAlloc (stdcall 5: block, size, tag, line, flags). Host-heap model: flag 0x10
// (in-place only in retail) also resizes on the host heap; the FN probes script the failing case through the hook.
extern "C" void* Storm_SMemReAlloc(void* p, uint32_t size, const char* tag, int32_t line, uint32_t flags) {
    if (g_storm_realloc_hook) return g_storm_realloc_hook(p, size, tag, line, flags);
    return realloc(p, size ? size : 1);
}

// 1.13c Storm+0x1C580
extern "C" void* Storm_Sub1C580(uint32_t size, const char* tag, int32_t line, uint32_t flags) {
    if (g_storm_alloc_hook) return g_storm_alloc_hook(size, tag, line, flags);
    return calloc(1, size ? size : 1);
}

// 1.13c Storm+0x1B960
extern "C" int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line) {
    if (g_storm_free_hook) return g_storm_free_hook(p, tag, line);
    if (!p) return 0;
    free(p);
    return 1;
}
