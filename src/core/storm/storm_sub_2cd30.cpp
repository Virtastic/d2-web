// 1.13c Storm+0x2CD30
// Bignum limb-vector divide by a 64-bit divisor: dst[off+i] = quotient limb, *rem = low 32 of remainder.
// Vector layout (u32 words): [0]=capacity [1]=size [2]=data ptr [3]=grow step [4]=limb offset.
#include <cstdint>
#include <cstring>

struct StormVec { uint32_t cap, size; uint32_t* data; uint32_t grow, off; };

struct StormSub2CD30Env {
    uint32_t (*growStep)(StormVec*);          // Storm+0x3C40
    void (*reserve)(StormVec*, uint32_t n);   // Storm+0x3C80
    void (*trim)(StormVec*);                  // Storm+0x36D0
};

void storm_sub_2cd30(const StormSub2CD30Env* env, StormVec* dst, uint32_t* rem,
                     const StormVec* src, uint64_t div) {
    uint32_t n = src->size - src->off;
    uint32_t want = dst->off + n;
    if (dst->size < want && dst->cap < want) {
        uint32_t g = dst->grow ? dst->grow : env->growStep(dst);
        env->reserve(dst, want % g ? want + g - want % g : want);
    }
    dst->size = want;
    uint64_t r = 0;
    for (uint32_t i = n; i != 0;) {
        --i;
        uint32_t idx = src->off + i;
        uint64_t limb = idx < src->size ? src->data[idx] : 0;
        uint64_t acc = (r << 32) + limb;
        uint32_t di = dst->off + i;
        if (dst->size <= di) {
            uint32_t sz = di + 1;
            if (dst->cap < sz) env->reserve(dst, sz);
            memset(dst->data + dst->size, 0, (sz - dst->size) * 4);
            dst->size = sz;
        }
        dst->data[di] = (uint32_t)(acc / div);
        r = acc % div;
    }
    env->trim(dst);
    *rem = (uint32_t)r;
}
