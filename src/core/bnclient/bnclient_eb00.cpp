// 1.13c BNClient+0x0EB00 send_frame (thiscall: ECX = payload length, EDI = payload or NULL, stack word 0 = frame id, ret 4).
// Transcribed from the disassembly (objdump of retail Bnclient.dll):
//   header = FF, id, u16 (len + 4) taken from the ORIGINAL length, written before the gate;
//   gate: conn != 0 and closing == 0; otherwise disconnect := 1 and return 0;
//   len := 0 when the payload is NULL; copy header (4 bytes) to sendbuf+0;
//   copy payload to sendbuf+4 only when payload != NULL and len + 4 < 0x2000;
//   return send(len + 4) with the buffer at sendbuf.
#include <cstdint>

namespace d2::bnclient::eb00 {
struct SendState {
    uint32_t conn;        // DAT_6FF3F654 (0x13654)
    int32_t closing;      // DAT_6FF3F660 (0x13660)
    int32_t disconnect;   // DAT_6FF3F644 (0x13644)
};
struct SendEnv {
    void (*copy)(void* dst, const void* src, uint32_t n);  // Storm #491 (SMemCopy), stdcall 3 args
    int32_t (*send)(uint32_t size);                        // Ordinal_10007 via 0x80FC: ECX = conn, EDX = sendbuf, stack = size
    uint8_t* sendbuf;                                      // the 0x2000-byte frame buffer on the caller's stack
};

int32_t send_frame(SendState& s, const SendEnv& e, uint32_t len, uint8_t id, const void* payload) {
    if (s.conn != 0 && s.closing == 0) {
        const uint16_t header_len = static_cast<uint16_t>(len + 4);
        const uint8_t hdr[4] = {0xFF, id, static_cast<uint8_t>(header_len & 0xFF), static_cast<uint8_t>(header_len >> 8)};
        const uint32_t n = payload != nullptr ? len : 0;
        e.copy(e.sendbuf, hdr, 4);
        if (payload != nullptr && n + 4 < 0x2000) e.copy(e.sendbuf + 4, payload, n);
        return e.send(n + 4);
    }
    s.disconnect = 1;
    return 0;
}
}
