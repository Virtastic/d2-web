#include <cstdint>
#include <initializer_list>
#include <emscripten.h>

struct BnFreeFlsEnv {
    uint32_t default_thread_data;  // DAT_6FF3F95C
    uint32_t default_locale;       // PTR_DAT_6FF3D21C
    uint32_t static_locale;        // DAT_6FF3D1C8
    void (*free)(void*);
    void (*lock)(int32_t);
    void (*unlock)(int32_t);
    void (*free_locale)(uint32_t);
};

// 1.13c BNClient+0x032C6 (__freefls@4): release the six owned thread-locale strings and
// references in the 32-bit CRT locale graph, then release the thread-data block. Retail pointer
// fields remain uint32_t in wasm64; callbacks represent the DLL's _free, lock and locale seams.
extern "C" EMSCRIPTEN_KEEPALIVE void bn_freefls_32c6(uint32_t thread_data,
                                                     const BnFreeFlsEnv* env) {
    if (thread_data == 0) return;
    auto field = [thread_data](uint32_t offset) -> uint32_t* {
        return reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(thread_data + offset));
    };
    for (uint32_t offset : {0x24u, 0x2Cu, 0x34u, 0x3Cu, 0x44u, 0x48u}) {
        const uint32_t p = *field(offset);
        if (p != 0) env->free(reinterpret_cast<void*>(static_cast<uintptr_t>(p)));
    }
    const uint32_t init_addr = *field(0x54);
    if (init_addr != 0x1D4D8u) // DAT_6FF3D4D8, stored as an RVA in the port
        env->free(reinterpret_cast<void*>(static_cast<uintptr_t>(init_addr)));

    env->lock(0xD);
    uint32_t locale = *field(0x60);
    if (locale != 0) {
        auto* refs = reinterpret_cast<int32_t*>(static_cast<uintptr_t>(locale));
        if (--*refs == 0 && locale != env->default_thread_data)
            env->free(reinterpret_cast<void*>(static_cast<uintptr_t>(locale)));
    }
    env->unlock(0xD);

    env->lock(0xC);
    locale = *field(0x64);
    if (locale != 0) {
        auto* refs = reinterpret_cast<int32_t*>(static_cast<uintptr_t>(locale));
        --*refs;
        const uint32_t refs_b = *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(locale + 0x2C));
        const uint32_t refs_d = *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(locale + 0x34));
        const uint32_t refs_c = *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(locale + 0x30));
        const uint32_t refs_10 = *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(locale + 0x40));
        const uint32_t refs_13 = *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(locale + 0x4C));
        if (refs_b) --*reinterpret_cast<int32_t*>(static_cast<uintptr_t>(refs_b));
        if (refs_d) --*reinterpret_cast<int32_t*>(static_cast<uintptr_t>(refs_d));
        if (refs_c) --*reinterpret_cast<int32_t*>(static_cast<uintptr_t>(refs_c));
        if (refs_10) --*reinterpret_cast<int32_t*>(static_cast<uintptr_t>(refs_10));
        if (refs_13)
            --*reinterpret_cast<int32_t*>(static_cast<uintptr_t>(refs_13 + 0xB4));
        if (locale != env->default_locale && locale != env->static_locale && *refs == 0)
            env->free_locale(locale);
    }
    env->unlock(0xC);
    env->free(reinterpret_cast<void*>(static_cast<uintptr_t>(thread_data)));
}
