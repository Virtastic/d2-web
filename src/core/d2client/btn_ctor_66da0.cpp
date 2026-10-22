// 1.13c D2Client+0x66DA0 (258 B): buy/sell-style button constructor, from the cached decompile.
// Object = retail 32-bit layout (byte-addressed); vtable stores and imports are host seams.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2ClientBtn66da0Env {
    uint32_t vtblBase, vtblBtn;                    // 0x6FB85048, 0x6FB850C0
    const uint16_t* frameTbl;                      // DAT 0x6FB928F0, stride 4 B
    uint32_t cel;                                  // DAT 0x6FBCB9C4 (lazy cel handle, 0 = unloaded)
    void (*fmtPath)();                             // wsprintfA("%s\\ui\\panel\\buysellbtn","DATA\\GLOBAL")
    uint32_t (*sub2b420)(uint32_t zero);           // D2Client+0x2B420
    uint32_t (*ord10021)(uint32_t* frame18);
    uint32_t (*ord10000)(uint32_t);
    uint32_t (*ord10060)(uint32_t);
};

// 1.13c D2Client+0x66DA0
extern "C" EMSCRIPTEN_KEEPALIVE uint8_t* d2_client_66da0(
    uint32_t p2, uint8_t* o, uint32_t p4, int32_t idx, D2ClientBtn66da0Env* e) {
    uint32_t* w = (uint32_t*)o;
    w[0] = e->vtblBase;
    w[0] = e->vtblBtn;
    uint16_t fr = e->frameTbl[idx * 2];
    memcpy(o + 0x30, &fr, 2);
    memcpy(o + 0x32, &idx, 4);
    w[10] = p2; w[8] = 0; w[11] = 0;
    if (!e->cel) { e->fmtPath(); e->cel = e->sub2b420(0); }
    uint32_t f[0x12] = {};
    f[0] = fr; f[0xd] = e->cel;
    uint32_t r = e->ord10021(f);
    w[2] = (uint32_t)(uintptr_t)o;
    w[3] = p4; w[9] = 0;
    w[6] = e->ord10000(r);
    w[7] = e->ord10060(r);
    return o;
}
