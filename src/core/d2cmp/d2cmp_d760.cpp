// src/core/d2cmp/d2cmp_d760.cpp -- 1.13c D2CMP sub_D760, transcribed from the disassembly.
#include <cstdint>

struct D2CmpCelEnv {
    void* (*celDecode)(uint8_t* src, void* a1, void* table, uint32_t a3, uint8_t* obj, uint32_t a5, uint32_t a6);
    void (*release)(uint32_t arg);
    void* table;
};
int32_t d2cmp_d76e(uint8_t* a1, uint32_t a2, uint32_t* ctx, const D2CmpCelEnv* e);

// stdcall(a1, a2, ctx) ret 0xC: the cel-cache callback guarded by a module flag at 0x6FE77270; flag 0 returns 0
// without touching anything, else the body is 0xD76E.
// 1.13c D2CMP+0x0D760
int32_t d2cmp_d760(uint8_t* a1, uint32_t a2, uint32_t* ctx, uint32_t flag7270, const D2CmpCelEnv* e) {
    if (flag7270 == 0) return 0;
    return d2cmp_d76e(a1, a2, ctx, e);
}
