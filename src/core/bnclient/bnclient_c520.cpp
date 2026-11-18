// BNClient 0xC520 (stdcall a, b; ret 8): two BNCS frames. First id 0x0B with the 4-byte product word, 'PX2D'
// (0x44325850) when the gate (Fog 10227) is nonzero, else 'VD2D' (0x44324456); its send result is ignored.
// Then a zeroed 0x200-byte buffer gets SStrCopy(buf, a, 0x100) and SStrCopy(buf + strlen + 1, b, 0x100); id 0x0A
// is sent with length strlen(a') + strlen(b') + 2. Returns 1 when that send is >= 0.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct BnC520Seams {
    int32_t (*gate)();                                                // 1.13c BNClient+0x8126 (Fog 10227)
    int32_t (*send)(uint32_t len, uint32_t id, const uint8_t* pl);   // 1.13c BNClient+0xEB00
    uint32_t (*strcopy)(char* d, const char* s, uint32_t cap);        // 1.13c BNClient+0x804E (Storm 501)
};

// 1.13c BNClient+0x0C520
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_send_string_pair_c520(const char* a, const char* b, const BnC520Seams* s) {
    uint32_t product = s->gate() != 0 ? 0x44325850u : 0x44324456u;
    uint8_t w[4]; std::memcpy(w, &product, 4);
    s->send(4, 0x0B, w);
    char buf[0x200];
    std::memset(buf, 0, sizeof buf);
    s->strcopy(buf, a, 0x100);
    s->strcopy(buf + std::strlen(buf) + 1, b, 0x100);
    const uint32_t n1 = (uint32_t)std::strlen(buf);
    const uint32_t len = n1 + (uint32_t)std::strlen(buf + n1 + 1) + 2;
    return s->send(len, 0x0A, (const uint8_t*)buf) >= 0 ? 1u : 0u;
}
