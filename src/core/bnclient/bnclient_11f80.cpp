// 1.13c BNClient+0x11F80: the 0x56 variant of 12000. As 12000, but the frame is DAT_1ED40[0x14] followed by
// DAT_1EC58[0x40] (0x54 bytes), and on success DAT_1EC58 is zeroed after the copy.
#include <cstdint>
#include <cstring>

typedef uint32_t (*BnSrpStepFn)(const uint8_t* ecx, const uint8_t* eax);                       // 1.13c BNClient+0xB3A0
typedef void (*BnFrameSendFn)(uint32_t len, uint32_t id, const uint8_t* payload);                // 1.13c BNClient+0xEB00

extern "C" uint32_t bnclient_logon_proof_11f80(const uint8_t* ecx, const uint8_t* eax, uint8_t* ecb8, const uint8_t* ed40,
                                               uint8_t* ec58, BnSrpStepFn srp, BnFrameSendFn send) {
    const uint32_t ok = srp(ecx, eax);
    memset(ecb8, 0, 0x20);
    if (!ok) return 0;
    uint8_t frame[0x54];
    memcpy(frame, ed40, 0x14);
    memcpy(frame + 0x14, ec58, 0x40);
    memset(ec58, 0, 0x40);
    send(0x54, 0x56, frame);
    return 1;
}
