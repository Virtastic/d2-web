// D2Client 0x23170 (caller 0xA7780): start-of-action gate. Callee seams carry the register inputs (the unit) the
// retail callees read from ESI/EAX/EBX/EDI; matches the FN probe's environment layout.
#include <cstdint>
struct Client23170Env {
    const uint8_t* (*mem)(uint32_t);               // unused by the port
    uint32_t (*f22430)(uint32_t unit);             // ==1 gate
    uint32_t (*f22a70)(uint32_t unit);             // config pointer, 0 when none
    uint32_t (*f55440)(uint32_t unit);             // 0xA5440 busy check
    uint32_t (*f22040)(uint32_t unit);             // stamp check
    void (*f88a70)(uint32_t unit, uint32_t ebx);   // play, EBX = cfg+0x70
    void (*f22010)(uint32_t unit, uint32_t v);     // writes unit stamp
    uint32_t (*f89640)(uint32_t unit);             // rand, ESI = 0x3D when retail calls it
    uint32_t (*f223d0)(uint32_t unit);             // ==8 calls f22ca0
    void (*f22ca0)(uint32_t unit, uint32_t slot);
    uint32_t dat97f8;                              // [0x1197F8]
    uint32_t* dat_c4c0; uint32_t* dat_c4c8;        // [0x11C4C0] / [0x11C4C8]
};
// 1.13c D2Client+0x23170
extern "C" void d2_client_23170(uint32_t unit, const Client23170Env* e) {
    if (e->f22430(unit) != 1) return;
    const uint32_t cfg = e->f22a70(unit);
    if (!cfg) return;
    const uint32_t ebx = *(const uint32_t*)(uintptr_t)(cfg + 0x70);
    if (ebx != 0 && e->f55440(unit) == 0 && e->f22040(unit) == 0) {
        e->f88a70(unit, ebx);
        *e->dat_c4c0 = e->dat97f8;
        e->f22010(unit, e->dat97f8);
        *e->dat_c4c8 = e->f89640(0x3d) + 0x1e;
        return;
    }
    if (e->f223d0(unit) == 8) e->f22ca0(unit, 8);
}
