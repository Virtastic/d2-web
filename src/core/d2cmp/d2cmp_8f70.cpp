// 1.13c D2CMP+0x8F70: ret 0xC, three stack arguments after the Storm handle in ESI.
#include <cstdint>
#include "d2cmp_8f70.h"

// Returns when the handle is nonzero and Ordinal_10106(handle, a0, a1, a2) != -1. Otherwise reports and exits; a -1 result first calls
// Ordinal_276 with a 0x104-byte buffer. The report is Ordinal_10265 (code 0x79 after a failed Storm call, 0x70 for a zero handle), then
// Ordinal_10024(msg, result, code), then _exit(-1). The exit path does not return in retail.
extern "C" void d2cmp_8f70(void* esi, uint32_t a0, uint32_t a1, uint32_t a2, const char* msg, const D2Cmp8f70Ops& ops) {
    uint32_t code = 0x70;
    if (esi) {
        if (ops.storm10106(esi, a0, a1, a2) != -1) return;
        uint8_t buf[0x104];
        ops.ord276(esi, buf, sizeof buf);
        code = 0x79;
    }
    uint32_t v = ops.ord10265(code);
    ops.ord10024(msg, v, code);
    ops.exit_(-1);
}
