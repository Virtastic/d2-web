// src/core/d2client/d2client_batch_b61.cpp -- 1.13c D2Client rows from build-wasm/ghidra-d2client/decomp.
// Only 0x62680 is a plain stack-arg function; the other eleven b61 rows are register-arg (in_EAX/ESI/EBX),
// SEH or CRT glue and are skipped.
#include <cstdint>
#include <initializer_list>

struct D2ClientB61Env {
    uint32_t (*ord10331)(uint32_t);   // Ordinal_10331(DAT_6fbcbbfc)
    void (*ord10184)();               // Ordinal_10184 (called twice)
    void (*f0f9f0)();                 // 0x5F9F0
    void (*f10c40)();                 // 0x60C40
    void (*f12410)();                 // 0x62410
    void (*f0ffe0)();                 // 0x5FFE0
    void (*f10aa0)();                 // 0x60AA0
    void (*f0fe90)();                 // 0x5FE90
    void (*f0fd20)();                 // 0x5FD20
    void (*f0fe10)();                 // 0x5FE10
    void (*f0ff70)();                 // 0x5FF70
};

struct D2ClientB61State {
    uint32_t d_c3b8;      // +0x11C3B8 DAT_6fbcc3b8
    uint32_t d_bbfc;      // +0x11BBFC DAT_6fbcbbfc
    uint32_t d_c394;      // +0x11C394 DAT_6fbcc394
    uint32_t d_c21c;      // +0x11C21C DAT_6fbcc21c
    uint32_t d_9854;      // +0x119854 DAT_6fbc9854
};

// 1.13c D2Client+0x62680
void d2client_62680(D2ClientB61State* s, const D2ClientB61Env* e) {
    if (s->d_c3b8 == 0 || s->d_bbfc == 0) return;
    if (e->ord10331(s->d_bbfc) == 0) return;
    e->f0f9f0();
    e->f10c40();
    e->f12410();
    uint32_t mode = s->d_c394;
    s->d_c21c = 0x18;
    e->ord10184();
    e->f0ffe0();
    e->f10aa0();
    e->f0fe90();
    e->f0fd20();
    if (mode == 6 || mode == 8) e->f0fe10();
    if (s->d_9854 != 0) e->f0ff70();
    e->ord10184();
}
