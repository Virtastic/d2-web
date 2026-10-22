// D2Client rva 0x7EFA0: per-frame panel-mode / screen-split update.
// EAX = ctx pointer (stored to [0x113AE8]); stack arg param_1 (may be null). Globals are a flat state
// struct, callees are injected.
#include <cstdint>
#include <cstring>

struct D2Client7EFA0State {
    uint32_t slotCount;      // 0x10D5D0 (must be 0x26)
    uint32_t panelFlag;      // 0xFAADC0 (F)
    uint32_t c3A68;          // 0x113A68
    uint32_t slotUsed[0x26]; // 0xFAAD80 + 4*i
    uint32_t slotKind[0x26]; // *(slotPtr[i]+0x40), PTR table at 0x10D538
    uint32_t c9721;          // 0x1C9721 byte
    const uint32_t* bbfc;    // 0x1CBBFC pointer (may be null)
    uint32_t a84, a90;       // 0xFAAD84, 0xFAAD90
    uint32_t bc48, a9e14;    // 0x10BC48, 0xFA9E14 (a9e14 is written by Ordinal_10006)
    uint32_t c418, c414;     // 0x1CC418, 0x1CC414
    uint32_t v46c8, v46c4;   // 0xFA46C8, 0xFA46C4
    uint32_t c3dc, c3e0, c3f8, c3fc, c2f8, c2f4;
    uint32_t ctx;            // 0x113AE8 (EAX in)
    const uint32_t* ctxMagic;// &ctx[0x80]  (dword read at ctx+0x80)
    uint32_t b14, aa0;       // 0x113B14, 0x113AA0
};

struct D2Client7EFA0Callees {
    void (*ord10227)();
    void (*fatal)();                                  // Ordinal_10265/10024/_exit(-1), when slotCount != 0x26
    void (*c72790)(uint32_t i, uint32_t a, uint32_t b);
    void (*c7B20)();                                  // 0x?F7B20
    uint64_t (*c55440)();                             // low = EAX, high = EDX
    uint32_t (*winHandle)();                          // Ordinal_10048
    void (*eccc0)(uint32_t h);                        // 0x3CCC0 (str 0xCD0810 fixed)
    void (*ecc60)(uint32_t h);                        // 0x3CC60 (same str)
    void (*c6e2e0)();
    void (*ord10006)();                               // sets a9e14
    void (*c3a4d0)(int32_t a, int32_t z, int32_t b);
    void (*c43e0)(const uint8_t* msg5);
};

// 1.13c D2Client+0x7EFA0
extern "C" void d2_client_7efa0(D2Client7EFA0State& s, const D2Client7EFA0Callees& k, uint32_t ctx,
                                const uint32_t* param1   /* param_1+0xc already applied by the caller: pass &field */) {
    s.ctx = ctx;
    k.ord10227();
    if (s.slotCount != 0x26) { k.fatal(); return; }
    const uint32_t old = s.panelFlag;
    uint32_t f = old;
    bool tail = false;
    if (f == 0) {
        if (s.c3A68 != 0) tail = true;
        else for (uint32_t i = 0; i < s.slotCount && !tail; i++) {
            if (s.slotUsed[i] != 1) continue;
            switch (s.slotKind[i]) {
            case 0: break;
            case 1: k.c72790(i, 1, 0); f = s.panelFlag; break;
            case 4: if (s.c9721 != 0) { k.c7B20(); f = s.panelFlag; } break;
            default: tail = true;
            }
        }
    }
    if (!tail) {
        bool set = s.bbfc == nullptr;
        if (!set && *s.bbfc == 0) {
            const uint64_t r = k.c55440();
            f = (uint32_t)(r >> 32);                  // EDX leaks into uVar5 when the call returns nonzero
            set = (uint32_t)r == 0;
        }
        if (set) { f = (f == 0); s.panelFlag = f; }
        if (old == 0) { if (f != 0) k.eccc0(k.winHandle()); }
        else if (f == 0) { k.ecc60(k.winHandle()); k.c6e2e0(); }
        int32_t a, b;
        if (s.a84 == 0 && s.a90 == 0) {
            if (f == 0) { s.c418 = 0; s.c414 = 0; k.ord10006(); b = 0; a = (int32_t)s.a9e14; }
            else {
                s.c418 = (uint32_t)((int32_t)s.bc48 / 4); s.c414 = 2; k.ord10006();
                b = (int32_t)s.a9e14 / 4; a = (int32_t)s.a9e14 + b;
            }
        } else if (f == 0) {
            s.c418 = (uint32_t)-((int32_t)s.bc48 / 4); s.c414 = 1; k.ord10006();
            b = (int32_t)s.a9e14 / 4; a = (int32_t)s.a9e14 - b; b = -b;
        } else { s.c418 = 0; s.c414 = 3; k.ord10006(); b = 0; a = (int32_t)s.a9e14; }
        k.c3a4d0(a, 0, b);
    }
    s.v46c8 = s.v46c4 = 0x10;
    s.c3dc = s.c3e0 = s.c3f8 = s.c3fc = s.c2f8 = s.c2f4 = 0;
    if (*s.ctxMagic == 0x20646b62) {
        s.b14 = 1;
        uint8_t m[5] = {'>'};
        const uint32_t v = param1 ? *param1 : 0xffffffffu;   // param_1+0xc is the caller's field; see spec
        memcpy(m + 1, &v, 4);
        k.c43e0(m);
        s.aa0 = 0;
        return;
    }
    s.b14 = 0; s.aa0 = 1;
}
