// 1.13c D2Client+0x7E6E0: per-frame client-state tick. Globals/callees are passed in (each names its address).
#include <cstdint>
typedef int32_t i32; typedef uint32_t u32;
struct D2c7e6e0State {            // D2Client globals (base 0x6FAB0000)
    uint8_t g3a7c, g3aca, g3a80, g3a84, g3a88, g3a60, g3aff, g3448; // +0x113A7C,+0x113ACA,+0x113A80,+0x113A84,+0x113A88,+0x113A60,+0x113AFF,+0x123448
    u32 gAad50;                   // 0x6fbaad50 resume tick
    i32 g8d5d4;                   // 0x6fb8d5d4
    i32 hasCb;                    // DAT_6fbcbf94 != 0
};
struct D2c7e6e0Hooks {
    void (*f7cef0)(void);         // FUN_6fb2cef0
    void (*f486a0)(void);         // FUN_6faf86a0
    u32 (*tick)(void);            // GetTickCount
    i32 (*cb)(u32 arg);           // *DAT_6fbcbf94(DAT_6fbc3aff)
    void (*f3cc60)(void);         // Ordinal_10048 + FUN_6faecc60(ret,&DAT_6fb91388)
    void (*f7bf40)(void);         // FUN_6fb2bf40
    void (*ord10043)(void);       // Ordinal_10043(0x945,0)
    void (*fatal)(void);          // Ordinal_10265(0x943); Ordinal_10024; _exit(-1)
};
extern "C" u32 d2c_7e6e0(D2c7e6e0State* s, const D2c7e6e0Hooks* h) {
    u32 r = 0;
    s->g3a7c = 1;
    if (s->g3aca != 0 || s->g3a80 == 1) {
        if (!s->hasCb) {
            if (s->g3aca == 0) h->f7cef0(); else h->f486a0();
            if (s->g8d5d4) s->gAad50 = h->tick() + 100;
        } else {
            h->f7cef0();
            if (s->g8d5d4) s->gAad50 = h->tick() + 100;
            if (h->cb(s->g3aff)) { s->g3aff = 0; return 1; }
            if (s->g3aca) h->f486a0();
        }
        s->g3aca = 0;
        if (s->g3448 == 1) { s->g3448 = 0; h->f3cc60(); }
        s->g3aff = 0;
        return 1;
    }
    if (s->g3a84 == 1) {
        if (s->g3a88 == 1) { s->g3aff = 0; return 0; }
        if (s->g3a60 == 0) { h->fatal(); return 0; }
        h->f7bf40(); h->ord10043();
        s->g3a60 = 0; s->g3a84 = 0; s->g3a88 = 0; r = 1;
    }
    s->g3aff = 0;
    return r;
}
