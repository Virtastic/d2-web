// D2Client rva 0x7E970: the per-frame client speech/interaction tick.
// Non-standard convention (ndisasm at 0x6FB2E970): EAX = arg0 (the cancel/forget flag), ECX = arg1 (the
// interaction owner; its mode byte is at +0x16). Callees are injected, GetTickCount is the `now` argument.
#include <cstddef>
#include <cstdint>
#include <cstring>

struct __attribute__((packed)) D2Client7E970Arg1 {   // the interaction owner (arg1); [arg1+0x16] is an unaligned dword
    uint8_t pad[0x16];
    uint32_t mode;           // [arg1+0x16]: 2 stamps "cancelled/forgotten" on the arg0 path
};
static_assert(offsetof(D2Client7E970Arg1, mode) == 0x16, "D2Client 0x7E970 arg1");

struct D2Client7E970Callees {
    uint32_t (*soundPending)();                                 // 0xD270, D2Sound ordinal 10001
    uint32_t (*samplePlaying)(uint32_t unit);                   // 0x884D0
    uint32_t (*interactEnd)(D2Client7E970Arg1* a1);             // 0x7BB40, ret 4
    void (*c7BF40)();                                           // 0x7BF40
    void (*c7CEF0)();                                           // 0x7CEF0
    void (*c486A0)(uint32_t eax);                               // 0x486A0 (its argument arrives in EAX)
    uint32_t (*winHandle)();                                    // 0xD0B4, D2Gfx ordinal 10048
    void (*msg3CC60)(uint32_t id, uint32_t win, uint32_t str);  // 0x3CC60, EAX = id, two stack words, ret 8
    uint32_t (*uiCallback)(uint32_t ecx, uint32_t zero);        // [0x11BF94], ret 4
};

// g layout (offset: module rva)
enum {
    G_UNIT = 0,      // 0x113B0B  the pending speech unit
    G_CONSUMED = 4,  // 0x113A7C  set to 1 when this frame was consumed
    G_SESSION = 8,   // 0x113ACA  the speech session is active
    G_OWNER = 12,    // 0x113A68  the interaction owner pointer
    G_C2FC = 16,     // 0x11C2FC
    G_CB = 20,       // 0x11BF94  the registered UI callback (a code pointer)
    G_CB_ECX = 24,   // 0x11BF98  its ECX argument
    G_A84 = 28,      // 0x113A84  the render-deadline arm
    G_D5D4 = 32,     // 0xDD5D4
    G_DEADLINE = 36, // 0xFAD50   the auto-close deadline
    G_3448 = 40,     // 0x123448
    G_3AC2 = 44,     // 0x113AC2
};
static uint32_t rd(const uint8_t* p, uint32_t o) { uint32_t v; memcpy(&v, p + o, 4); return v; }
static void wr(uint8_t* p, uint32_t o, uint32_t v) { memcpy(p + o, &v, 4); }

// 1.13c D2Client+0x7E970
extern "C" uint32_t d2_client_7e970(uint32_t flag, D2Client7E970Arg1* a1, uint8_t* g, uint32_t now, uint32_t base,
                                    const D2Client7E970Callees& k) {
    if (flag != 0) { wr((uint8_t*)a1, 0x16, 2); return 0; }     // 0x6FB2E974: the cancel/forget arm
    const uint32_t unit = rd(g, G_UNIT);
    const uint32_t snd = k.soundPending();
    if (unit != 0 && k.samplePlaying(unit) == 0 && snd != 0) return 1;   // still an event to finish: hold
    wr(g, G_UNIT, 0);
    if (k.interactEnd(a1) == 0) return 1;                       // the interaction produced no successor
    wr(g, G_CONSUMED, 1);
    if (rd(g, G_SESSION) == 0 && rd(g, G_OWNER) != 0) { k.c7BF40(); wr(g, G_OWNER, 0); wr(g, G_C2FC, 0); }
    k.c7CEF0();
    const uint32_t cb = rd(g, G_CB);
    if (cb != 0) return k.uiCallback(rd(g, G_CB_ECX), 0);       // the registered UI callback's own value
    if (rd(g, G_A84) == 1) wr(g, G_A84, 0);
    else if (rd(g, G_SESSION) != 0) k.c486A0(rd(g, G_3AC2));
    if (rd(g, G_D5D4) != 0) wr(g, G_DEADLINE, now + 100);
    wr(g, G_SESSION, 0);
    wr(g, G_C2FC, 0);
    if (rd(g, G_3448) == 1) { k.msg3CC60(7, k.winHandle(), base + 0xE1388); wr(g, G_3448, 0); }
    return 0;
}
