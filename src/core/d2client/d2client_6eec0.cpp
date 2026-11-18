// 1.13c D2Client+0x6EEC0: chat history recall. Copies the 0x202-byte entry idx of the
// history ring (stride 0x202) to the edit line, then stores the entry's UTF-16 length into the length global.
#include <cstdint>
#include <cstring>

extern "C" void d2c_port_6eec0(uint32_t idx, const uint8_t* ring, uint8_t* line, uint32_t* lenOut) {
    const uint8_t* e = ring + (uint64_t)idx * 0x202;
    memcpy(line, e, 0x202);
    uint32_t n = 0;
    for (;; ++n) { uint16_t c; memcpy(&c, e + 2 * n, 2); if (!c) break; }
    *lenOut = n;
}
