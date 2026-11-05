// 1.13c D2CMP+0x10E30 (sub_10E30, 443 bytes). Stdcall, two words: out (a record the caller owns: +8 flags
// byte, +0xA length word, +0x10 block pointer) and stream (a 32-pixel-wide frame, 15 rows of 0x20 bytes).
// Encodes the same row runs as D2CMP+0x11000, for 15 rows, into a 0x5AF-byte scratch block from Fog #10042
// (ECX = size, line 0x128, zeroed by the function). Then copies the used bytes into a block of their own
// size (line 0x169), frees the scratch (Fog #10043, line 0x176), and fills out.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "d2cmp_rle_runs.h"

extern "C" __attribute__((weak)) uint8_t* d2cmp_fog_alloc(uint32_t size, const char* file, uint32_t line, uint32_t zero);
extern "C" __attribute__((weak)) void d2cmp_fog_fatal(uint32_t msg);
extern "C" __attribute__((weak)) void d2cmp_fog_free(void* p, uint32_t line, uint32_t zero);

// 1.13c D2CMP+0x10E30
extern "C"
void d2cmp_10e30(uint8_t* out, const uint8_t* stream) {
    uint8_t* scratch = d2cmp_fog_alloc(0x5AF, nullptr, 0x128, 0);
    if (!scratch) { d2cmp_fog_fatal(0x129); exit(-1); }
    memset(scratch, 0, 0x5AF);
    uint8_t* o = scratch;
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
    } while (row < 0xF);
    uint32_t len = (uint32_t)(o - scratch);
    if (len >= 0x10000) { d2cmp_fog_fatal(0x166); exit(-1); }
    uint8_t* blk = d2cmp_fog_alloc(len, nullptr, 0x169, 0);
    if (!blk) { d2cmp_fog_fatal(0x16A); exit(-1); }
    memcpy(blk, scratch, len);
    d2cmp_fog_free(scratch, 0x176, 0);
    out[8] |= 5;
    uint16_t l16 = (uint16_t)len;
    memcpy(out + 0xA, &l16, 2);
    uint32_t b32 = (uint32_t)(uintptr_t)blk;
    memcpy(out + 0x10, &b32, 4);
}
