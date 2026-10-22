// src/core/d2cmp/d2cmp_9070.cpp -- 1.13c D2CMP.dll 0x9070, transcribed from the cached Ghidra decompile.
// Host seams: Fog #10102 (open file), GetLastError, Fog #10029 (log "Error opening file: %s").
#include <cstdint>

struct D2CMP9070Env {
    int      (*openFile)(void);          // Fog #10102; nonzero = opened
    uint32_t (*getLastError)(void);      // kernel32 GetLastError
    void     (*logError)(const char*);   // Fog #10029("Error opening file: %s")
};

// 1.13c D2CMP+0x09070 (57 bytes): open; on failure log unless quiet && error==2 (not found).
// 1.13c D2CMP+0x09070
extern "C"
uint32_t d2cmp_9070(const D2CMP9070Env* env, int quiet) {
    if (env->openFile()) return 1;
    uint32_t err = env->getLastError();
    if (quiet == 0 || err != 2) env->logError("Error opening file: %s");
    return 0;
}
