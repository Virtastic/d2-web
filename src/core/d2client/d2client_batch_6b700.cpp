// src/core/d2client/d2client_batch_6b700.cpp -- 1.13c D2Client+0x6B700 (903 B), alpha lane, from
// build-wasm/ghidra-d2client/decomp/0x6B700.c. Base 0x6FAB0000. Globals are parameters; unrecovered
// callees and ordinals are host callbacks. HIDDEN-REGISTER: EAX = p (object, GUESS). The decompile
// shows stack-slot reuse; locals below are a best-effort reading and the mode-0 x (a4) is a GUESS.
#include <cstdint>
#include <emscripten.h>

static inline uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }

struct H6b700 {
    int (*ord10013)(void);
    void (*ord10651)(uint32_t);
    int (*ord11142)(void);
    uint32_t (*ord11168)(void);
    void (*ord11080)(void);
    void (*ord10023)(void);
    uint32_t (*ord10175)(void);
    void (*ord10350)(uint32_t, void*, void*, void*);
    void (*ord10663)(uint32_t);
    void (*ord10569)(void*);
    int (*ord10866)(void);
    uint32_t (*ord10431)(void);
    void (*ord10046)(void*, int, uint32_t, int);
    void (*sub_560d0)(uint32_t);
    void (*sub_6a860)(void);
    bool (*sub_6a3d0)(int);
    uint8_t (*sub_68cf0)(void);
    int32_t* (*sub_69910)(int);
    int32_t* (*sub_6a2f0)(uint32_t, uint32_t);
    int32_t* (*sub_69090)(void*);
    void (*sub_6dca0)(uint32_t, void*, void*, void*, void*, void*, void*, uint32_t, int);
    uint8_t* (*sub_6d6d0)(int, int);
    int (*sub_6ea50)(uint8_t*, uint8_t*, int*, void*, void*);
    uint8_t (*sub_6e670)(void);
    int (*sub_6e2c0)(uint32_t, void*);
    void (*fatal)(uint32_t);
};

// 1.13c D2Client+0x6B700
extern "C" EMSCRIPTEN_KEEPALIVE
int d2_client_6b700(uint32_t p, const H6b700* h, int32_t glob_c995c, int32_t mode_cc414, int32_t half_8bc48) {
    int mode = h->ord10013();
    if (!p || (int32_t)rd32(p) < 0 || (int32_t)rd32(p) > 2) { h->sub_6a860(); return 1; }
    uint32_t sub = rd32(p + 0x54), s30 = sub ? rd32(sub + 0x30) : 0;
    int32_t a4, a0;
    if (mode == 0) {
        h->ord10651(p);
        int32_t w = s30 ? (int32_t)rd32(s30 + 0x38) : 0;
        int32_t v = h->ord11142();
        a4 = v;  // GUESS: slot never written in this arm
        a0 = v + (w - (glob_c995c - 8));
        a0 += s30 ? (int32_t)rd32(s30 + 0x3c) : 0;
    } else {
        a4 = s30 ? (int32_t)rd32(s30 + 0x3c) : 0;
        h->ord11168(); h->ord11080(); h->ord10023();
        int32_t q = (half_8bc48 + ((half_8bc48 >> 31) & 3)) >> 2;
        if (mode_cc414 == 1) a4 -= q; else if (mode_cc414 == 2) a4 += q;
        a4 += s30 ? (int32_t)rd32(s30 + 0x34) : 0;
        a0 = 0x10 + (s30 ? (int32_t)rd32(s30 + 0x38) : 0);
    }
    if (!h->sub_6a3d0(a4)) return 0;
    uint8_t buf[0x80] = {0}; uint32_t o1 = 0, o2 = 0; int32_t o3[4] = {0};
    h->sub_68cf0();
    h->ord10175();
    h->ord10350(p, &o1, &o2, buf);
    h->ord10663(p);
    h->ord10569(buf);
    if (((rd32(p + 0xc8) >> 3) & 1) && h->ord10866()) h->sub_560d0(0);
    uint32_t id = h->ord10431();
    int32_t* r = h->sub_69910(-1);
    if (!r) r = h->sub_6a2f0(id, p);
    if (!r) return 0;
    int32_t* rec = h->sub_69090(buf);
    if (!rec) return 0;
    uint32_t cnt = 0;
    h->sub_6dca0(p, &o1, &o2, buf, &o1, &o2, o3, ((const uint8_t*)rec)[2], 1);
    uint8_t* tbl = h->sub_6d6d0((int)o2, (int)o1);
    if (!tbl) { h->fatal(0xa56); return 0; }
    int rc = h->sub_6ea50((uint8_t*)rec, tbl, (int*)&cnt, &o1, &o2);
    if (rc <= 0) return 0;
    for (cnt = 0; cnt < *(const uint8_t*)rec; cnt++) {
        uint8_t b = tbl[cnt];
        if (h->sub_6e670()) {
            uint32_t pk[0x12] = {0};
            pk[0] = o1; pk[1] = (uint32_t)b << 8; pk[0x11] = o2;
            if (h->sub_6e2c0(p, pk)) h->ord10046(pk, 0, p, 0xa2f);
        }
    }
    return 1;
}
