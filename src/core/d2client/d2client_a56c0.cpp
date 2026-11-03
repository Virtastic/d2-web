// 1.13c D2Client+0xA56C0: skill-row class test. EAX = skill row index; two stack words (ret 8) are handed on to the class
// handler as ECX and EDX. Row = [sgptDataTables+0xB98] + index * 0x23C; its byte +0x15 selects the handler in the
// table at D2Client+0xDE838 (entry count: dword at +0xDE854). Returns 1 when the handler's result is non-zero, else 0.
// Callers: 0xA6070.
#include <cstdint>
#include <emscripten.h>

// Host view of the globals it reads: rows = [tables+0xB98], count = [tables+0xBA0], nclass = dword 0xDE854,
// classify = the function table 0xDE838 (called with ECX, EDX).
struct D2cA56c0Env {
    const uint8_t* rows;
    int32_t count;
    int32_t nclass;
    uint32_t (*const* classify)(uint32_t ecx, uint32_t edx);
};

extern "C" {
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_a56c0(int32_t eax, uint32_t ecx, uint32_t edx, const D2cA56c0Env* e) {
    if (eax < 0 || eax >= e->count) return 0;
    const uint8_t* rec = e->rows + (size_t)(uint32_t)eax * 0x23C;
    if (!rec) return 0;
    const uint8_t b = rec[0x15];
    if (b == 0 || (int32_t)b >= e->nclass) return 0;
    return e->classify[b](ecx, edx) != 0 ? 1u : 0u;
}
}
