// 1.13c D2Multi+0x10410 env form. fastcall: ECX = p1, EDX = flags, ESI = object,
// stack = name (ret 4). The callees arrive in the env so the FUNCTION-mode probe drives them; production is
// d2_multi_sub_10410 in d2multi_batch_22.cpp.
#include <cstdint>

struct D2m10410Env {
    uint32_t (*f340)(uint32_t p1, uint32_t flags, uint32_t* o);
    void     (*storm501)(void* dst, const char* src, int32_t max);
    int32_t  (*fog10227)(void);
    void     (*d2win10168)(int32_t a, uint32_t b, uint32_t c, void* d, void* f);
    void     (*f170)(int32_t* p);
    void     (*f750)(void);
};

extern "C" void d2multi_10410_env(const D2m10410Env* e, uint32_t p1, uint32_t flags, const char* name, uint32_t* o) {
    uint8_t* ob = (uint8_t*)o;
    o[0x24] = flags;
    uint32_t r = e->f340(p1, flags, o);
    e->storm501(ob + 4, name, 0x7fffffff);
    uint32_t k = 5;
    if ((*(uint16_t*)(ob + 0x8a) & 4) && !(*(uint16_t*)(ob + 0x8a) & 8)) {
        if (e->fog10227() != 0 || (uint8_t)r < 5) k = 1;
    }
    e->d2win10168(*(int32_t*)(ob + 0x9c), r & 0xff, k, ob + 0x66, ob + 0x76);
    e->f170((int32_t*)o);
    e->f750();
}
