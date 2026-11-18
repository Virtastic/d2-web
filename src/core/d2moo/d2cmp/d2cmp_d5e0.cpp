// src/core/d2cmp/d2cmp_d5e0.cpp -- 1.13c D2CMP sub_D5E0, transcribed from the disassembly.
#include <cstdint>

struct D2CmpD5e0Env {
    // RVA 0x12A30 (fastcall: ECX unchanged = self, EDX = node, stack: g727c, table 0x6FE76240)
    void (*unlink)(uint32_t* self, uint32_t* node, uint32_t g727c, uint32_t table);
    uint32_t table;     // 0x6FE76240, the address of the cel-data hash in the module
};

// thiscall(ECX = self): node := self+8; node+0x24 := 0; 0x12A30(node, [0x6FE7727C], 0x6FE76240).
// 1.13c D2CMP+0x0D5E0
void d2cmp_d5e0(uint32_t* self, uint32_t g727c, const D2CmpD5e0Env* e) {
    uint32_t* node = (uint32_t*)(uintptr_t)self[2];
    node[9] = 0;
    e->unlink(self, node, g727c, e->table);
}
