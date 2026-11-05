// 1.13c D2CMP+0x11000 (sub_11000, 246 bytes). Stdcall, two words: stream (a 32x32 frame, 0x20 bytes per row,
// read in place) and outLen (a dword the function writes last). Returns the encoded block in EAX.
//
// Encodes 32 rows. Each row is a transparent run (count 0..0x7F of zero pixels) then a literal run (count and
// its bytes). A row that ends on a run closes with 00 00; a full 0x7F transparent run is the single byte 7F.
// The block is 0xC20 bytes from Fog #10042 (ECX = size, line 0x1C), with no zero fill and no NULL check.
#include <cstdint>
#include <cstdlib>
#include "d2cmp_rle_runs.h"

extern "C" __attribute__((weak)) uint8_t* d2cmp_fog_alloc(uint32_t size, const char* file, uint32_t line, uint32_t zero);

// 1.13c D2CMP+0x11000
extern "C"
uint8_t* d2cmp_11000(const uint8_t* stream, uint32_t* outLen) {
    uint8_t* base = d2cmp_fog_alloc(0xC20, nullptr, 0x1C, 0);
    uint8_t* o = base;
    const uint8_t* p = stream;
    const uint8_t* rowNext = stream + 0x20;
    const uint8_t* rowEnd = stream + 0x20;
    uint32_t row = 0;
    do {
        uint32_t f1, f2, g1 = 0, g2 = 0;
        uint32_t n1 = d2cmp_run_eq(p, rowEnd, 0, 0x7F, &f1, &f2);
        p += n1;
        bool endRow;
        if (f1 || f2) {
            endRow = true;
        } else if (n1 == 0x7F) {
            *o++ = 0x7F;
            endRow = false;
        } else {
            uint32_t n2 = d2cmp_run_ne(p, rowEnd, 0, 0x7F, &g1, &g2);
            *o++ = (uint8_t)n1;
            *o++ = (uint8_t)n2;
            for (uint32_t i = 0; i < n2; i++) *o++ = *p++;
            endRow = (g1 || g2);
        }
        if (endRow) {
            *o++ = 0;
            *o++ = 0;
            p = rowNext;
            rowNext += 0x20;
            rowEnd += 0x20;
            row++;
        }
    } while (row < 0x20);
    *outLen = (uint32_t)(o - base);
    return base;
}
