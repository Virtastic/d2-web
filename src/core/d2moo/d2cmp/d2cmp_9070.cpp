// 1.13c D2CMP+0x9070: ESI = the file object, stack word 0 = quiet (ret 4). Host seams: Fog open thunk 0x8DB4 (ECX = esi), GetLastError (IAT 0x1C068),
// Fog log thunk 0x8DBA (cdecl: format string, esi).
#include <cstdint>

struct D2CMP9070Env {
    int      (*openFile)(uint32_t esi);                   // thunk 0x8DB4; nonzero = opened
    uint32_t (*getLastError)(void);                       // kernel32 GetLastError
    void     (*logError)(const char* fmt, uint32_t esi);  // thunk 0x8DBA("Error opening file: %s", esi)
};

// 1.13c D2CMP+0x09070 (57 bytes): open; on failure log unless quiet && error==2 (not found).
extern "C"
uint32_t d2cmp_9070(const D2CMP9070Env* env, uint32_t esi, int quiet) {
    if (env->openFile(esi)) return 1;
    uint32_t err = env->getLastError();
    if (quiet == 0 || err != 2) env->logError("Error opening file: %s", esi);
    return 0;
}
