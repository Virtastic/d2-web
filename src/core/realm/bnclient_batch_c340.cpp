// BNClient batch b49: 0xC340 0xC3A0 0xC4A0 0xC5F0 0xC6E0.
// Skipped (decompile stack-garbled, not faithful): 0xC520 0xC860 0xC960.
// Storm/ordinal calls and DLL globals are injected.
#include <cstdint>
#include <cstring>

typedef int (*BnSendFn)(uint32_t len, uint32_t id);   // 0xEB00 (<0 on failure)
typedef int (*BnGateFn)();                            // Ordinal_10227
typedef int (*BnRawSendFn)(uint32_t len);             // Ordinal_10007 after Ordinal_491 copied the header

struct BnLink { int sockUp; int closing; int pending; };  // DAT+0x3F654, +0x3F660, +0x3F644

// 1.13c BNClient+0x0C340
// Gate call (result ignored), then send id 0x21 with length 0x0E. Always true.
extern "C" uint32_t d2_bnclient_c340(BnGateFn gate, BnSendFn send) {
    gate();
    send(0x0E, 0x21);
    return 1;
}

// 1.13c BNClient+0x0C3A0
// Gate call, time(NULL), then send id 0x15 with length 0x10. Always true.
extern "C" uint32_t d2_bnclient_c3a0(BnGateFn gate, void (*timeNow)(), BnSendFn send) {
    gate();
    timeNow();
    send(0x10, 0x15);
    return 1;
}

// Bare 4-byte frame {FF id 04 00}: if the link is down or closing set pending, else send; false on send error.
static uint32_t bn_bare(BnLink* l, uint8_t id, BnRawSendFn raw) {
    if (!l->sockUp || l->closing) { l->pending = 1; return 1; }
    uint8_t hdr[4] = {0xFF, id, 4, 0};
    (void)hdr;  // Ordinal_491 copies it into the send buffer before raw(4)
    return raw(4) < 0 ? 0 : 1;
}

// 1.13c BNClient+0x0C4A0
// Bare frame id 0x10.
extern "C" uint32_t d2_bnclient_c4a0(BnLink* l, BnRawSendFn raw) { return bn_bare(l, 0x10, raw); }

// 1.13c BNClient+0x0C5F0
// Payload u32 flags + name (<=0x100). Name null/empty: flags=1, name "Diablo II", +4 if gate nonzero and
// param3 == 0; else flags = (param2 ? 2 : 0). Sends id 0x0C, length strlen(name)+5. Always true.
extern "C" uint32_t d2_bnclient_c5f0(const char* name, int param2, int param3, BnGateFn gate,
                                     BnSendFn send, uint8_t* buf /* >= 0x104 B */) {
    uint32_t flags;
    memset(buf, 0, 0x104);
    if (!name || !*name) {
        flags = 1;
        if (gate() != 0 && param3 == 0) flags |= 4;
        name = "Diablo II";
    } else {
        flags = param2 ? 2 : 0;
    }
    memcpy(buf, &flags, 4);
    strncpy((char*)buf + 4, name, 0x100);
    send((uint32_t)strlen((char*)buf + 4) + 5, 0x0C);
    return 1;
}

// 1.13c BNClient+0x0C6E0
// Clear DAT+0x3EE58 and +0x3EFD8, send bare frame id 0x40, then FUN_10D90(0x40); returns DAT+0x3F884.
extern "C" uint32_t d2_bnclient_c6e0(BnLink* l, uint32_t* g3ee58, uint32_t* g3efd8, BnRawSendFn raw,
                                     void (*f10d90)(uint32_t), uint32_t g3f884) {
    *g3ee58 = 0;
    *g3efd8 = 0;
    if (!l->sockUp || l->closing) l->pending = 1;
    else {
        uint8_t hdr[4] = {0xFF, 0x40, 4, 0};
        (void)hdr;
        if (raw(4) < 0) return 0;
    }
    f10d90(0x40);
    return g3f884;
}
