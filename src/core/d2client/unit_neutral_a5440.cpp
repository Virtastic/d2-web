#include <cstdint>
// 1.13c D2Client+0xA5440
extern "C" uint32_t d2_client_a5440(uint32_t type, uint32_t mode, uint32_t flags) {
    if (flags & 0x10000u) return 1;
    if (type == 0) return mode == 0 || mode == 0x11;
    if (type == 1) return mode == 0 || mode == 0xC;
    return 0;
}
