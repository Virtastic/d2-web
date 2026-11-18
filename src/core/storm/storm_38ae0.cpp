// 1.13c Storm+0x38AE0 (20 B): pointer-table entry.
// Writes the byte 0xE9 at the pointer, a zero dword at pointer + 1, and returns pointer + 5 (a five-byte jmp rel32 with a zero displacement, emitted into a code buffer). ret 4.
#include <cstdint>
#include <cstring>

uint8_t* storm_38ae0(uint8_t* p) {
    p[0] = 0xE9;
    const uint32_t zero = 0;
    memcpy(p + 1, &zero, 4);
    return p + 5;
}
