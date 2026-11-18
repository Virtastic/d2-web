// 1.13c D2Multi+0x12120: EDI = game record. Appends (key u16 @+8, u16 @+0x14, name @+0x1c, text @+0x2c) as a 0x3b-byte row of the table at module+0x1bf21
// unless a row with the same key (u16 at row+0x32) exists among the first *count rows; the row's u32 at +0x36 is cleared; *count (DAT_6fa09dcc) grows by one.
#include <cstdint>
#include <cstring>

void d2multi_12120(uint8_t* tbl /* module+0x1bf21 */, int32_t* count, const uint8_t* rec) {
    for (int32_t i = 0; i < *count; i++) if (!memcmp(tbl + i * 0x3b + 0x32, rec + 8, 2)) return;
    uint8_t* row = tbl + *count * 0x3b;
    memcpy(row + 0x32, rec + 8, 2);
    memset(row + 0x36, 0, 4);
    memcpy(row + 0x30, rec + 0x14, 2);
    const uint8_t* s = rec + 0x1c; uint8_t* d = row;
    do { *d++ = *s; } while (*s++);
    s = rec + 0x2c; d = row + 0x10;
    do { *d++ = *s; } while (*s++);
    ++*count;
}
