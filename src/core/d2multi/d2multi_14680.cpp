// 1.13c D2Multi+0x14680. The host calls go through the env struct the probe passes; the Storm and D2Win
// arguments are retail image constants and are not carried by the env.
#include <cstdint>

struct D2m14680Env {
    int32_t (*storm423)(void);
    void (*storm578)(void);
    void (*storm426)(void);
    void (*ord10185)(void);
    void (*ord10097)(void);
    void (*sub13de0)(void);
};

// Storm 423 reads "LVL_REST". Present: store through 578, run 10185 and 10097, then 0x13DE0. Absent: write the default through 426.
extern "C" void d2multi_14680_env(const D2m14680Env* e) {
    if (e->storm423()) {
        e->storm578();
        e->ord10185();
        e->ord10097();
        e->sub13de0();
        return;
    }
    e->storm426();
}
