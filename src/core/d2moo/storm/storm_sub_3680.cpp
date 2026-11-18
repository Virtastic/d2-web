// 1.13c Storm+0x3680: grow a buffer object's used length by n, reserving capacity rounded to its granularity.
#include <cstdint>

struct Storm3680Env {
    uint32_t (*f_3c40)(uint32_t* obj);       // default granularity
    void (*f_3c80)(uint32_t* obj, uint32_t newCap);  // reserve (EAX = obj in retail)
};

// obj: [0] capacity, [1] used, [3] granularity (0 = default), [4] base offset.
extern "C" void Storm_Sub3680(uint32_t* obj, int32_t n, const Storm3680Env* env) {
    uint32_t end = obj[4] + (uint32_t)n;
    if (obj[1] < end && obj[0] < end) {
        uint32_t g = obj[3];
        if (g == 0) g = env->f_3c40(obj);
        uint32_t cap = end;
        if (end % g != 0) cap = (g - end % g) + end;
        env->f_3c80(obj, cap);
    }
    obj[1] = end;
}
