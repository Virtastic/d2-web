// 1.13c Storm+0x349E0 (ordinal 546): real body over the shared module state.
#include <Windows.h>
#include "storm_state.h"

extern "C" int FlushFileBuffers(HANDLE) __attribute__((weak));

StormLog3b1e0* g_stormLogs[4];       // 0x6FC42FC0
CRITICAL_SECTION g_stormLogLocks[4]; // 0x6FC42F60

extern "C" void Storm_FlushLogs_349E0(uint32_t scratch) {
    DWORD written = scratch;
    for (uint32_t b = 0; b < 4; ++b) {
        EnterCriticalSection(&g_stormLogLocks[b]);
        for (StormLog3b1e0* n = g_stormLogs[b]; n; n = n->next) {
            if (n->used != 0) {
                WriteFile(n->file, n->data, n->used, &written, nullptr);
                FlushFileBuffers(n->file);
                n->used = 0;
                n->position = 0;
            }
        }
        LeaveCriticalSection(&g_stormLogLocks[b]);
    }
}
