// 1.13c D2CMP+0x10AA0 (sub_10AA0, 199 bytes), read from the retail disassembly (the cached decompile
// takes the source pointer from a register and mislabels the stack words). Stdcall, two words:
// out (the record the caller owns) and src (a table of 15 rows, 0x20 bytes apart).
//
// Builds a zeroed 256-byte block from Fog #10042 (size 0x100 in ECX), then copies 15 rows: row i copies
// size_i bytes from src + 0x20*i + off_i into the block, back to back. size starts at 4 and grows by 4 for
// rows 0..6, then shrinks by 4; off starts at 14 and falls by 2 for rows 0..6, then climbs by 2. The sizes sum
// to 0x100. Then out->+8 = 1 (word), out->+0xA = 0x100 (word), out->+0x10 = block (dword).
#include <cstdint>
#include <cstring>
#include <cstdlib>

// Seams: Fog #10042 alloc(size in ECX, file in EDX, line, zero), #10265+#10024 fatal (msg 0x186), then _exit(-1).
extern "C" __attribute__((weak)) void* d2cmp_fog_alloc(uint32_t size, const char* file, uint32_t line, uint32_t zero);
extern "C" __attribute__((weak)) void d2cmp_fog_fatal(uint32_t msg);

// The fill half: takes the block the allocator returned. The probe calls this directly with its own arena.
extern "C"
void d2cmp_10aa0_fill(uint8_t* out, const uint8_t* src, uint8_t* block) {
    memset(block, 0, 0x100);
    uint32_t dst = 0;
    int32_t off = 0xE;
    uint32_t size = 4;
    for (uint32_t i = 0; i < 0xF; i++) {
        const uint8_t* row = src + 0x20 * i;
        memcpy(block + dst, row + off, size);
        dst += size;
        if (i < 7) { off -= 2; size += 4; }
        else       { off += 2; size -= 4; }
    }
    uint16_t one = 1, span = 0x100;
    memcpy(out + 8, &one, 2);
    memcpy(out + 0xA, &span, 2);
    uint32_t blk = (uint32_t)(uintptr_t)block;
    memcpy(out + 0x10, &blk, 4);
}

// 1.13c D2CMP+0x10AA0
extern "C"
void d2cmp_10aa0(uint8_t* out, const uint8_t* src) {
    uint8_t* block = (uint8_t*)d2cmp_fog_alloc(0x100, nullptr, 0x185, 0);   // file string at 0x6FE2FE68: diagnostic only
    if (!block) { d2cmp_fog_fatal(0x186); exit(-1); }
    d2cmp_10aa0_fill(out, src, block);
}
