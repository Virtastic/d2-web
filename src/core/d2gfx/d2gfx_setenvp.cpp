// D2MOO's D2Gfx DllMain covers the game entry, not this VC7.1 CRT leaf.
// The packed environment block and allocator are supplied by the CRT host seam.
#include <cstddef>
#include <cstdint>
#include <cstring>

struct D2GfxSetenvpEnv {
    char* environment_strings;       // retail DAT_6fa90dec
    char** environment;              // retail DAT_6fa90dc4
    uint32_t environment_initialized; // retail DAT_6fa9ea28
    uint32_t mbcs_initialized;        // retail DAT_6fa9ea34
    void (*init_mbcs)();
    void* (*allocate)(size_t);
    void (*deallocate)(void*);
};

// 1.13c D2Gfx+0x2F66 -- __setenvp. The Windows environment is a packed list of
// NUL-terminated strings ending in an extra NUL. Entries beginning with '=' are
// skipped in the exported array. Allocation failure returns -1; the CRT's partial
// copy failure releases only the pointer array, leaving any copied strings allocated.
extern "C" int32_t d2gfx_setenvp(D2GfxSetenvpEnv* env) {
    if (env->mbcs_initialized == 0) env->init_mbcs();
    const char* block = env->environment_strings;
    if (block == nullptr) return -1;

    uint32_t count = 0;
    for (const char* p = block; *p != '\0'; p += std::strlen(p) + 1) {
        if (*p != '=') ++count;
    }
    char** entries = static_cast<char**>(env->allocate((static_cast<size_t>(count) + 1) * sizeof(char*)));
    env->environment = entries;
    if (entries == nullptr) return -1;

    char** slot = entries;
    for (const char* p = block; *p != '\0'; p += std::strlen(p) + 1) {
        if (*p == '=') continue;
        const size_t size = std::strlen(p) + 1;
        char* copy = static_cast<char*>(env->allocate(size));
        *slot = copy;
        if (copy == nullptr) {
            env->deallocate(entries);
            env->environment = nullptr;
            return -1;
        }
        std::memcpy(copy, p, size);
        ++slot;
    }
    env->deallocate(env->environment_strings);
    env->environment_strings = nullptr;
    *slot = nullptr;
    env->environment_initialized = 1;
    return 0;
}
