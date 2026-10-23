// D2CMP sub_BD70 (ordinal 10089) transcribed from the cached Ghidra decompile. Globals and callees are seams.
#include <cstdint>
#include <cstring>

struct D2CMPBd70State {
    uint32_t active;        // DAT_6fe87f08
    uint32_t block[13];     // DAT_6fe872b0, 0xd dwords
    uint32_t tail;          // DAT_6fe87f0c
    void (*reg9880)(void*);   // 0x9880 registry shutdown
    void (*lru175c0)();       // 0x175C0 cache shutdown
};

// 1.13c D2CMP+0x0BD70
extern "C" void d2cmp_bd70(D2CMPBd70State& s, void* self, int32_t flush) {
    if (!s.active) { s.reg9880(self); return; }
    if (flush) s.lru175c0();
    std::memset(s.block, 0, sizeof s.block);
    s.tail = 0;
    s.reg9880(nullptr);
}
