// 1.13c BNClient+0xF050: the 0x33 GetFileTime request for "bnserver-D2DV.ini".
// The payload is built on the stack: u32 0x80000004 (root key), u32 0 (request id), then the name with its NUL
// (n + 1 bytes, n = the length Ordinal_501 returns). The frame sender gets ECX = n + 9 and id 0x33, EDI = payload.
// Callees are injected: BnF050Env::str_copy is Ordinal_501 (thunk 0x804E) and send is the EB00 frame sender.
#include <cstdint>
#include <cstring>

struct BnF050Env {
    int32_t (*str_copy)(char* dst, const char* src, uint32_t max);   // Ordinal_501 (SStrCopy), stdcall 3
    void (*send)(uint32_t len, uint8_t id, const uint8_t* payload);  // BNClient+0xEB00
};

namespace {
const char kServerIniName[] = "bnserver-D2DV.ini";  // module literal at RVA 0x1AD58
}  // namespace

extern "C" void bnclient_server_ini_f050(const BnF050Env* e) {
    uint8_t payload[8 + 0x104];
    char buf[0x104];
    const uint32_t rootKey = 0x80000004u, requestId = 0;
    std::memcpy(payload, &rootKey, 4);
    std::memcpy(payload + 4, &requestId, 4);
    const int32_t n = e->str_copy(buf, kServerIniName, 0x104);
    std::memcpy(payload + 8, buf, (size_t)n + 1);
    e->send((uint32_t)n + 9, 0x33, payload);
}
