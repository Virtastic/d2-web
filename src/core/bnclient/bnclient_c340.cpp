// BNClient 0xC340: sends BNCS frame 0x21 (14-byte payload): "68XI", the product code ("D2XP" when 0x8126 answers
// non-zero, else "D2DV"), the version dword DAT+0x3F648, and two zero bytes. Always returns 1.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

typedef uint32_t (*Bn21ExpansionFn)();                                                         // 1.13c BNClient+0x8126
typedef int32_t (*Bn21SendFn)(uint32_t length, uint8_t id, const uint8_t* payload);            // 1.13c BNClient+0xEB00

// 1.13c BNClient+0x0C340
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_send_id21_c340(const uint32_t* version, Bn21ExpansionFn expansion,
                                                                 Bn21SendFn send) {
    uint8_t payload[14];
    const uint32_t tag = 0x49583836u;
    const uint32_t product = expansion() != 0 ? 0x44325850u : 0x44324456u;
    memcpy(payload, &tag, 4);
    memcpy(payload + 4, &product, 4);
    memcpy(payload + 8, version, 4);
    payload[12] = 0;
    payload[13] = 0;
    send(14, 0x21, payload);
    return 1;
}
