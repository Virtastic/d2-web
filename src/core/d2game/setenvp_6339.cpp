// VC7.1 environment initialization; globals and CRT allocation use a pinned host seam.
#include <cstddef>
#include <cstdint>
#include <cstring>

struct D2GameSetenvpState {
    uint32_t multibyte_ready; // retail RVA 0x11B358
    char* block;              // retail RVA 0x10FD6C
    char** environment;       // retail RVA 0x10FD44
    uint32_t initialized;     // retail RVA 0x11B34C
    void (*init_multibyte)();
    void* (*allocate)(uint32_t retail_bytes, size_t host_bytes);
    void (*release)(void*);
};

// 1.13c D2Game+0x06339
extern "C" int32_t d2game_setenvp(D2GameSetenvpState* state) {
    if (!state->multibyte_ready) state->init_multibyte();
    if (!state->block) return -1;
    uint32_t count = 0;
    for (const char* p = state->block; *p; p += std::strlen(p) + 1)
        if (*p != '=') ++count;
    char** entries = static_cast<char**>(state->allocate(count * 4u + 4u,
        (static_cast<size_t>(count) + 1) * sizeof(char*)));
    state->environment = entries;
    if (!entries) return -1;
    char** slot = entries;
    for (const char* p = state->block; *p; p += std::strlen(p) + 1) {
        if (*p == '=') continue;
        const size_t bytes = std::strlen(p) + 1;
        *slot = static_cast<char*>(state->allocate(static_cast<uint32_t>(bytes), bytes));
        if (!*slot) {
            state->release(state->environment);
            state->environment = nullptr;
            return -1;
        }
        std::memcpy(*slot++, p, bytes);
    }
    state->release(state->block);
    state->block = nullptr;
    *slot = nullptr;
    state->initialized = 1;
    return 0;
}
