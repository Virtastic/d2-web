// Prior art used: D2MOO D2Lang string-table header notes; cached retail 1.13c routine.
#include <cstdint>
#include <cstdlib>
#include <cstring>

extern "C" void d2_fn_d2lang_92f0(uint16_t*, const uint8_t*, uint32_t, uint32_t);

// 1.13c D2Lang+0x094E0: validate a loaded .tbl image (retail fetches it via sub_7EF0, here the caller passes it).
// Header (packed): u16 crc @0, u16 n @2, i32 count @4, u32 dataStart @9, u32 size @0x11.
// CRC16 over [dataStart,size) must equal the stored crc; each 0x11-byte node at 0x20+2n has
// two u32 (at -4 and 0) that must be < size. Any failure exits -1 in retail; returns tbl on success.
extern "C" uint8_t* d2_fn_d2lang_94e0(uint8_t* tbl)
{
    uint32_t start, size, a, b;
    std::memcpy(&start, tbl + 9, 4);
    std::memcpy(&size, tbl + 0x11, 4);
    uint16_t crc, stored;
    d2_fn_d2lang_92f0(&crc, tbl, start, size);
    std::memcpy(&stored, tbl, 2);
    if (crc != stored) std::exit(-1);
    int32_t count; uint16_t n;
    std::memcpy(&count, tbl + 4, 4);
    std::memcpy(&n, tbl + 2, 2);
    uint8_t* p = tbl + 2 * (uint32_t)n + 0x20;
    for (int32_t i = 0; i < count; i++, p += 0x11) {
        std::memcpy(&a, p - 4, 4);
        std::memcpy(&b, p, 4);
        if (size <= a || size <= b) std::exit(-1);
    }
    return tbl;
}
