// 1.13c D2Client+0x440C0 (79 B): one-shot pass over the room list, flag bit0 of +0x34 marks "seen".
// Seams: ord_10717 (CCC4 thunk) head of list at DAT_6fbcc3b8; ord_10826 (C1DE thunk) unit kind.
#include <cstdint>
#include <emscripten.h>
typedef uintptr_t U;
struct D2Client440C0Env {
    U (*ord_10717)(uint32_t head);          // 0xCCC4
    void (*fn_18370)(U n);                  // 0x18370
    void (*fn_a7d00)();                     // 0xA7D00 (register args unrecovered)
    int32_t (*ord_10826)(U n);              // 0xC1DE
    void (*fn_53550)(U n);                  // 0x53550
};
// 1.13c D2Client+0x440C0 ; cc3b8 = DAT_6fbcc3b8, cc2e0 = DAT_6fbcc2e0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_440c0(uint32_t cc3b8, uint32_t cc2e0, const D2Client440C0Env* e) {
    for (U n = e->ord_10717(cc3b8); n; n = *(uint32_t*)(n + 0x7c))
        if (!(*(uint8_t*)(n + 0x34) & 1)) {
            e->fn_18370(n);
            e->fn_a7d00();
            if (e->ord_10826(n) == 8 && cc2e0) e->fn_53550(n);
            *(uint32_t*)(n + 0x34) |= 1;
        }
}
