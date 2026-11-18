// 1.13c D2Client+0x16620. Shutdown-time client resource cleanup.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

struct D2Client16620CleanupRecord {
    uint32_t kind;       // record +0; arrives in ESI
    uint32_t argument0;  // record +4; arrives in ECX
    uint32_t argument1;  // record +8; arrives in EDX
};

struct D2Client16620State {
    D2Client16620CleanupRecord cleanup[3]; // D2Client RVA 0xf8530, 3 records, stride 12
    uint32_t cleanupFlag;                  // D2Client RVA 0x11C4F0
    uint32_t celFiles[7];                  // D2Client RVA 0x11B838, 7 DWORD handles
};

struct D2Client16620Imports {
    uint32_t (*windowHandle)();
    void (*cleanupRecord)(uint32_t window, uint32_t argument0, uint32_t argument1,
                         uint32_t kind);
    void (*closeCelFile)(uint32_t handle);
    void (*freeClientMemory)(uint32_t handle, uint32_t sourceRva, uint32_t line,
                             uint32_t reserved);
    void (*fatalWindowError)(uint32_t messageId, int32_t exitCode);
};

static_assert(offsetof(D2Client16620State, cleanup) == 0);
static_assert(offsetof(D2Client16620State, cleanupFlag) == 36);
static_assert(offsetof(D2Client16620State, celFiles) == 40);

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_16620(
    D2Client16620State* state, const D2Client16620Imports* imports) {
    const uint32_t window = imports->windowHandle();
    if (window == 0) {
        // Retail displays message 0x18C through Fog and terminates with _exit(-1).
        imports->fatalWindowError(0x18C, -1);
        return;
    }

    for (const auto& record : state->cleanup) {
        imports->cleanupRecord(window, record.argument0, record.argument1, record.kind);
    }

    state->cleanupFlag = 0;
    for (uint32_t& handle : state->celFiles) {
        const uint32_t value = handle;
        if (value != 0) {
            imports->closeCelFile(value); // D2CMP ordinal 10065
            if (value != 0) {
                imports->freeClientMemory(value, 0xD6AB0, 0x7A, 0); // Fog ordinal 10043
            }
        }
        handle = 0;
    }
}
