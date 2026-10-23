// D2Client 0x23170 (caller 0xA7780): start-of-action gate. Seams are the register-arg callees; names UNVERIFIED.
#include <cstdint>
struct Client23170Env {
    int32_t (*f22430)();                 // ==1 gate
    uint8_t* (*f22a70)();                // current entry (p), may be null
    int32_t (*f55440)();
    int32_t (*f22040)(uint8_t* p);       // reads p+0x80
    void (*f88a70)(int32_t eax, int32_t, int32_t, int32_t);
    void (*f22010)(uint8_t* p, int32_t v); // writes p+0x80
    uint32_t (*f89640)();                // rand
    int32_t (*f223d0)();                 // ==8 -> f22ca0
    void (*f22ca0)();
    int32_t dat97f8;                     // DAT_6fbc97f8
    int32_t* dat_c4c0; int32_t* dat_c4c8; // DAT_6fbcc4c0 / c8
};
// 1.13c D2Client+0x23170
extern "C" void d2_client_23170(int32_t eax, const Client23170Env& e) {
    if (e.f22430() != 1) return;
    uint8_t* p = e.f22a70();
    if (!p) return;
    if (*(const int32_t*)(p + 0x70) != 0 && e.f55440() == 0 && e.f22040(p) == 0) {
        e.f88a70(eax, 0, 0, 0);
        *e.dat_c4c0 = e.dat97f8;
        e.f22010(p, e.dat97f8);
        *e.dat_c4c8 = (int32_t)(e.f89640() + 0x1e);
        return;
    }
    if (e.f223d0() == 8) e.f22ca0();
}
