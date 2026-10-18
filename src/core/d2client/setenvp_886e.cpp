// 1.13c D2Client+0x0886E (__setenvp). Retail module globals and CRT calls are
// passed through this seam so callers can supply a pinned environment block.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2ClientSetenvpState {
    uint32_t mb_table_ready;    // DAT_6fbd4ad8, RVA 0x124AD8
    char* environment;          // DAT_6fbaa8f0, RVA 0x0FA8F0
    char** envp;                // DAT_6fbaa8c4, RVA 0x0FA8C4
    uint32_t envp_ready;        // _DAT_6fbd4acc, RVA 0x124ACC
    void (*init_mb_table)();    // 0x0B627
    void* (*allocate)(uint32_t retail_size, size_t host_size); // 0x07D5F
    void (*release)(void*);     // 0x04891
};

// Native pointers are 64-bit in wasm64. The seam receives both the retail
// request size and the host storage size needed for the returned pointer array.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_886e(D2ClientSetenvpState* state) {
    if (state->mb_table_ready == 0) state->init_mb_table();
    if (state->environment == nullptr) return -1;

    uint32_t count = 0;
    for (const char* p = state->environment; *p != '\0'; p += std::strlen(p) + 1) {
        if (*p != '=') ++count;
    }
    char** const envp = static_cast<char**>(state->allocate(count * 4 + 4,
                                                           (size_t)(count + 1) * sizeof(char*)));
    state->envp = envp;
    if (envp == nullptr) return -1;

    char** slot = envp;
    for (const char* p = state->environment;;) {
        if (*p == '\0') {
            state->release(state->environment);
            state->environment = nullptr;
            *slot = nullptr;
            state->envp_ready = 1;
            return 0;
        }
        const size_t length = std::strlen(p);
        if (*p != '=') {
            char* const copy = static_cast<char*>(state->allocate(static_cast<uint32_t>(length + 1),
                                                                  length + 1));
            *slot = copy;
            if (copy == nullptr) {
                state->release(state->envp);
                state->envp = nullptr;
                return -1;
            }
            std::memcpy(copy, p, length + 1); // 0x07BC0 copies through the NUL
            ++slot;
        }
        p += length + 1;
    }
}
