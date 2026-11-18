// D2Client+0xADB90: unit-attribute packet handler (pkt[1]=type, pkt[2..5]=id, pkt[6]=len, bit payload at +7); env-seam style.
// Ordinals unverified: Ordinal_10126 = bit-stream init, 10130 = read n bits, 10129 = read n bits sign-extended.
#include <cstdint>
#include <emscripten.h>
struct D2cAdb90Env {
    void* (*findUnit)(uint32_t id, void* tbl, uint32_t type);   // FUN 0xA4E20 (table = D2Client+0x1BA608 + type*0x80)
    void (*bsInit)(void* bs, const uint8_t* data, int32_t len); // Ordinal_10126
    int32_t (*bsRead)(void* bs, int32_t bits);                  // Ordinal_10130
    int32_t (*bsReadSigned)(void* bs, int32_t bits);            // Ordinal_10129
    void (*prep)(int32_t stat);                                 // 0x23760
    int32_t (*bsReadAt)(void* bs, int32_t bits);                // Ordinal_10130 on the second stream (iStack_38)
    int32_t (*add)(int32_t acc, int32_t val);                   // 0x22610 (acc, value) -> acc
    void (*finish)(int32_t acc);                                // 0x22530
    const uint8_t* rec;      // sgptDataTables+0xBCC table, stride 0x144
    int32_t recCount;        // sgptDataTables+0xBD4
    uint8_t bitMask4;        // gdwBitMasks[4] low byte
    void* tbl;               // &DAT_6fbba608
    void* bs;                // stream object
};
extern "C" {
// 1.13c D2Client+0xADB90
EMSCRIPTEN_KEEPALIVE void d2c_adb90(const D2cAdb90Env* e, const uint8_t* p) {
    if (!e->findUnit(*(const uint32_t*)(p + 2), (uint8_t*)e->tbl + p[1] * 0x80, p[1])) return;
    e->bsInit(e->bs, p + 7, (int32_t)p[6] - 7);
    int32_t id = e->bsRead(e->bs, 8);
    while (id < 0xff) {
        e->prep(id);
        int32_t acc = 0;
        if (e->bsRead(e->bs, 1)) {
            int32_t s = e->bsRead(e->bs, 9);
            while (s < 0x1ff) {
                if (s < 0 || s >= e->recCount) return;
                const uint8_t* r = e->rec + (uint32_t)s * 0x144;
                if (!r[8]) return;
                if (r[9]) e->bsReadSigned(e->bs, (int8_t)r[9]);
                int8_t v = (r[8] < 0x20 && (e->bitMask4 & r[4])) ? (int8_t)e->bsReadSigned(e->bs, r[8])
                                                                   : (int8_t)e->bsRead(e->bs, r[8]);
                acc = e->add(acc, v);
                s = e->bsReadAt(e->bs, 9);
            }
        }
        e->finish(acc);
        id = e->bsRead(e->bs, 8);
    }
}
}
