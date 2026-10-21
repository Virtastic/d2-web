// D2Client+0x3A730: load the 0x474-byte key/command table from the Storm record, validate it, else fall back to defaults.
// Env-seam style; no float.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct D2c3A730Env {
    int32_t (*probe)();                            // 0x38E80
    void (*o10110)(int32_t n, void* blk, int32_t z);  // Ordinal_10110(0x476,&local_484,0); blk starts at local_484
    void (*o10109)();
    void (*cmdtbl_load)();                         // 0x39910
    void (*apply)();                               // 0x3A3A0
    void (*def_reload)();                          // 0x39A60
    uint8_t* live;                                 // DAT_6fbca2e0, 0x474 bytes
    const uint8_t* defaults;                       // DAT_6fba4a98, 0x474 bytes
};
static bool valid(const uint8_t* t) {              // 114 entries x 10 bytes: {i32 id, i16 key, ...}
    for (int id = 0; id <= 0x38; id++) {
        bool f = false;
        for (int i = 0; i < 114 && !f; i++) { int32_t v; std::memcpy(&v, t + 10 * i, 4); f = v == id; }
        if (!f) return false;
    }
    for (int i = 0; i < 114; i++) {
        int16_t a; std::memcpy(&a, t + 10 * i + 4, 2);
        if (a == -1) continue;
        for (int j = 0; j < 114; j++) { int16_t b; std::memcpy(&b, t + 10 * j + 4, 2); if (i != j && a == b) return false; }
    }
    return true;
}
static void clear_keys(uint8_t* t) { for (int i = 0; i < 114; i++) { t[10 * i + 4] = 0xff; t[10 * i + 5] = 0xff; } }
extern "C" {
// 1.13c D2Client+0x3A730
EMSCRIPTEN_KEEPALIVE void d2c_3a730(const D2c3A730Env* e) {
    if (e->probe() != -1) {
        uint8_t blk[6 + 0x474] = {0};                // int hdr, i16 version, table
        int32_t hdr;
        e->o10110(0x476, blk, 0);
        e->o10109();
        std::memcpy(&hdr, blk, 4);
        int16_t ver; std::memcpy(&ver, blk + 4, 2);
        if (hdr == 0x476 && ver == 0x25 && valid(blk + 6)) {
            clear_keys(e->live);
            std::memcpy(e->live, blk + 6, 0x474);
            e->apply();
            return;
        }
    }
    e->cmdtbl_load();
    clear_keys(e->live);
    std::memcpy(e->live, e->defaults, 0x474);
    e->apply();
    e->def_reload();
}
}
