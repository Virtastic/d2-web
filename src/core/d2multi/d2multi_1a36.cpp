// 1.13c D2Multi+0x1A36 _tolower: locinfo = [ptd+0x64]; if it differs from the global current one, __updatetlocinfo; then ___tolower_mt.
#include <cstdint>
struct D2MultiTolowerEnv {
    uint8_t* (*getptd)(); uint8_t* (*updatetlocinfo)(); int (*tolower_mt)(uint8_t*, int); uint32_t cur_locinfo;
};
extern "C" int d2multi_tolower_env(int c, const D2MultiTolowerEnv* env) {
    uint8_t* li = (uint8_t*)(uintptr_t)*(uint32_t*)(env->getptd() + 0x64);
    if ((uint32_t)(uintptr_t)li != env->cur_locinfo) li = env->updatetlocinfo();
    return env->tolower_mt(li, c);
}
