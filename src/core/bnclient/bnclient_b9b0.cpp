// BNClient 0xB9B0: sends BNCS frame 0x5D. A 0x5DC-byte local buffer is handed to the fill routine at 0x8150 (a
// `jmp [IAT]` thunk; ECX = buffer + 4, EDX = &length, length preset to 0x5DC). On a non-zero answer the frame
// goes out through 0x0EB00 (ECX = the length the fill left, id 0x5D, EDI = the buffer).
#include <cstdint>
#include <emscripten.h>

typedef uint32_t (*Bn5dFillFn)(uint8_t* buffer, uint32_t* length);                          // 1.13c BNClient+0x8150
typedef int32_t (*Bn5dSendFn)(uint32_t length, uint8_t id, const uint8_t* payload);          // 1.13c BNClient+0xEB00

// 1.13c BNClient+0x0B9B0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_send_frame5d_b9b0(Bn5dFillFn fill, Bn5dSendFn send) {
    uint8_t buffer[0x5DC];
    uint32_t length = 0x5DC;
    if (fill(buffer, &length) == 0) return 0;
    return send(length, 0x5D, buffer) >= 0 ? 1 : 0;
}
