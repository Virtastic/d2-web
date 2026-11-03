#include <cstdint>
// 1.13c D2Launch+0x19700 -- logo/intro movie sequence. A set state byte 0x35b is fatal (error 0x8d1). Otherwise D2Win#10149(0), the
// screen list is cleared (0x15D30), 0x11CB0 runs, and when D2Win#10148 says movies are on the two Blizzard movies are queued with
// D2gfx#10027(name, mode, 0) (low-resolution pair with mode 2 when Fog#10191 is non-zero, else the 480 pair with mode 0); a zero 0x12940
// plays the intro (0x19680); otherwise Fog#10227 and 0x128D0 gate the 0x12770 sequence. Every path ends with D2Win#10141(1) and a
// tail jump to 0x18160.
struct D2l19700Seams {
    void (*win10149)(uint32_t);
    void (*f15d30)();
    void (*f11cb0)();
    int (*win10148)();
    int (*fog10191)();
    void (*gfx10027)(uint32_t name, uint32_t mode, uint32_t z);
    int (*f12940)();
    int (*fog10227)();
    int (*f128d0)();
    void (*f12770)();
    void (*f19680)();
    void (*win10141)(uint32_t);
    void (*f18160)();
    void (*fatal)(uint32_t);
};
// names: [0] low New_BLIZ, [1] low BlizNorth, [2] 480 New_BLIZ, [3] 480 BlizNorth (retail strings at 0x1E124, 0x1E0FC, 0x1E174, 0x1E14C)
extern "C" void d2l_19700_core(const uint8_t* state, const uint32_t names[4], const D2l19700Seams* s) {
    if (state[0x35b]) { s->fatal(0x8d1); return; }
    s->win10149(0);
    s->f15d30();
    s->f11cb0();
    if (s->win10148()) {
        const bool low = s->fog10191() != 0;
        s->gfx10027(low ? names[0] : names[2], low ? 2 : 0, 0);
        s->gfx10027(low ? names[1] : names[3], low ? 2 : 0, 0);
        if (s->f12940() == 0) { s->f19680(); s->win10141(1); s->f18160(); return; }
        if (s->fog10227() != 0 && s->f128d0() != 0) s->f12770();
    }
    s->win10141(1);
    s->f18160();
}
