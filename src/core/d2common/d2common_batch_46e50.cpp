// D2Common 1.13c batch: item-unit stat application by item-type class. Retail reads the unit
// from ESI; globals are env seams (DAT_6fdeff6c lookup table, DAT_6fdefb98 row base).
#include <cstdint>
#include <cstdlib>
#include <emscripten.h>

struct D2Batch46E50Env {
    const uint8_t* (*itemsTxt)(int32_t classId);                        // 0x719A0
    int32_t (*lookup)(const void* table, uint32_t key, int32_t zero);    // Ordinal_10213
    const void* lookupTable;                                             // DAT_6fdeff6c
    const uint8_t* rowBase;                                              // DAT_6fdefb98, 0x1a8 stride
    uint32_t (*checkType)(const int32_t* unit, int32_t type);            // 0x24430
    int32_t (*getStat)(const int32_t* unit, int32_t stat);               // 0x38C20
    void (*setStat)(const int32_t* unit, int32_t stat, int32_t value);   // 0x3A740
    uint32_t (*flag24C30)(const int32_t* unit);                          // 0x24C30 (low byte)
    void (*fatal)(uint32_t messageId);                                   // 10265/10024 report
};

extern "C" {
// 1.13c D2Common+0x46E50
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_46e50(const int32_t* u, const D2Batch46E50Env* e) {
    uint32_t msg;
    if (!u) msg = 0x61b;
    else if (u[0] == 4) {
        const uint8_t* it = e->itemsTxt(u[1]);
        if (it) {
            const uint32_t key = *reinterpret_cast<const uint32_t*>(it + 0x80);
            const int32_t idx = e->lookup(e->lookupTable, key, 0);
            if (idx >= 0) {
                const uint8_t* r = e->rowBase + static_cast<int64_t>(idx) * 0x1a8;
                if (r) switch (key) {
                case 0x10: case 0x1f:
                    if (e->checkType(u, 0x32)) {
                        const int32_t lim = *reinterpret_cast<const int32_t*>(r + 0xd0);
                        if (lim) {
                            int32_t v = e->getStat(u, 0x1f) + 1;
                            if (v <= lim) v = lim + 1;
                            e->setStat(u, 0x1f, v);
                        }
                    }
                    break;
                case 0x11: case 0x16:
                    if (e->checkType(u, 0x2d)) {
                        if (r[0xff]) e->setStat(u, 0x16, r[0xff]);
                        if (r[0x103]) e->setStat(u, 0x18, r[0x103]);
                        if ((e->flag24C30(u) & 0xff) && r[0x101]) e->setStat(u, 0xa0, r[0x101]);
                    }
                    break;
                case 0x12: case 0x15:
                    if (e->checkType(u, 0x2d)) {
                        if (r[0xfe]) e->setStat(u, 0x15, r[0xfe]);
                        if (r[0x102]) e->setStat(u, 0x17, r[0x102]);
                        if ((e->flag24C30(u) & 0xff) && r[0x100]) e->setStat(u, 0x9f, r[0x100]);
                    }
                    break;
                }
            }
            return;
        }
        msg = 0x61f;
    } else msg = 0x61c;
    if (e->fatal) e->fatal(msg);
    __builtin_trap(); // retail: Ordinal_10265/10024 report then _exit(-1)
}
}
