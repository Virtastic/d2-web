// 1.13c BNClient+0x11BF0: queue the empty SID 0x57 frame. param != 0 counts DAT_1EB50 and bails (0) from the
// second call; else the pre-step runs; connected and not blocked -> the 4-byte frame FF 57 04 00 is sent (80FC: socket, buffer, 4); otherwise
// DAT_1F644 (pending) is set. Returns 1.
#include <cstdint>
#include <cstring>

typedef void (*BnPreFn)();                                                       // 1.13c BNClient+0xAED0 on DAT_1F8E4
typedef void (*BnSendFn)(uint32_t sock, const uint8_t* buf, uint32_t len);       // 1.13c BNClient+0x80FC (Storm 10007)

extern "C" uint32_t bnclient_send_57_11bf0(int32_t param, uint32_t* eb50, uint32_t sock, uint32_t blocked, uint32_t* pending,
                                           BnPreFn pre, BnSendFn send) {
    if (param != 0 && ++*eb50 > 1) return 0;
    pre();
    const uint8_t frame[4] = { 0xFF, 0x57, 4, 0 };
    if (sock != 0 && blocked == 0) { send(sock, frame, 4); return 1; }
    *pending = 1;
    return 1;
}
