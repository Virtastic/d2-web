// BNClient 0x11670: the BNCS message 0x15 (SID_CHECKAD) entry of the receive dispatch table (0x1E128 + 0x15*4).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

// 0x0E120 download starter: stack (kind, flags, p3, p4, name, url), EBX = FILETIME* of the server copy.
typedef void (*Bn11670StartFn)(uint32_t kind, uint8_t flags, uint32_t p3, uint32_t p4, const char* name,
                               const char* url, const uint32_t* filetime);

// 1.13c BNClient+0x11670 (99 B): handler(ECX = payload, EDX = size).
// Payload: +0 u32 ad id, +4 u32 extension, +8 FILETIME, +0x10 file name (NUL), then the link URL (NUL).
// Under 0x11 bytes, or an ad id equal to the last one seen (0x6FF3F648), nothing happens. Otherwise the
// id is remembered first, and only when both strings are non-empty the ad download is started as kind
// 0x80000003, flags 5. The strings are not bounded by `size` (retail scans them unchecked). Returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid_checkad_11670(const uint8_t* payload, uint32_t size,
                                                                    uint32_t* last_ad_id,
                                                                    Bn11670StartFn start) {
    if (size < 0x11) return 1;
    uint32_t ad_id, extension;
    std::memcpy(&ad_id, payload, 4);
    if (ad_id == *last_ad_id) return 1;
    std::memcpy(&extension, payload + 4, 4);
    *last_ad_id = ad_id;
    const char* name = (const char*)(payload + 0x10);
    const char* url = name + std::strlen(name) + 1;
    if (name[0] != 0 && url[0] != 0) {
        start(0x80000003u, 5, ad_id, extension, name, url, (const uint32_t*)(payload + 8));
    }
    return 1;
}
