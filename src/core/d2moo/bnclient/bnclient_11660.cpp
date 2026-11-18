// BNClient 0x11660: the BNCS message 0x46 (SID_NEWS_INFO) entry of the receive dispatch table (0x1E128 + 0x46*4).
#include <cstdint>
#include <emscripten.h>

// 0x12C30, the news-info body (src/core/bnclient/bnclient_12c30.cpp), bound by the caller.
typedef uint32_t (*Bn11660BodyFn)(const uint8_t* payload, uint32_t size);

// 1.13c BNClient+0x11660 (12 B): handler(ECX = payload, EDX = size). A tail jump into 0x12C30 with both
// registers unchanged; the push/store/pop of EDX around it leaves no trace. Returns the body's result.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid_news_info_11660(const uint8_t* payload, uint32_t size,
                                                                      Bn11660BodyFn body) {
    return body(payload, size);
}
