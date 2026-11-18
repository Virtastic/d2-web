// D2Client+0x22CA0: register-arg helper (EAX=obj, EDI=slot 8..11); env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2c22CA0Env {
    int32_t (*f22430)();                 // D2Client+0x22430 (returns ESI; ==1 gates)
    uint8_t* (*f22A70)();                // D2Client+0x22A70 (result in EAX; record ptr)
    int32_t (*f220A0)(uint32_t eax);     // D2Client+0x220A0 (obj+0x7C, fatal if null)
    uint32_t (*f38A70)(uint32_t eax);    // D2Client+0x88A70 (args eax,0,0,0; returns obj in EAX)
    uint32_t (*ord10265)(uint32_t id);   // unverified
    void (*ord10024)(uint32_t str);      // unverified
    void (*exit_)(int32_t code);         // D2Client+0x3664
    uint32_t g6fbc97f8;                  // DAT D2Client+0xC97F8 (pinned by vector)
    uint32_t* g6fbcc4c4;                 // DAT D2Client+0xCC4C4 (written)
};
extern "C" {
// 1.13c D2Client+0x22CA0
EMSCRIPTEN_KEEPALIVE void d2c_22ca0(const D2c22CA0Env* e, uint32_t eax, uint32_t edi, uint8_t* mem) {
    if (e->f22430() != 1) return;
    const uint8_t* r = e->f22A70();
    if (!r) return;
    uint32_t now = e->g6fbc97f8, v;
    if (edi == 8) v = *(const uint32_t*)(r + 0x44);
    else if (edi == 9) v = *(const uint32_t*)(r + 0x48);
    else if (edi == 10) v = *(const uint32_t*)(r + 0x4c);
    else if (edi == 11) v = *(const uint32_t*)(r + 0x50);
    else return;
    if (v == 0xa84) { if (now - (uint32_t)e->f220A0(eax) < 0x19) return; }
    else if (v == 0) return;
    uint32_t o = e->f38A70(eax);
    if (o == 0) { e->ord10024(e->ord10265(0xcc6)); e->exit_(-1); return; }
    *(uint32_t*)(mem + o + 0x7c) = now;  // mem = guest memory base
    *e->g6fbcc4c4 = now;
}
}
