// 1.13c BNClient+0x11220: the logon-exchange reply handler. size < 0x44 -> 0. dword0: 0 -> the logon-proof composer on
// payload+4 / payload+0x24, 1 -> report 0xD and DAT_1E120 = 1, 5 -> the 11BF0 body with param 1, else report 0x12 -> 0.
#include <cstdint>
#include <cstring>

struct Bn11220Cb {
    uint32_t (*proof)(const uint8_t* ecx, const uint8_t* eax);                  // 1.13c BNClient+0x12000
    void (*report)(uint32_t code);                                               // 1.13c BNClient+0xFCF0
    void (*pre)();                                                               // 1.13c BNClient+0xAED0
    void (*send)(uint32_t sock, const uint8_t* buf, uint32_t len);               // 1.13c BNClient+0x80FC
};

extern "C" uint32_t bnclient_handle_11220(const uint8_t* payload, uint32_t size, uint32_t* eb50, uint32_t sock, uint32_t blocked,
                                          uint32_t* pending, uint32_t* e120, const Bn11220Cb* cb) {
    if (size < 0x44) return 0;
    uint32_t kind; memcpy(&kind, payload, 4);
    if (kind == 0) return cb->proof(payload + 4, payload + 0x24);
    if (kind == 1) { cb->report(0xD); *e120 = 1; return 1; }
    if (kind == 5 && ++*eb50 < 2) {
        cb->pre();
        const uint8_t frame[4] = { 0xFF, 0x57, 4, 0 };
        if (sock != 0 && blocked == 0) { cb->send(sock, frame, 4); return 1; }
        *pending = 1;
        return 1;
    }
    cb->report(0x12);
    return 0;
}
