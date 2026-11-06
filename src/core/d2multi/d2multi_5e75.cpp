// 1.13c D2Multi+0x5E75 ___initmbctable with __setmbcp as a seam: once only, setmbcp(-3), flag set to 1, returns 0.
#include <cstdint>

struct D2MultiInitmbctableEnv { void (*setmbcp)(int); };

extern "C" int d2multi_initmbctable_env(uint32_t* flag, const D2MultiInitmbctableEnv* env) {
    if (*flag == 0) { env->setmbcp(-3); *flag = 1; }
    return 0;
}
