// Prior art used: D2Client's ___tolower_mt model (src/core/d2client/crt_strings.cpp, 0x7604) and
// D2Game's sibling wrappers at 0x2813 and 0x3853. D2MOO's 1.10f D2Game tree contains no copy of
// this statically linked CRT helper; the cached 1.13c decompile establishes this wrapper's calls.
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace {

struct D2GameTolowerEnv {
    void* (*getptd)();
    void* (*updateTlocinfo)();
    int32_t (*tolowerMt)(void* output, const void* locinfo, int32_t c);
    const void* currentTlocinfo;
};

} // namespace

// Retail calls __getptd, reads the thread's locale pointer at tiddata+0x64, refreshes it through
// ___updatetlocinfo unless it is the startup locale, then forwards the original int to
// ___tolower_mt. Its ECX value is the local output buffer used by the locale mapping path.
// 1.13c D2Game+0x0391B (_tolower), Stage A (OBJECTIVE v9): implementation + marker only, no spec or vector.
extern "C" int32_t d2game_tolower_impl(int32_t c, const D2GameTolowerEnv* env) {
    void* ptd = env->getptd();
    const void* locinfo;
    std::memcpy(&locinfo, static_cast<const uint8_t*>(ptd) + 0x64, sizeof(locinfo));
    if (locinfo != env->currentTlocinfo) locinfo = env->updateTlocinfo();
    char output[3] = {};
    return env->tolowerMt(output, locinfo, c);
}

// FN-probe seam for 0x391B (src/core/fn/d2game_391b.cpp): startup C locale, A-Z -> a-z.
static uint8_t g_ptd[0x80], g_locinfo[0x40];
extern "C" int32_t d2game_tolower_probe(int32_t c) {
    D2GameTolowerEnv env = {};
    env.getptd = []() -> void* { const void* p = g_locinfo; std::memcpy(g_ptd + 0x64, &p, sizeof p); return g_ptd; };
    env.updateTlocinfo = []() -> void* { return g_locinfo; };
    env.tolowerMt = [](void*, const void*, int32_t ch) -> int32_t { return (ch >= 'A' && ch <= 'Z') ? ch + 32 : ch; };
    env.currentTlocinfo = g_locinfo;
    return d2game_tolower_impl(c, &env);
}
