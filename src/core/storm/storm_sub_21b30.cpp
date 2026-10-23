// 1.13c Storm+0x21B30 -- picks (bits, *param_5) from the mode in param_6 and forwards to 0x21780.
// mode 0 or >=4 -> 5/7; 1,2 -> 4/6; 3 -> 6/8. Unmapped callee 0x21780 is a seam.
#include <cstdint>

struct StormSub21B30Env {
    uint8_t* (*f_21780)(int16_t* a, uint8_t* cur, int16_t* b, uint32_t two, uint8_t bits);
};

// 1.13c Storm+0x21B30
void Storm_Sub21B30(int16_t* p1, uint8_t** p2, int16_t* p3, uint32_t* p5, uint32_t mode,
                    const StormSub21B30Env& env) {
    uint8_t bits = 5;
    *p5 = 7;
    if (mode == 1 || mode == 2) { bits = 4; *p5 = 6; }
    else if (mode == 3) { bits = 6; *p5 = 8; }
    *p2 = env.f_21780(p1, *p2, p3, 2, bits);
}
