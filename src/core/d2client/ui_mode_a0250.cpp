// D2Client rva 0xA0250: UI mode-change handler. ESI = msg pointer
// (byte [+5] = mode, dword [+1] = value). Globals are a flat state struct, callees are injected.
#include <cstdint>

struct D2ClientA0250State {
    uint32_t c214, c224, c21c, c210, c228;  // 0x1BB214, 0x1BB224, 0x1BB21C, 0x1BB210, 0x1BB228 (tick)
    uint32_t cbc48, cbc34;                  // 0x1CBC48, 0x1CBC34
    uint32_t slotCount;                     // 0x10D5D0 (must be 0x26)
    uint32_t c3a68;                         // 0x1C3A68
    uint32_t slotUsed[0x26];                // 0xFAAD80 + 4*i (u8 index in retail)
    uint32_t slotKind[0x26];                // *(slotPtr[i]+0x38), PTR table at 0x10D538
    uint32_t c9721;                         // 0x1C9721 byte
    const uint32_t* bbfc;                   // 0x1CBBFC pointer (may be null)
    uint32_t aadb8;                         // 0xFAADB8
    uint32_t bc48, a9e14;                   // 0x10BC48, 0xFA9E14 (a9e14 set by Ordinal_10006)
    uint32_t c418, c414;                    // 0x1CC418, 0x1CC414
    uint32_t v46c8, v46c4;                  // 0xFA46C8, 0xFA46C4
    uint32_t c644, c2fc, c3dc, c3e0, c3f8, c3fc, c2f8, c2f4, c63c, c640, c648;
    uint32_t tbl8ecbc[8];                   // 0x10ECBC + 0xC*i
};

struct D2ClientA0250Callees {
    void (*fatal)();                        // Ordinal_10265/10024/_exit(-1)
    void (*c53d20)();
    void (*ord10466)();
    void (*c38a70)(uint32_t, uint32_t, uint32_t, uint32_t);
    void (*c4f9e0)();
    void (*c8150)();                        // 0x?F8150
    void (*c6e350)(uint32_t, uint32_t);
    uint32_t (*tickCount)();                // GetTickCount
    uint32_t (*ord10048)();
    void (*ca80)(uint32_t);                 // 0x?EC A80
    void (*ord10227)();
    void (*c72790)(uint32_t i, uint32_t a, uint32_t b);
    void (*c7b20)();
    uint64_t (*c55440)();
    void (*c6e2e0)();
    void (*c92f0)();
    void (*ord10006)();                     // sets a9e14
    void (*c3a4d0)(int32_t a, int32_t z, int32_t b);
    void (*c43e0)(const uint8_t* msg5);     // msg is an uninitialised stack buffer in retail
};

// 1.13c D2Client+0xA0250
extern "C" void d2_client_a0250(D2ClientA0250State& s, const D2ClientA0250Callees& k, const uint8_t* msg) {
    switch (msg[5]) {
    case 0: break;
    default: k.fatal(); return;
    case 4:
        s.c214 = 2;
        if (s.c224 != 0) { k.c53d20(); s.c224 = 0; }
        return;
    case 5: k.ord10466(); k.c38a70(0, 0, 0, 0); /* fallthrough */
    case 1: k.c4f9e0(); return;
    case 6: case 7: k.c4f9e0(); k.c8150(); return;
    }
    s.c21c = (uint32_t)msg[1] | (uint32_t)msg[2] << 8 | (uint32_t)msg[3] << 16 | (uint32_t)msg[4] << 24;
    s.c210 = 0;
    k.c6e350(0, 0);
    s.c214 = 1;
    s.c228 = k.tickCount();
    if (k.ord10048() == 0) { k.fatal(); return; }
    uint32_t old = s.aadb8;
    for (uint32_t i = 0; i < 8; i++) { k.ca80(s.tbl8ecbc[i * 3]); old = s.aadb8; }
    s.cbc48 = 0; s.cbc34 = 0x13;
    k.ord10227();
    if (s.slotCount != 0x26) { k.fatal(); return; }
    bool early = s.c3a68 != 0;
    for (uint32_t i = 0; !early && (int32_t)i < (int32_t)s.slotCount; i++) {
        if (s.slotUsed[i] != 1) continue;
        switch (s.slotKind[i]) {
        case 0: break;
        case 1: k.c72790(i, 1, 0); break;
        case 4: if (s.c9721 != 0) k.c7b20(); break;
        default: early = true;
        }
    }
    if (early) {
        if (s.c210 == 0) k.c43e0(nullptr); else k.c8150();
        k.c4f9e0();
        return;
    }
    if (s.bbfc == nullptr || (*s.bbfc == 0 && (uint32_t)k.c55440() == 0)) s.aadb8 = 1;
    int32_t a, b;
    if (s.aadb8 == 0) {
        if (old != 0) { k.c6e2e0(); k.c92f0(); }
        s.c418 = 0; s.c414 = 0; k.ord10006(); b = 0; a = (int32_t)s.a9e14;
    } else {
        s.c418 = (uint32_t)-((int32_t)s.bc48 / 4); s.c414 = 1; k.ord10006();
        const int32_t q = (int32_t)s.a9e14 / 4; b = -q; a = (int32_t)s.a9e14 - q;
    }
    k.c3a4d0(a, 0, b);
    s.c644 = 0xde; s.v46c8 = s.v46c4 = 0x10; s.c2fc = 1;
    s.c3dc = s.c3e0 = s.c3f8 = s.c3fc = s.c2f8 = s.c2f4 = 0;
    s.c63c = 0x6b; s.c640 = 0x5b; s.c648 = 0x116;
}
