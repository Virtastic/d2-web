// 1.13c BNClient+0x0C3A0 -- id 0x15 logon-start frame: gate call (0x8126), time32(NULL) (0x1621), 16-byte payload
// {"68XI", 'VD2D' (+0x13FA when the gate is nonzero), DAT_3f648, time}. The gate, clock and frame sender (0xEB00) are injected.
#include <cstdint>
struct BnC3a0Env {
    uint32_t (*gate)();
    uint32_t (*time32)();
    void (*send)(uint32_t len, uint8_t id, const uint8_t* payload);
};
extern "C" uint32_t bnclient_c3a0(const BnC3a0Env* e, uint32_t dat3f648) {
    uint32_t pl[4];
    pl[0] = 0x49583836u;
    pl[1] = 0x44324456u + (e->gate() ? 0x13FAu : 0u);
    pl[2] = dat3f648;
    pl[3] = e->time32();
    e->send(0x10, 0x15, (const uint8_t*)pl);
    return 1;
}
