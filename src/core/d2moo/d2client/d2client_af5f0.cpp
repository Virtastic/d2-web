// 1.13c D2Client+0xAF5F0 (71 B, ECX = value, EDX = record, result = the callee's): builds a zeroed 7-dword block with
// word0 = record byte +7 and word1 = record byte +8, then calls 0x2A750 (ECX = record byte +6, EDX = the incoming ECX,
// stack: block, 1; ret 8) and returns its result.
#include <emscripten.h>
#include <cstdint>
extern "C" uint32_t d2c_port_af5f0(uint32_t val, const uint8_t* rec, uint32_t (*f2a750)(uint32_t ecx, uint32_t edx, uint32_t* blk, uint32_t one)) {
    uint32_t loc[7] = { rec[7], rec[8], 0, 0, 0, 0, 0 };
    return f2a750(rec[6], val, loc, 1);
}
