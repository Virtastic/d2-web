// BNClient 0x15760: the BNCS message 0x0F (SID_CHATEVENT) entry of the receive dispatch table (0x1E128 + 0xF*4).
#include <cstdint>
#include <emscripten.h>

// 0x0FEC0 message queue: EAX = size, stack = (type, data), ret 8. Copies at most 0x120 bytes into a
// fresh queued message of the given type.
typedef int32_t (*Bn15760QueueFn)(uint32_t type, const void* data, uint32_t size);

// 1.13c BNClient+0x15760 (16 B): handler(ECX = payload, EDX = size). Hands the raw chat-event payload to
// the queue as message type 0 and returns 1; the payload is not parsed here.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid_chatevent_15760(const uint8_t* payload, uint32_t size,
                                                                      Bn15760QueueFn queue) {
    queue(0, payload, size);
    return 1;
}
