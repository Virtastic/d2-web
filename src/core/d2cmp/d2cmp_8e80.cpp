// src/core/d2cmp/d2cmp_8e80.cpp -- 1.13c D2CMP.dll+0x8E80 (the routine's entry is 0x8E70).
// Retail convention: ESI = pointer to a record whose first dword is a Storm handle, plain ret. If that handle is nonzero it is closed through
// Storm ordinal 252 (thunk 0x8E3C, stdcall, one stack word); the field is then cleared and the record is freed with Fog #10043 (ECX = record,
// EDX = file name RVA 0x2007C, stack line 0x26E, 0).
#include <cstdint>

struct D2Cmp8e80Env {
    void (*closeHandle)(uint32_t handle);               // 0x8E3C -> Storm 252
    void (*release)(uint32_t* block, uint32_t line);    // 0x8DA8 -> Fog 10043
};

// 1.13c D2CMP+0x8E80
extern "C" void d2cmp_8e80(uint32_t* esi, const D2Cmp8e80Env* env) {
    if (*esi) env->closeHandle(*esi);
    *esi = 0;
    env->release(esi, 0x26e);
}
