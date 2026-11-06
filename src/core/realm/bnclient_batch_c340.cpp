// BNClient batch b49: 0xC340 0xC3A0 0xC5F0.
// Skipped (decompile stack-garbled, not faithful): 0xC520 0xC860 0xC960.
// Storm/ordinal calls and DLL globals are injected.
#include <cstdint>
#include <cstring>

typedef int (*BnSendFn)(uint32_t len, uint32_t id);   // 0xEB00 (<0 on failure)
typedef int (*BnGateFn)();                            // Ordinal_10227

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

// 0xC4A0 and 0xC6E0 moved to src/core/bnclient/bnclient_c4a0.cpp and bnclient_c6e0.cpp.

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
