// 1.13c BNClient+0x12000: the logon-proof composer. ECX = the server payload, EAX = the salt pointer.
// B3A0 (the SRP step) decides; DAT_1ECB8[0x20] is zeroed either way; on success the 0x14 bytes at DAT_1ED40 go out as frame 0x54 (EB00).
#include <cstdint>
#include <cstring>

typedef uint32_t (*BnSrpStepFn)(const uint8_t* ecx, const uint8_t* eax);                       // 1.13c BNClient+0xB3A0
typedef void (*BnFrameSendFn)(uint32_t len, uint32_t id, const uint8_t* payload);                // 1.13c BNClient+0xEB00 (ECX = len, EDI = payload)

extern "C" uint32_t bnclient_logon_proof_12000(const uint8_t* ecx, const uint8_t* eax, uint8_t* ecb8, const uint8_t* ed40,
                                               BnSrpStepFn srp, BnFrameSendFn send) {
    const uint32_t ok = srp(ecx, eax);
    memset(ecb8, 0, 0x20);
    if (!ok) return 0;
    uint8_t frame[0x14];
    memcpy(frame, ed40, 0x14);
    send(0x14, 0x54, frame);
    return 1;
}
