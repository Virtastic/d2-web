#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>

// Kernel32 and the CRT allocator are injected to retain their failure/release
// semantics. Windows WCHAR is always 16 bits, independent of native wchar_t.
struct D2GameEnvironment660e {
    uint32_t kind; // retail module probe word at RVA 0x10FFF8
    const uint16_t* (*get_wide)();
    const char* (*get_ansi)();
    uint32_t (*last_error)();
    int32_t (*convert_acp)(const uint16_t*, int32_t, char*, int32_t);
    void (*release_wide)(const uint16_t*);
    void (*release_ansi)(const char*);
    void* (*allocate)(uint32_t);
    void (*deallocate)(void*);
};

// Prior art: sibling D2Client+0x08B43 and D2DDraw+0x024FB; checked
// against the cached D2Game 1.13c body. No D2MOO game implementation exists.
// 1.13c D2Game+0x0660E
extern "C" EMSCRIPTEN_KEEPALIVE void* d2game_environment_strings_660e(
    D2GameEnvironment660e* env) {
    const uint16_t* wide = nullptr;
    if (env->kind == 0) {
        wide = env->get_wide();
        if (wide) env->kind = 1;
        else if (env->last_error() == 0x78) env->kind = 2;
    }
    if (env->kind != 1) {
        if (env->kind != 0 && env->kind != 2) return nullptr;
        const char* source = env->get_ansi();
        if (!source) return nullptr;
        const char* end = source;
        while (*end) { while (*end) ++end; ++end; }
        const uint32_t bytes = static_cast<uint32_t>(end - source) + 1;
        void* copy = env->allocate(bytes);
        if (copy) std::memcpy(copy, source, bytes);
        env->release_ansi(source);
        return copy;
    }
    if (!wide) wide = env->get_wide();
    if (!wide) return nullptr;
    const uint16_t* end = wide;
    while (*end) { while (*end) ++end; ++end; }
    const int32_t units = static_cast<int32_t>(end - wide) + 1;
    const uint32_t bytes = static_cast<uint32_t>(env->convert_acp(wide, units, nullptr, 0));
    char* copy = bytes ? static_cast<char*>(env->allocate(bytes)) : nullptr;
    if (copy && !env->convert_acp(wide, units, copy, static_cast<int32_t>(bytes))) {
        env->deallocate(copy);
        copy = nullptr;
    }
    env->release_wide(wide);
    return copy;
}
