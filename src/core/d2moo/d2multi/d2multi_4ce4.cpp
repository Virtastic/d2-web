// 1.13c D2Multi+0x4CE4 ___wctomb_mt: C locale (loc+0x14 == 0) stores the byte when wc < 0x100; otherwise WideCharToMultiByte(cp=loc+4,
// 0, &wc, 1, mb, loc+0x28, 0, &usedDefault); any failure sets errno 0x2A and returns -1.
#include <cstdint>
struct D2MultiWctombMtEnv {
    int (*widechar_to_multibyte)(uint32_t, uint32_t, const uint16_t*, int, char*, int, const char*, int*);
    int* (*errno_ptr)();
};
extern "C" int d2multi_wctomb_mt_env(const uint8_t* loc, char* mb, uint16_t wc, const D2MultiWctombMtEnv* env) {
    if (!mb) return 0;
    if (*(const int*)(loc + 0x14) == 0) {
        if (wc < 0x100) { *mb = (char)wc; return 1; }
    } else {
        int used = 0;
        int n = env->widechar_to_multibyte(*(const uint32_t*)(loc + 4), 0, &wc, 1, mb, *(const int*)(loc + 0x28), nullptr, &used);
        if (n != 0 && used == 0) return n;
    }
    *env->errno_ptr() = 0x2a;
    return -1;
}
