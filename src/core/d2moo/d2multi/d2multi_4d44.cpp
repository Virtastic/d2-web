// 1.13c D2Multi+0x4D44 _wctomb: locinfo = [ptd+0x64], refreshed by __updatetlocinfo when it is not the current global; then ___wctomb_mt.
#include <cstdint>
struct D2MultiWctombEnv {
    uint8_t* (*getptd)(); uint8_t* (*updatetlocinfo)(); int (*wctomb_mt)(uint8_t*, char*, uint32_t); uint32_t cur_locinfo;
};
extern "C" int d2multi_wctomb_env(char* mb, uint32_t wc, const D2MultiWctombEnv* env) {
    uint8_t* li = (uint8_t*)(uintptr_t)*(uint32_t*)(env->getptd() + 0x64);
    if ((uint32_t)(uintptr_t)li != env->cur_locinfo) li = env->updatetlocinfo();
    return env->wctomb_mt(li, mb, wc);
}
