// Storm+0x10E30 own-file port: queue one packet on a connection (register args conn EDI, pkt ESI, count EBX).
// Register convention: conn (EDI), pkt (ESI), count (EBX, the count Storm+0x10D20 reads; retail inherits it from the caller).
#include <cstdint>
#include <cstring>

// 10D20 seam: null in the engine (the real Storm_Sub10D20 runs); the FN probe sets it to log the call.
extern "C" uint32_t (*g_storm_10d20_seam)(const void* const*, const uint8_t*, uint32_t, uint32_t) = nullptr;
extern "C" uint32_t Storm_Sub10D20(const void* const* parts, const uint8_t* data, uint32_t length, uint32_t count);

// 1.13c Storm+0x10E30: queue one packet on a connection. conn +0x108 is the 16-byte record, pkt +0x02 is the length,
// +0x06 the word stored at slot +0x08 (index) of conn+0x1E6, and the dword at conn+0x1F0+index*4 is cleared.
extern "C" void storm_port_10e30(uint32_t* r) {  // 1.13c Storm+0x10E30
    uint8_t* conn = (uint8_t*)(uintptr_t)r[0];
    const uint8_t* pkt = (const uint8_t*)(uintptr_t)r[1];
    const uint32_t count = r[2];
    const void* part = conn + 0x108;
    uint16_t len, word;
    std::memcpy(&len, pkt + 0x02, 2);
    std::memcpy(&word, pkt + 0x06, 2);
    if (g_storm_10d20_seam) g_storm_10d20_seam(&part, pkt, len, count);
    else Storm_Sub10D20(&part, pkt, len, count);
    const uint32_t index = pkt[0x08];
    std::memcpy(conn + 0x1E6 + index * 2, &word, 2);
    const uint32_t zero = 0;
    std::memcpy(conn + 0x1F0 + index * 4, &zero, 4);
}
