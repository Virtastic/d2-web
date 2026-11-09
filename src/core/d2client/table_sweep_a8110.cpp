// 1.13c D2Client+0xA8110 (no args): fixed sequence of table walks 0xA4DA0 (stack table, EBX = per-unit callback RVA) and A8030 calls (stack table),
// then two more walks when DAT_6fbcc394 (D2Client+0x11C394) is 0. Callback RVAs: 0x2B1E0, 0xA4D90, 0xA4D80, 0xA4E80.
#include <cstdint>
#include <emscripten.h>
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a8110(uint32_t flag_11c394, void (*walk)(uint32_t table, uint32_t cb_rva), void (*fn8030)(uint32_t table)) {
    const uint32_t B = 0x6FAB0000u;
    walk(B + 0x10AC08, 0x2B1E0);
    fn8030(B + 0x10A008);
    fn8030(B + 0x109E08);
    walk(B + 0x10A608, 0x2B1E0);
    walk(B + 0x10A808, 0x2B1E0);
    walk(B + 0x10AA08, 0x2B1E0);
    walk(B + 0x10AE08, 0x2B1E0);
    fn8030(B + 0x109C08);
    walk(B + 0x109C08, 0xA4D90);
    walk(B + 0x10AE08, 0xA4D90);
    walk(B + 0x10A608, 0xA4D90);
    walk(B + 0x10A808, 0xA4D90);
    walk(B + 0x109C08, 0xA4D80);
    walk(B + 0x10AE08, 0xA4D80);
    walk(B + 0x10A608, 0xA4D80);
    if (flag_11c394 == 0) {
        walk(B + 0x10A608, 0xA4E80);
        walk(B + 0x10A808, 0xA4E80);
    }
}
