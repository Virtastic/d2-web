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
extern "C" int32_t d2_fn_d2game_391b(int32_t c, const D2GameTolowerEnv* env) {
    void* ptd = env->getptd();
    const void* locinfo;
    std::memcpy(&locinfo, static_cast<const uint8_t*>(ptd) + 0x64, sizeof(locinfo));
    if (locinfo != env->currentTlocinfo) locinfo = env->updateTlocinfo();
    char output[3] = {};
    return env->tolowerMt(output, locinfo, c);
}
