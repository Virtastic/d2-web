// 1.13c D2Client+0xA5B90 -- CUnit.cpp: may unit pair (ESI=a, EDI=b) be matched? Env-seam style; retail
// structs are 32-bit, read as byte buffers. Ordinals 10828/10507/10601 are UNVERIFIED (see spec).
#include <cstdint>
#include <cstring>

namespace {
uint32_t rd(const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }
}

struct D2ClientA5B90Env {
    uint32_t (*ord_10828)();                       // Ordinal_10828, no stack args
    uint32_t (*ord_10507)();                       // Ordinal_10507, no stack args
    void (*ord_10601)(uint32_t h, uint32_t line);  // assert/trace, file CUnit.cpp
    const uint8_t* (*rec_15b0)();                  // FUN_6fab15b0 (D2Client+0x15B0), record or null
    uint32_t (*remap_216a0)(uint32_t key);         // D2Client+0x216A0, 0xFFFFFFFF = absent
    bool (*match_9d6c0)(uint32_t x, uint32_t y);   // D2Client+0x9D6C0
    uint8_t mask8;                                 // (byte)gdwBitMasks[8]
};

// a/b: the units' 32-bit images (type at +0, id at +0xC).
extern "C" bool d2_client_a5b90(const D2ClientA5B90Env* e, const uint8_t* a, const uint8_t* b) {
    if (!a || !b) return false;
    if (rd(a, 0) != 0 && rd(b, 0) != 0 && rd(b, 0) != 1) return false;
    bool ok = false;
    if (uint32_t h = e->ord_10828()) {
        e->ord_10601(h, 0xb7f);
        const uint8_t* r = e->rec_15b0();
        if (r && (e->mask8 & r[7])) ok = true;
    }
    bool second = false;
    if (uint32_t h = e->ord_10507()) {
        e->ord_10601(h, 0xb88);
        const uint8_t* r = e->rec_15b0();
        if (r && (e->mask8 & r[7])) second = true;
    }
    if (!second && !ok) return false;
    uint32_t id = rd(b, 0xC);
    if (rd(b, 0) == 1) {
        id = e->remap_216a0(id);
        if (id == 0xFFFFFFFFu) return false;
    }
    return e->match_9d6c0(rd(a, 0xC), id);
}
